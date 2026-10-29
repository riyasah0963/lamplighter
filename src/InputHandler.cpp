#include "InputHandler.h"

SDL_FPoint InputHandler::movementDirection() const
{
    // Scancodes are physical key positions, so WASD stays in the same place on non-QWERTY keyboards.
    const bool* keys = SDL_GetKeyboardState(nullptr);

    float x = 0.0f;
    float y = 0.0f;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  { x -= 1.0f; }
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) { x += 1.0f; }
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    { y -= 1.0f; }
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  { y += 1.0f; }

    return SDL_FPoint{ x, y };
}
