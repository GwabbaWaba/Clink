#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "render.hpp"
#include "main.hpp"

namespace rl = raylib;

std::vector<Light> lights = std::vector<Light>();

// Move a light and mark it as dirty so that we update it's mask next frame
void Light::move(float x, float y)
{
    this->dirty = true;
    this->position.x = x; 
    this->position.y = y;

    // update the cached bounds
    this->bounds.x = x - this->outer_radius;
    this->bounds.y = y - this->outer_radius;
}

// Compute a shadow volume for the edge
// It takes the edge and projects it back by the light radius and turns it into a quad
void computeShadowVolumeForEdge(Light* light, rl::Vector2 sp, rl::Vector2 ep) {
    float extension = light->outer_radius*2;

    rl::Vector2 spVector = Vector2Normalize(Vector2Subtract(sp, light->position));
    rl::Vector2 spProjection = Vector2Add(sp, Vector2Scale(spVector, extension));

    rl::Vector2 epVector = Vector2Normalize(Vector2Subtract(ep, light->position));
    rl::Vector2 epProjection = Vector2Add(ep, Vector2Scale(epVector, extension));

    light->shadows.push_back(ShadowGeometry{
        .vertices = {
            sp,
            ep,
            epProjection,
            spProjection
        }
    });
}

// Draw the light and shadows to the mask for a light
void Light::drawLightMask() {
    // Use the light mask
    rl::BeginTextureMode(this->mask);

        rl::ClearBackground(rl::WHITE);

        // Force the blend mode to only set the alpha of the destination
        rl::rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
        rl::rlSetBlendMode(rl::BLEND_CUSTOM);

        // If we are valid, then draw the light radius to the alpha mask
        if (this->valid) {
            DrawCircleGradient(
                as(int, this->position.x),
                as(int, this->position.y),
                this->outer_radius,
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
        for (auto& shadow : this->shadows) {
            rl::DrawTriangleFan(shadow.vertices, 4, rl::WHITE);
        }

        rl::rlDrawRenderBatchActive();
        
        // Go back to normal blend mode
        rl::rlSetBlendMode(rl::BLEND_ALPHA);

    rl::EndTextureMode();
}


std::shared_ptr<Light> LightRenderer::addLight(float x, float y, float radius) {
    auto mask = rl::LoadRenderTexture(rl::GetScreenWidth(), rl::GetScreenHeight());
    auto light = std::make_shared<Light>();
    light->outer_radius = radius;
    light->bounds = rl::Rectangle {
        .width = radius*2,
        .height = radius*2
    };
    light->mask = mask;

    light->move(x, y);
    // Force the render texture to have something in it
    light->drawLightMask();

    this->lights.push_back(light);
    return lights.back();
}

// See if a light needs to update it's mask
bool Light::update(std::vector<rl::Rectangle>& boxes) {
    if (!this->active || !this->dirty) return false;

    this->dirty = false;
    this->valid = false;

    this->shadows = std::vector<ShadowGeometry>();


    for (auto& box : boxes)
    {
        // Are we in a box? if so we are not valid
        if (raylib::CheckCollisionPointRec(this->position, box)) return false;

        // If this box is outside our bounds, we can skip it
        if (!raylib::CheckCollisionRecs(this->bounds, box)) continue;

        // Check the edges that are on the same side we are, and cast shadow volumes out from them
        
        // Top
        rl::Vector2 sp = { box.x, box.y };
        rl::Vector2 ep = { box.x + box.width, box.y };

        if (this->position.y > ep.y)
            computeShadowVolumeForEdge(this, sp, ep);

        // Right
        sp = ep;
        ep.y += box.height;
        if (this->position.x < ep.x)
            computeShadowVolumeForEdge(this, sp, ep);

        // Bottom
        sp = ep;
        ep.x -= box.width;
        if (this->position.y < ep.y)
            computeShadowVolumeForEdge(this, sp, ep);

        // Left
        sp = ep;
        ep.y -= box.height;
        if (this->position.x > ep.x)
            computeShadowVolumeForEdge(this, sp, ep);
    }

    this->valid = true;

    this->drawLightMask();

    return true;
}

// Set up some boxes
void setupBoxes(std::vector<rl::Rectangle>& boxes, int count)
{
    for (int i = 0; i < count; i++)
    {
        boxes.push_back(rl::Rectangle{
            as(float, rl::GetRandomValue(0,rl::GetScreenWidth())),
            as(float, rl::GetRandomValue(0,rl::GetScreenHeight())),
            as(float, rl::GetRandomValue(50,100)),
            as(float, rl::GetRandomValue(50,100))
        });
    }
}
