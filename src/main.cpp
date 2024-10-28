/*
 *  Code guide:
 *  use types in bitsizeints.h over standard numeric types
 *  when interfacing with c ABI, prefer standard numeric types
 *
 *  avoid using namespace as to upkeep clarity
 *  instead specifically declare using for very commonly used members of the namespace
 *  don't:
 *  using namespace std;
 *  do:
 *  using std::string;
 */

#include <chrono>
#include <filesystem>
#include <iostream>

#include "clinkinterface.hpp"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "render.hpp"
#include "mod_registry.hpp"
#include "event_register.hpp"

#include "bitsize_ints.h"
#include "clinkAPI.hpp"
#include "main.hpp"
#include "render.hpp"
#include "exceptions.hpp"
#include "registry.hpp"

namespace rl = raylib;
using std::optional;
using namespace chrono_literals;

// #include "zig.h"

ModRegister mod_register = ModRegister();
EventRegister event_register = EventRegister();

int main(int argc, char* argv[]) {
    // disable most logs
    rl::SetTraceLogLevel(rl::LOG_WARNING); 

    rl::SetConfigFlags(rl::FLAG_WINDOW_RESIZABLE);
    rl::InitWindow(
        rl::GetMonitorWidth(rl::GetCurrentMonitor()),
        rl::GetMonitorHeight(rl::GetCurrentMonitor()),
        "Clink"
    );
    rl::SetTargetFPS(60);
    
    ClinkAPI api;
    api = ClinkAPI {
        // TODO actually generate fresh ids
        .registerEvent = [&api](ModMemberNamespace event_name) {
            auto event_id = api.getEventIdByName(event_name);
            if(event_id == std::nullopt) {
                auto mod_id = mod_register.mod_ids.at(RegistryNamespace(event_name.mod_name));
                return event_register.registerEvent(event_name, mod_id);
            }

            std::cerr
                << "Duplicate event registry, errors may occur: "
                << event_name.mod_name
                << "::"
                << event_name.name
                << std::endl;
            return event_id.value();
        },
        .getEvent = [](ModMemberId& event_id) -> std::vector<VoidFn>& {
            return event_register.events.at(event_id);
        },
        .getEventByName = [](ModMemberNamespace event_name) -> std::vector<VoidFn>& {
            return event_register.events.at(event_register.event_ids.at(event_name));
        },
        .getEventIdByName = [](ModMemberNamespace event_name) -> optional<ModMemberId> {
            auto events = event_register.event_ids;
            if(events.find(event_name) == events.end())
                return {};

            return event_register.event_ids.at(event_name);
        },
        .subscribeToEvent = [&api](ModMemberNamespace event_name, VoidFn callback) {
            auto& event = api.getEventByName(event_name);
            event.push_back(callback);
            std::clog
                << "Subscribing "
                << (void*)callback
                << " to "
                << event_name.mod_name
                << "::"
                << event_name.name
                << std::endl;
        },
    };
    mod_register.loadMods("./base_mods", &api);

    auto update_event_id = api.registerEvent(ModMemberNamespace("clink", "update"));
    auto draw_event_id = api.registerEvent(ModMemberNamespace("clink", "draw"));
    auto key_press_event_id = api.registerEvent(ModMemberNamespace("clink", "key_press"));

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

    f32 dt = 0;
    debug::println("made it passed setup");
    while(!rl::WindowShouldClose()) {
        dt = rl::GetFrameTime();
        for(auto callback : api.getEvent(update_event_id)) {
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

            
            for(auto callback : api.getEvent(draw_event_id)) {
                as(fn(void), callback)();
            }
        rl::EndDrawing();
    }

    return 0;
}
