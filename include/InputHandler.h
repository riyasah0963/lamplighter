#pragma once

#include <SDL3/SDL.h>

// Turns the keyboard into game input, so the rest of the game never checks keys directly.
// Movement is read from SDL's live keyboard state every update, so holding a key moves smoothly.
// Discrete actions (light a lamp, open or shut the lantern) are added here as Command objects,
// so that keys can be rebound, as planned in the proposal.
class InputHandler
{
public:
    // The direction the player is pressing: x and y are each -1, 0 or 1 (both 0 when no key is held).
    SDL_FPoint movementDirection() const;
};
