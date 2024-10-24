#include <iostream>

#include <filesystem>
namespace fs = std::filesystem;

#include "render.hpp"
#include "mod_registry.hpp"
#include "event_register.hpp"

#include "bitsizeints.h"
#include "clinkAPI.hpp"
#include "main.hpp"
#include "render.hpp"

namespace raylib {
    #include <raylib.h>
}

namespace zig {
    #include "zig.h"
}


ModRegister mod_register = ModRegister();
EventRegister event_register = EventRegister();

int main(i32 argc, char* argv[]) {
    raylib::SetConfigFlags(raylib::FLAG_WINDOW_RESIZABLE);
    raylib::InitWindow(
        raylib::GetMonitorWidth(raylib::GetCurrentMonitor()),
        raylib::GetMonitorHeight(raylib::GetCurrentMonitor()),
        "Clink"
    );
    raylib::SetTargetFPS(60);
    
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

    raylib::Image img = raylib::GenImageChecked(
        64,
        64,
        32,
        32,
        raylib::BLACK,
        raylib::DARKGRAY
    );
    raylib::Texture2D background_texture = LoadTextureFromImage(img);
    raylib::UnloadImage(img);

    int boxCount = 0;
    raylib::Rectangle boxes[MAX_BOXES] = { 0 };
    SetupBoxes(boxes, &boxCount);

    raylib::RenderTexture light_mask = raylib::LoadRenderTexture(raylib::GetScreenWidth(), raylib::GetScreenHeight());

    int next_light = 0;
    
    bool show_lines = false;

    while(!raylib::WindowShouldClose()) {
        for(auto callback : event_register.events["clink::update"]) {
            as(fn(void, f32), callback)(1.0);
        }

        // Make a new light
        if (raylib::IsMouseButtonPressed(raylib::MOUSE_BUTTON_RIGHT) && (next_light < MAX_LIGHTS))
        {
            SetupLight(next_light, raylib::GetMousePosition().x, raylib::GetMousePosition().y, 200);
            next_light++;
        }

        // Toggle debug info
        if (raylib::IsKeyPressed(raylib::KEY_F1))
            show_lines = !show_lines;

        // Update the lights and keep track if any were dirty so we know if we need to update the master light mask
        bool dirty_lights = false;
        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            dirty_lights = UpdateLight(i, boxes, boxCount) || dirty_lights;
        }

        // Update the light mask
        if (dirty_lights)
        {
            // Build up the light mask
            raylib::BeginTextureMode(light_mask);
            
                raylib::ClearBackground(raylib::BLACK);

                // Force the blend mode to only set the alpha of the destination
                raylib::rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
                raylib::rlSetBlendMode(raylib::BLEND_CUSTOM);

                // Merge in all the light masks
                for (int i = 0; i < MAX_LIGHTS; i++)
                {
                    if (lights[i].active)
                        DrawTextureRec(
                            lights[i].mask.texture,
                            raylib::Rectangle{ 0, 0, as(float, raylib::GetScreenWidth()), -as(float, raylib::GetScreenHeight()) },
                            raylib::Vector2Zero(),
                            raylib::WHITE
                        );
                }

                raylib::rlDrawRenderBatchActive();

                // Go back to normal blend
                raylib::rlSetBlendMode(raylib::BLEND_ALPHA);
            raylib::EndTextureMode();
        }

        
        raylib::BeginDrawing();

            raylib::ClearBackground(raylib::BLACK);
            
            // Draw the tile background
            raylib::DrawTextureRec(
                background_texture,
                raylib::Rectangle{ 0, 0, (float)raylib::GetScreenWidth(), (float)raylib::GetScreenHeight() },
                raylib::Vector2Zero(),
                raylib::WHITE
            );
            
            // Overlay the shadows from all the lights
            raylib::DrawTextureRec(
                light_mask.texture,
                raylib::Rectangle{ 0, 0, as(float, raylib::GetScreenWidth()), -as(float, raylib::GetScreenHeight()) },
                raylib::Vector2Zero(),
                raylib::WHITE
            );


            // Draw the lights
            for (int i = 0; i < MAX_LIGHTS; i++)
            {
                if (lights[i].active)
                    DrawCircle(
                        as(int, lights[i].position.x),
                        as(int, lights[i].position.y),
                        10,
                        (i == 0)? raylib::YELLOW : raylib::WHITE
                    );
            }

            if (show_lines)
            {
                for (int s = 0; s < lights[0].shadowCount; s++)
                {
                    raylib::DrawTriangleFan(lights[0].shadows[s].vertices, 4, raylib::DARKPURPLE);
                }

                for (int b = 0; b < boxCount; b++)
                {
                    if (raylib::CheckCollisionRecs(boxes[b],lights[0].bounds))
                        raylib::DrawRectangleRec(boxes[b], raylib::PURPLE);

                    raylib::DrawRectangleLines(
                        as(int, boxes[b].x),
                        as(int, boxes[b].y),
                        as(int, boxes[b].width),
                        as(int, boxes[b].height),
                        raylib::DARKBLUE
                    );
                }

                raylib::DrawText("(F1) Hide Shadow Volumes", 10, 50, 10, raylib::GREEN);
            }
            else
            {
                raylib::DrawText("(F1) Show Shadow Volumes", 10, 50, 10, raylib::GREEN);
            }

            
            for(auto callback : event_register.events["clink::draw"]) {
                as(fn(void), callback)();
            }
        raylib::EndDrawing();
    }

    return 0;
}
