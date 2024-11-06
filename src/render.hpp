#pragma once

#include <memory>
#include <vector>

#include "raylib.h"

namespace rl = raylib;

// Custom Blend Modes
#define RLGL_SRC_ALPHA 0x000302
#define RLGL_MIN       0x008007
#define RLGL_MAX       0x008008

struct ShadowGeometry {
    rl::Vector2 vertices[4];
};

struct Light {
    bool active = true;             // Is this light slot active?
    bool dirty = true;              // Does this light need to be updated?
    bool valid = false;             // Is this light in a valid position?

    rl::Vector2 position;           // Light position
    rl::RenderTexture mask;         // Alpha mask for the light
    float outer_radius;             // The distance the light touches
    rl::Rectangle bounds;           // A cached rectangle of the light bounds to help with culling

    Light() = default;

    std::vector<ShadowGeometry> shadows;
    bool update(std::vector<rl::Rectangle>& boxes);
    void move(float x, float y);
    void drawLightMask();
};

class LightRenderer {
    public:
    std::vector<std::shared_ptr<Light>> lights;

    LightRenderer() = default;
    ~LightRenderer() = default;

    std::shared_ptr<Light> addLight(float x, float y, float radius);
};

extern std::vector<Light> lights;

void setupBoxes(std::vector<rl::Rectangle>& boxes, int count);
