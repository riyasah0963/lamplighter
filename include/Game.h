#pragma once

#include "SceneManager.h"
#include "SdlHandles.h"

// Owns the window, the renderer and the scenes, and runs the main loop (Game Loop pattern).
class Game
{
public:
    Game();  // throws std::runtime_error if SDL, the window or the renderer cannot be created
    void run();

private:
    void processEvents();
    void render();
    void toggleFullscreen();

    // Members are destroyed in reverse order of this list: the scenes first (they may use
    // the renderer), then the renderer, then the window, and SDL_Quit last.
    SdlContext sdl_;
    WindowPtr window_;
    RendererPtr renderer_;
    SceneManager scenes_;

    bool vsyncOn_ = false;
    bool quitRequested_ = false;
};
