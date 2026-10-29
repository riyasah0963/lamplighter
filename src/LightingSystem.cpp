#include "LightingSystem.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

#include "GameConstants.h"

namespace
{
    constexpr int kGlowSize = 256;  // pixels; scaled to each light's radius when drawn
}

LightingSystem::LightingSystem(int moonlight)
    : moonlight_(static_cast<Uint8>(std::clamp(moonlight, 0, 255)))
{
}

void LightingSystem::createTextures(SDL_Renderer* renderer)
{
    // Build the glow in memory: white in the middle, fading smoothly to black at the edge.
    // It is generated in code, so no image file is needed.
    SurfacePtr surface(SDL_CreateSurface(kGlowSize, kGlowSize, SDL_PIXELFORMAT_RGBA32));
    if (!surface)
    {
        throw std::runtime_error(std::string("SDL_CreateSurface failed: ") + SDL_GetError());
    }

    Uint8* pixels = static_cast<Uint8*>(surface->pixels);
    const float half = kGlowSize / 2.0f;
    for (int y = 0; y < kGlowSize; ++y)
    {
        for (int x = 0; x < kGlowSize; ++x)
        {
            // Distance from the centre: 0 in the middle, 1 at the edge of the circle.
            const float dx = (static_cast<float>(x) + 0.5f - half) / half;
            const float dy = (static_cast<float>(y) + 0.5f - half) / half;
            const float distance = std::sqrt(dx * dx + dy * dy);

            // Smoothstep curve: a full, bright middle that fades softly to nothing at the edge.
            const float s = std::clamp(1.0f - distance, 0.0f, 1.0f);
            const float strength = s * s * (3.0f - 2.0f * s);
            const Uint8 value = static_cast<Uint8>(255.0f * strength);

            // RGBA32 stores each pixel as four bytes in the order red, green, blue, alpha.
            Uint8* pixel = pixels + y * surface->pitch + x * 4;
            pixel[0] = value;
            pixel[1] = value;
            pixel[2] = value;
            pixel[3] = 255;
        }
    }

    glow_.reset(SDL_CreateTextureFromSurface(renderer, surface.get()));
    if (!glow_)
    {
        throw std::runtime_error(std::string("SDL_CreateTextureFromSurface failed: ") + SDL_GetError());
    }
    checkSdl(SDL_SetTextureBlendMode(glow_.get(), SDL_BLENDMODE_ADD), "SDL_SetTextureBlendMode");

    lightMap_.reset(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                      kLogicalWidth, kLogicalHeight));
    if (!lightMap_)
    {
        throw std::runtime_error(std::string("SDL_CreateTexture (light map) failed: ") + SDL_GetError());
    }
    checkSdl(SDL_SetTextureBlendMode(lightMap_.get(), SDL_BLENDMODE_MOD), "SDL_SetTextureBlendMode");
}

void LightingSystem::render(SDL_Renderer* renderer, const std::vector<Light>& lights)
{
    if (!lightMap_)
    {
        createTextures(renderer);
    }

    // 1. Draw into the light map instead of the window. It is exactly 1280x720, the same as the
    //    game's coordinate space, so light positions need no conversion.
    checkSdl(SDL_SetRenderTarget(renderer, lightMap_.get()), "SDL_SetRenderTarget");

    // The dark is slightly blue, like moonlight.
    const Uint8 moonBlue = static_cast<Uint8>(std::min(255, moonlight_ + moonlight_ / 2));
    checkSdl(SDL_SetRenderDrawColor(renderer, moonlight_, moonlight_, moonBlue, SDL_ALPHA_OPAQUE), "SDL_SetRenderDrawColor");
    checkSdl(SDL_RenderClear(renderer), "SDL_RenderClear");

    // 2. Add each light as a tinted glow, stretched to the light's size.
    for (const Light& light : lights)
    {
        checkSdl(SDL_SetTextureColorMod(glow_.get(), light.colour.r, light.colour.g, light.colour.b), "SDL_SetTextureColorMod");
        const SDL_FRect area{ light.centre.x - light.radius, light.centre.y - light.radius,
                              light.radius * 2.0f, light.radius * 2.0f };
        checkSdl(SDL_RenderTexture(renderer, glow_.get(), nullptr, &area), "SDL_RenderTexture");
    }

    // 3. Back to the window, and multiply the light map over the scene.
    checkSdl(SDL_SetRenderTarget(renderer, nullptr), "SDL_SetRenderTarget");
    const SDL_FRect screen{ 0.0f, 0.0f, static_cast<float>(kLogicalWidth), static_cast<float>(kLogicalHeight) };
    checkSdl(SDL_RenderTexture(renderer, lightMap_.get(), nullptr, &screen), "SDL_RenderTexture");
}
