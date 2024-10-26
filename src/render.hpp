#pragma once

#include <vector>

#include "raylib.h"

#include "bitsizeints.h"

// Custom Blend Modes
#define RLGL_SRC_ALPHA 0x000302
#define RLGL_MIN       0x008007
#define RLGL_MAX       0x008008

struct ShadowGeometry {
    raylib::Vector2 vertices[4];
};
using namespace std;

struct LightInfo {
    bool active;                // Is this light slot active?
    bool dirty;                 // Does this light need to be updated?
    bool valid;                 // Is this light in a valid position?

    raylib::Vector2 position;           // Light position
    raylib::RenderTexture mask;         // Alpha mask for the light
    f32 outerRadius;                    // The distance the light touches
    raylib::Rectangle bounds;           // A cached rectangle of the light bounds to help with culling

    vector<ShadowGeometry> shadows;
    int shadowCount;
};

extern vector<LightInfo> lights;

void addLight(float x, float y, float radius);
bool UpdateLight(int slot, vector<raylib::Rectangle>& boxes);
void MoveLight(int slot, float x, float y);
void SetupBoxes(vector<raylib::Rectangle>& boxes, int count);
