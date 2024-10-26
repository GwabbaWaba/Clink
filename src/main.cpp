#include <filesystem>
#include <iostream>

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

// #include "zig.h"

ModRegister mod_register = ModRegister();
EventRegister event_register = EventRegister();

int main(i32 argc, char* argv[]) {
    rl::SetConfigFlags(rl::FLAG_WINDOW_RESIZABLE);
    rl::InitWindow(
        rl::GetMonitorWidth(rl::GetCurrentMonitor()),
        rl::GetMonitorHeight(rl::GetCurrentMonitor()),
        "Clink"
    );
    rl::SetTargetFPS(60);
    
    auto api = ClinkAPI {
        .registerEvent = [](string eventName) {
            event_register.events[eventName] = vector<VoidFn>();
        },
        .getEvent = [](string eventName) {
            return event_register.events[eventName];
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
    SetupBoxes(boxes, 10);

    rl::RenderTexture light_mask = rl::LoadRenderTexture(rl::GetScreenWidth(), rl::GetScreenHeight());

    bool show_lines = false;

    while(!rl::WindowShouldClose()) {
        for(auto callback : event_register.events["clink::update"]) {
            as(fn(void, f32), callback)(1.0);
        }

        // Make a new light
        if (rl::IsMouseButtonPressed(rl::MOUSE_BUTTON_RIGHT))
        {
            addLight(rl::GetMousePosition().x, rl::GetMousePosition().y, 200);
        }

        // Toggle debug info
        if (rl::IsKeyPressed(rl::KEY_F1))
            show_lines = !show_lines;

        // Update the lights and keep track if any were dirty so we know if we need to update the master light mask
        bool dirty_lights = false;
        for (int i = 0; i < lights.size(); i++)
        {
            dirty_lights = UpdateLight(i, boxes) || dirty_lights;
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
                for (int i = 0; i < lights.size(); i++)
                {
                    if (lights[i].active)
                        DrawTextureRec(
                            lights[i].mask.texture,
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
                rl::Rectangle{ 0, 0, (float)rl::GetScreenWidth(), (float)rl::GetScreenHeight() },
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
            for (int i = 0; i < lights.size(); i++)
            {
                if (lights[i].active)
                    DrawCircle(
                        as(int, lights[i].position.x),
                        as(int, lights[i].position.y),
                        10,
                        (i == 0)? rl::YELLOW : rl::WHITE
                    );
            }

            if (show_lines)
            {
                for (int s = 0; s < lights[0].shadowCount; s++)
                {
                    rl::DrawTriangleFan(lights[0].shadows[s].vertices, 4, rl::DARKPURPLE);
                }

                for (int b = 0; b < boxes.size(); b++)
                {
                    if (rl::CheckCollisionRecs(boxes[b],lights[0].bounds))
                        rl::DrawRectangleRec(boxes[b], rl::PURPLE);

                    rl::DrawRectangleLines(
                        as(int, boxes[b].x),
                        as(int, boxes[b].y),
                        as(int, boxes[b].width),
                        as(int, boxes[b].height),
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
    }

    return 0;
}
