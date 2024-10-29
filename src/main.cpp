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

#include <filesystem>

#include "clinkinterface.hpp"
#include "player.hpp"
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
using namespace chrono_literals;
using namespace registry_literals;

// #include "zig.h"

ModRegister mod_register = ModRegister();
EventRegister event_register = EventRegister();

int main(int argc, char* argv[]) {
    #if defined(DEBUG)
    // disable most logs
    rl::SetTraceLogLevel(rl::LOG_WARNING);
    #else
    // release gets less logs
    rl::SetTraceLogLevel(rl::LOG_ERROR);
    #endif

    rl::SetWindowState(rl::FLAG_WINDOW_RESIZABLE);
    rl::InitWindow(
        rl::GetMonitorWidth(rl::GetCurrentMonitor()),
        rl::GetMonitorHeight(rl::GetCurrentMonitor()),
        "Clink"
    );
    rl::SetTargetFPS(60);

    Light& player_light = addLight(0, 0, 600);
    Player player = Player(&player_light);
    debug::println(&player);
    
    PlayerAPI player_api = PlayerAPI(player);
    ClinkAPI api = ClinkAPI(&player_api, event_register, mod_register);
    // load in engine mods
    // brings the clink namespace into the registry's scope
    auto base_mod_load_result = mod_register.loadMods("./base_mods", &api);
    // if base mods fail it will error later anyway
    if(base_mod_load_result != ModError::OK)
        exit(-1);

    // defined by clink base mod
    auto update_event_id = api.getEventIdByName("clink::update"_ModMember).value();
    auto draw_event_id = api.getEventIdByName("clink::draw"_ModMember).value();

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
            dirty_lights = light.update(boxes) || dirty_lights;
        }

        // Update the light mask
        if (dirty_lights) {
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
            
            // draw the boxes
            for (auto& box : boxes)
            {
                if (rl::CheckCollisionRecs(box,lights[0].bounds))
                    rl::DrawRectangleRec(box, rl::GRAY);

                rl::DrawRectangleLines(
                    as(int, box.x),
                    as(int, box.y),
                    as(int, box.width),
                    as(int, box.height),
                    rl::BLACK
                );
            }

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
