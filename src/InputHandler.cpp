#include "InputHandler.h"

#include <utility>

InputHandler::InputHandler()
{
    bind(SDLK_E, std::make_unique<LightLampCommand>());
    bind(SDLK_SPACE, std::make_unique<ToggleLanternCommand>());
}

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

const Command* InputHandler::commandFor(const SDL_Event& event) const
{
    // Ignore held-key repeats, so holding Space does not flick the lantern on and off.
    if (event.type != SDL_EVENT_KEY_DOWN || event.key.repeat)
    {
        return nullptr;
    }

    const auto binding = bindings_.find(event.key.key);
    return binding == bindings_.end() ? nullptr : binding->second.get();
}

void InputHandler::bind(SDL_Keycode key, std::unique_ptr<Command> command)
{
    bindings_[key] = std::move(command);
}
