#pragma once

// Small drawing helpers for the placeholder scenes.

#include <SDL3/SDL.h>

#include <string>

#include "GameConstants.h"
#include "SdlHandles.h"

// Fills the whole 1280x720 game area with one colour. An alpha below 255 tints whatever is already drawn.
inline void fillScreen(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a = SDL_ALPHA_OPAQUE)
{
    const SDL_FRect screen{ 0.0f, 0.0f, static_cast<float>(kLogicalWidth), static_cast<float>(kLogicalHeight) };
    checkSdl(SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND), "SDL_SetRenderDrawBlendMode");
    checkSdl(SDL_SetRenderDrawColor(renderer, r, g, b, a), "SDL_SetRenderDrawColor");
    checkSdl(SDL_RenderFillRect(renderer, &screen), "SDL_RenderFillRect");
}

// Draws one line of text centred across the screen, in SDL3's built-in 8x8 debug font enlarged by `scale`.
// Temporary: the BitmapFont class from the proposal replaces this.
inline void drawCentredText(SDL_Renderer* renderer, float y, const std::string& text, float scale)
{
    const float charSize = static_cast<float>(SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE);
    const float textWidth = static_cast<float>(text.size()) * charSize * scale;
    const float x = (static_cast<float>(kLogicalWidth) - textWidth) / 2.0f;

    checkSdl(SDL_SetRenderDrawColor(renderer, 235, 225, 200, SDL_ALPHA_OPAQUE), "SDL_SetRenderDrawColor");  // warm lamplight white

    // The render scale enlarges everything drawn, positions included, so the position is divided by it.
    checkSdl(SDL_SetRenderScale(renderer, scale, scale), "SDL_SetRenderScale");
    checkSdl(SDL_RenderDebugText(renderer, x / scale, y / scale, text.c_str()), "SDL_RenderDebugText");
    checkSdl(SDL_SetRenderScale(renderer, 1.0f, 1.0f), "SDL_SetRenderScale");
}
