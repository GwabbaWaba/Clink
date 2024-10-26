#include <iostream>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "render.hpp"
#include "main.hpp"
#include "bitsizeints.h"

namespace rl = raylib;

vector<LightInfo> lights = vector<LightInfo>();

// Move a light and mark it as dirty so that we update it's mask next frame
void MoveLight(int slot, float x, float y)
{
    lights[slot].dirty = true;
    lights[slot].position.x = x; 
    lights[slot].position.y = y;

    // update the cached bounds
    lights[slot].bounds.x = x - lights[slot].outerRadius;
    lights[slot].bounds.y = y - lights[slot].outerRadius;
}

// Compute a shadow volume for the edge
// It takes the edge and projects it back by the light radius and turns it into a quad
void ComputeShadowVolumeForEdge(int slot, rl::Vector2 sp, rl::Vector2 ep)
{
    float extension = lights[slot].outerRadius*2;

    rl::Vector2 spVector = Vector2Normalize(Vector2Subtract(sp, lights[slot].position));
    rl::Vector2 spProjection = Vector2Add(sp, Vector2Scale(spVector, extension));

    rl::Vector2 epVector = Vector2Normalize(Vector2Subtract(ep, lights[slot].position));
    rl::Vector2 epProjection = Vector2Add(ep, Vector2Scale(epVector, extension));

    lights[slot].shadows[lights[slot].shadowCount].vertices[0] = sp;
    lights[slot].shadows[lights[slot].shadowCount].vertices[1] = ep;
    lights[slot].shadows[lights[slot].shadowCount].vertices[2] = epProjection;
    lights[slot].shadows[lights[slot].shadowCount].vertices[3] = spProjection;

    lights[slot].shadowCount++;
}

// Draw the light and shadows to the mask for a light
void DrawLightMask(int slot)
{
    // Use the light mask
    rl::BeginTextureMode(lights[slot].mask);

        rl::ClearBackground(rl::WHITE);

        // Force the blend mode to only set the alpha of the destination
        rl::rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
        rl::rlSetBlendMode(rl::BLEND_CUSTOM);

        // If we are valid, then draw the light radius to the alpha mask
        if (lights[slot].valid) {
            DrawCircleGradient((int)lights[slot].position.x, (int)lights[slot].position.y, lights[slot].outerRadius, ColorAlpha(rl::WHITE, 0), rl::WHITE);
        }
        
        rl::rlDrawRenderBatchActive();

        // Cut out the shadows from the light radius by forcing the alpha to maximum
        rl::rlSetBlendMode(rl::BLEND_ALPHA);
        rl::rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MAX);
        rl::rlSetBlendMode(rl::BLEND_CUSTOM);

        // Draw the shadows to the alpha mask
        for (int i = 0; i < lights[slot].shadowCount; i++)
        {
            rl::DrawTriangleFan(lights[slot].shadows[i].vertices, 4, rl::WHITE);
        }

        rl::rlDrawRenderBatchActive();
        
        // Go back to normal blend mode
        rl::rlSetBlendMode(rl::BLEND_ALPHA);

    rl::EndTextureMode();
}


// Setup a light
void addLight(float x, float y, float radius)
{
    LightInfo light = LightInfo {
        .active = true,
        .valid = false, // The light must prove it is valid
        .mask = rl::LoadRenderTexture(rl::GetScreenWidth(), rl::GetScreenHeight()),
        .outerRadius = radius,
        .bounds = {
            .width = radius*2,
            .height = radius*2
        }
    };
    usize index = lights.size();
    lights.push_back(light);

    MoveLight(index, x, y);

    // Force the render texture to have something in it
    DrawLightMask(index);
}

// See if a light needs to update it's mask
bool UpdateLight(int slot, vector<rl::Rectangle>& boxes)
{
    if (!lights[slot].active || !lights[slot].dirty) return false;

    lights[slot].dirty = false;
    lights[slot].shadowCount = 0;
    lights[slot].valid = false;


    for (int i = 0; i < boxes.size(); i++)
    {
        // Are we in a box? if so we are not valid
        if (raylib::CheckCollisionPointRec(lights[slot].position, boxes[i])) return false;

        // If this box is outside our bounds, we can skip it
        if (!raylib::CheckCollisionRecs(lights[slot].bounds, boxes[i])) continue;

        // Check the edges that are on the same side we are, and cast shadow volumes out from them
        
        // Top
        rl::Vector2 sp = rl::Vector2{ boxes[i].x, boxes[i].y };
        rl::Vector2 ep = rl::Vector2{ boxes[i].x + boxes[i].width, boxes[i].y };

        if (lights[slot].position.y > ep.y) ComputeShadowVolumeForEdge(slot, sp, ep);

        // Right
        sp = ep;
        ep.y += boxes[i].height;
        if (lights[slot].position.x < ep.x) ComputeShadowVolumeForEdge(slot, sp, ep);

        // Bottom
        sp = ep;
        ep.x -= boxes[i].width;
        if (lights[slot].position.y < ep.y) ComputeShadowVolumeForEdge(slot, sp, ep);

        // Left
        sp = ep;
        ep.y -= boxes[i].height;
        if (lights[slot].position.x > ep.x) ComputeShadowVolumeForEdge(slot, sp, ep);

        // The box itself
        debug::println("TEST");
        lights[slot].shadows[lights[slot].shadowCount].vertices[0] = rl::Vector2{ boxes[i].x, boxes[i].y };
        lights[slot].shadows[lights[slot].shadowCount].vertices[1] = rl::Vector2{ boxes[i].x, boxes[i].y + boxes[i].height };
        lights[slot].shadows[lights[slot].shadowCount].vertices[2] = rl::Vector2{ boxes[i].x + boxes[i].width, boxes[i].y + boxes[i].height };
        lights[slot].shadows[lights[slot].shadowCount].vertices[3] = rl::Vector2{ boxes[i].x + boxes[i].width, boxes[i].y };
        lights[slot].shadowCount++;
        debug::println("TEST2");
    }

    lights[slot].valid = true;

    DrawLightMask(slot);

    return true;
}

// Set up some boxes
void SetupBoxes(vector<rl::Rectangle>& boxes, int count)
{
    if(count < 1) return;
    boxes.push_back(rl::Rectangle{ 150,80, 40, 40 });
    if(count < 2) return;
    boxes.push_back(rl::Rectangle{ 1200, 700, 40, 40 });
    if(count < 3) return;
    boxes.push_back(rl::Rectangle{ 200, 600, 40, 40 });
    if(count < 4) return;
    boxes.push_back(rl::Rectangle{ 1000, 50, 40, 40 });
    if(count < 5) return;
    boxes.push_back(rl::Rectangle{ 500, 350, 40, 40 });

    for (int i = 5; i < count; i++)
    {
        boxes.push_back(rl::Rectangle{
            (float)rl::GetRandomValue(0,rl::GetScreenWidth()),
            (float)rl::GetRandomValue(0,rl::GetScreenHeight()),
            (float)rl::GetRandomValue(10,100),
            (float)rl::GetRandomValue(10,100)
        });
    }
}
