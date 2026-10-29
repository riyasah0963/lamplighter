#pragma once

#include <SDL3/SDL.h>

#include <vector>

#include "SdlHandles.h"

// One round light: its centre and radius in screen pixels, and its colour.
struct Light
{
    SDL_FPoint centre;
    float radius;
    SDL_Color colour;
};

// Draws the night over the scene, with pools of light cut out of it.
//
// How it works (the light map technique from the proposal):
// 1. A 1280x720 "light map" texture is cleared to the colour of the dark (dim moonlight).
// 2. Each light is drawn onto it as a soft round glow, with additive blending so overlapping lights add up.
// 3. The light map is drawn over the finished scene with multiply (modulate) blending: where the
//    light map is white the scene shows at full brightness, where it is dark the scene is darkened.
class LightingSystem
{
public:
    // moonlight: brightness of unlit areas, 0 (pitch black) to 255.
    explicit LightingSystem(int moonlight);

    // Darkens everything drawn so far, except where the lights are. Call after drawing the scene
    // and before drawing the HUD, so the HUD stays readable.
    void render(SDL_Renderer* renderer, const std::vector<Light>& lights);

private:
    // The textures need a renderer, which scenes only receive when drawing, so they are made on first use.
    void createTextures(SDL_Renderer* renderer);

    Uint8 moonlight_;
    TexturePtr glow_;      // one soft white circle, tinted and scaled for every light
    TexturePtr lightMap_;  // the render target the lights are drawn onto
};
