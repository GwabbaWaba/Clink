#include <iostream>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "render.hpp"
#include "main.hpp"

namespace rl = raylib;

vector<LightInfo> lights = vector<LightInfo>();

// Move a light and mark it as dirty so that we update it's mask next frame
void moveLight(LightInfo& light, float x, float y)
{
    light.dirty = true;
    light.position.x = x; 
    light.position.y = y;

    // update the cached bounds
    light.bounds.x = x - light.outerRadius;
    light.bounds.y = y - light.outerRadius;
}

// Compute a shadow volume for the edge
// It takes the edge and projects it back by the light radius and turns it into a quad
void computeShadowVolumeForEdge(LightInfo& light, rl::Vector2 sp, rl::Vector2 ep) {
    float extension = light.outerRadius*2;

    rl::Vector2 spVector = Vector2Normalize(Vector2Subtract(sp, light.position));
    rl::Vector2 spProjection = Vector2Add(sp, Vector2Scale(spVector, extension));

    rl::Vector2 epVector = Vector2Normalize(Vector2Subtract(ep, light.position));
    rl::Vector2 epProjection = Vector2Add(ep, Vector2Scale(epVector, extension));

    light.shadows.push_back(ShadowGeometry{
        .vertices = {
            sp,
            ep,
            epProjection,
            spProjection
        }
    });
}

// Draw the light and shadows to the mask for a light
void drawLightMask(LightInfo& light) {
    // Use the light mask
    rl::BeginTextureMode(light.mask);

        rl::ClearBackground(rl::WHITE);

        // Force the blend mode to only set the alpha of the destination
        rl::rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
        rl::rlSetBlendMode(rl::BLEND_CUSTOM);

        // If we are valid, then draw the light radius to the alpha mask
        if (light.valid) {
            DrawCircleGradient(
                as(int, light.position.x),
                as(int, light.position.y),
                light.outerRadius,
                ColorAlpha(rl::WHITE, 0),
                rl::WHITE
            );
        }
        
        rl::rlDrawRenderBatchActive();

        // Cut out the shadows from the light radius by forcing the alpha to maximum
        rl::rlSetBlendMode(rl::BLEND_ALPHA);
        rl::rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MAX);
        rl::rlSetBlendMode(rl::BLEND_CUSTOM);

        // Draw the shadows to the alpha mask
        for (auto& shadow : light.shadows) {
            rl::DrawTriangleFan(shadow.vertices, 4, rl::WHITE);
        }

        rl::rlDrawRenderBatchActive();
        
        // Go back to normal blend mode
        rl::rlSetBlendMode(rl::BLEND_ALPHA);

    rl::EndTextureMode();
}


// Setup a light
void addLight(float x, float y, float radius)
{
    LightInfo light = {
        .active = true,
        .valid = false, // The light must prove it is valid
        .mask = rl::LoadRenderTexture(rl::GetScreenWidth(), rl::GetScreenHeight()),
        .outerRadius = radius,
        .bounds = {
            .width = radius*2,
            .height = radius*2
        }
    };
    moveLight(light, x, y);
    // Force the render texture to have something in it
    drawLightMask(light);

    lights.push_back(light);
}

// See if a light needs to update it's mask
bool updateLight(LightInfo& light, vector<rl::Rectangle>& boxes)
{
    if (!light.active || !light.dirty) return false;

    light.dirty = false;
    light.valid = false;


    for (auto& box : boxes)
    {
        // Are we in a box? if so we are not valid
        if (raylib::CheckCollisionPointRec(light.position, box)) return false;

        // If this box is outside our bounds, we can skip it
        if (!raylib::CheckCollisionRecs(light.bounds, box)) continue;

        // Check the edges that are on the same side we are, and cast shadow volumes out from them
        
        // Top
        rl::Vector2 sp = { box.x, box.y };
        rl::Vector2 ep = { box.x + box.width, box.y };

        if (light.position.y > ep.y)
            computeShadowVolumeForEdge(light, sp, ep);

        // Right
        sp = ep;
        ep.y += box.height;
        if (light.position.x < ep.x)
            computeShadowVolumeForEdge(light, sp, ep);

        // Bottom
        sp = ep;
        ep.x -= box.width;
        if (light.position.y < ep.y)
            computeShadowVolumeForEdge(light, sp, ep);

        // Left
        sp = ep;
        ep.y -= box.height;
        if (light.position.x > ep.x)
            computeShadowVolumeForEdge(light, sp, ep);

        // The box itself
        light.shadows.push_back(ShadowGeometry{
            .vertices = {
                { box.x, box.y },
                { box.x, box.y + box.height },
                { box.x + box.width, box.y + box.height },
                { box.x + box.width, box.y }
            }
        });
    }

    light.valid = true;

    drawLightMask(light);

    return true;
}

// Set up some boxes
void setupBoxes(vector<rl::Rectangle>& boxes, int count)
{
    for (int i = 0; i < count; i++)
    {
        boxes.push_back(rl::Rectangle{
            as(float, rl::GetRandomValue(0,rl::GetScreenWidth())),
            as(float, rl::GetRandomValue(0,rl::GetScreenHeight())),
            as(float, rl::GetRandomValue(10,100)),
            as(float, rl::GetRandomValue(10,100))
        });
    }
}
