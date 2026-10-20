#pragma once

#include <SDL3/SDL.h>

class SceneManager;

// One screen of the game: menu, play, pause or result. Each scene is one state of the game (State pattern).
// Each frame the top scene gets its events and zero or more fixed-step updates (Update Method pattern);
// render() is called for it and for any scene visible underneath an overlay.
class Scene
{
public:
    explicit Scene(SceneManager& scenes) : scenes_(scenes) {}
    virtual ~Scene() = default;

    // Scenes are owned by the SceneManager through unique_ptr and never copied.
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    virtual void handleEvent(const SDL_Event& event) = 0;
    virtual void update(double dt) = 0;               // dt is always the fixed step, in seconds
    virtual void render(SDL_Renderer* renderer) = 0;  // the renderer is borrowed from Game, not owned

    // An overlay (Pause) is drawn on top of the scene below it instead of hiding it.
    virtual bool isOverlay() const { return false; }

protected:
    // Scenes use this to ask for a change of scene (see SceneManager).
    SceneManager& scenes() const { return scenes_; }

    // True only for the first press of a key, not for the repeats sent while it is held down,
    // so holding Esc does not flick between Play and Pause.
    static bool isKeyPress(const SDL_Event& event, SDL_Keycode key)
    {
        return event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.key == key;
    }

private:
    SceneManager& scenes_;
};
