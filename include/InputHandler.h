#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <unordered_map>

#include "Command.h"

// Turns the keyboard into game input, so the rest of the game never checks keys directly.
// Movement is read from SDL's live keyboard state every update, so holding a key moves smoothly.
// One-off actions (light a lamp, open or shut the lantern) are Command objects bound to keys.
class InputHandler
{
public:
    InputHandler();  // sets the default keys: E lights a lamp, Space opens or shuts the lantern

    // The direction the player is pressing: x and y are each -1, 0 or 1 (both 0 when no key is held).
    SDL_FPoint movementDirection() const;

    // The command bound to this key press, or nullptr if the event is not a bound key press.
    const Command* commandFor(const SDL_Event& event) const;

    // Binds a key to a command, replacing whatever that key did before.
    void bind(SDL_Keycode key, std::unique_ptr<Command> command);

private:
    std::unordered_map<SDL_Keycode, std::unique_ptr<Command>> bindings_;
};
