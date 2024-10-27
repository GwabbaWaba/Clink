/*
 *  Code guide:
 *  use types in bitsizeints.h over standard numeric types
 *  when interfacing with c ABI, prefer standard numeric types
 */

#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "render.hpp"
#include "mod_registry.hpp"
#include "event_register.hpp"

#include "bitsizeints.h"
#include "clinkAPI.hpp"
#include "main.hpp"
#include "render.hpp"

namespace rl = raylib;
using namespace chrono_literals;

// #include "zig.h"

ModRegister mod_register = ModRegister();
EventRegister event_register = EventRegister();

int main(int argc, char* argv[]) {
    rl::SetConfigFlags(rl::FLAG_WINDOW_RESIZABLE);
    rl::InitWindow(
        rl::GetMonitorWidth(rl::GetCurrentMonitor()),
        rl::GetMonitorHeight(rl::GetCurrentMonitor()),
        "Clink"
    );
    rl::SetTargetFPS(60);
    
    auto api = ClinkAPI {
        .registerEvent = [](string event_name) {
            auto& events = event_register.events;
            if(events.find(event_name) == events.end()) {
                events[event_name] = vector<VoidFn>();
            } else {
                std::cerr
                    << "Duplicate event registry, errors may occur: "
                    << event_name
                    << std::endl;
            }
        },
        .getEvent = [](string event_name) {
            auto& events = event_register.events;
            if(events.find(event_name) != events.end())
                return events[event_name];

            std::stringstream err_msg_stream = std::stringstream();
            err_msg_stream
                << "Access of unregistered event: "
                << event_name;
            
            throw clink_exceptions::EventException(
                clink_exceptions::EventExceptionType::invalid_access,
                err_msg_stream.str()
            );
        },
        .subscribeToEvent = [](string eventName, VoidFn callback) {
            auto event = &event_register.events[eventName];
            event->push_back(callback);
            std::clog << "Subscribing " << (void*)callback << " to " << eventName << std::endl;
        },
    };
    api.registerEvent("clink::update");
    api.registerEvent("clink::draw");

    api.registerEvent("clink::key");

    mod_register.loadMods("./mods", &api);

    rl::Image img = rl::GenImageChecked(
        64,
        64,
        32,
        32,
        rl::BLACK,
        rl::DARKGRAY
    );
    rl::Texture2D background_texture = LoadTextureFromImage(img);
    rl::UnloadImage(img);

    vector<rl::Rectangle> boxes = vector<rl::Rectangle>();
    setupBoxes(boxes, 20);

    rl::RenderTexture light_mask = rl::LoadRenderTexture(rl::GetScreenWidth(), rl::GetScreenHeight());

    bool show_lines = false;

    auto last_time = std::chrono::steady_clock::now();
    f32 dt = 0;

    while(!rl::WindowShouldClose()) {
        auto current_time = std::chrono::steady_clock::now();
        dt = (current_time - last_time) / 1s;
        for(auto callback : event_register.events["clink::update"]) {
            as(fn(void, f32), callback)(dt);
        }

        // Make a new light
        if (rl::IsMouseButtonPressed(rl::MOUSE_BUTTON_RIGHT)) {
            addLight(rl::GetMousePosition().x, rl::GetMousePosition().y, 200);
        }

        // Toggle debug info
        if (rl::IsKeyPressed(rl::KEY_F1))
            show_lines = !show_lines;

        // Update the lights and keep track if any were dirty so we know if we need to update the master light mask
        bool dirty_lights = false;
        for(auto& light: lights) {
            dirty_lights = updateLight(light, boxes) || dirty_lights;
        }

        // Update the light mask
        if (dirty_lights)
        {
            // Build up the light mask
            rl::BeginTextureMode(light_mask);
            
                rl::ClearBackground(rl::BLACK);

                // Force the blend mode to only set the alpha of the destination
                rl::rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
                rl::rlSetBlendMode(rl::BLEND_CUSTOM);

                // Merge in all the light masks
                for(auto& light : lights) {
                    if (light.active)
                        DrawTextureRec(
                            light.mask.texture,
                            rl::Rectangle{ 0, 0, as(float, rl::GetScreenWidth()), -as(float, rl::GetScreenHeight()) },
                            rl::Vector2Zero(),
                            rl::WHITE
                    );
                }

                rl::rlDrawRenderBatchActive();

                // Go back to normal blend
                rl::rlSetBlendMode(rl::BLEND_ALPHA);
            rl::EndTextureMode();
        }

        
        rl::BeginDrawing();

            rl::ClearBackground(rl::BLACK);
            
            // Draw the tile background
            rl::DrawTextureRec(
                background_texture,
                rl::Rectangle{ 0, 0, as(float, rl::GetScreenWidth()), as(float, rl::GetScreenHeight()) },
                rl::Vector2Zero(),
                rl::WHITE
            );
            
            // Overlay the shadows from all the lights
            rl::DrawTextureRec(
                light_mask.texture,
                rl::Rectangle{ 0, 0, as(float, rl::GetScreenWidth()), -as(float, rl::GetScreenHeight()) },
                rl::Vector2Zero(),
                rl::WHITE
            );


            // Draw the lights
            for (auto& light : lights)
            {
                if (light.active)
                    DrawCircle(
                        as(int, light.position.x),
                        as(int, light.position.y),
                        10,
                        rl::WHITE
                    );
            }

            if (show_lines)
            {
                for (auto& shadow : lights[0].shadows)
                {
                    rl::DrawTriangleFan(shadow.vertices, 4, rl::DARKPURPLE);
                }

                for (auto& box : boxes)
                {
                    if (rl::CheckCollisionRecs(box,lights[0].bounds))
                        rl::DrawRectangleRec(box, rl::PURPLE);

                    rl::DrawRectangleLines(
                        as(int, box.x),
                        as(int, box.y),
                        as(int, box.width),
                        as(int, box.height),
                        rl::DARKBLUE
                    );
                }

                rl::DrawText("(F1) Hide Shadow Volumes", 10, 50, 10, rl::GREEN);
            }
            else
            {
                rl::DrawText("(F1) Show Shadow Volumes", 10, 50, 10, rl::GREEN);
            }

            
            for(auto callback : event_register.events["clink::draw"]) {
                as(fn(void), callback)();
            }
        rl::EndDrawing();

        last_time = std::chrono::system_clock::now();
    }

    return 0;
}
