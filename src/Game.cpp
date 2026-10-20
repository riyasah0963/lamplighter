#include "Game.h"

#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>

#include "GameConstants.h"
#include "scenes/MenuScene.h"

namespace
{
    WindowPtr createWindow()
    {
        WindowPtr window(SDL_CreateWindow("Lamplighter", kLogicalWidth, kLogicalHeight, SDL_WINDOW_RESIZABLE));
        if (!window)
        {
            throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
        }
        return window;
    }

    RendererPtr createRenderer(SDL_Window* window)
    {
        RendererPtr renderer(SDL_CreateRenderer(window, nullptr));
        if (!renderer)
        {
            throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
        }
        return renderer;
    }

    // Logging both versions makes the program load SDL3_image.dll at start-up, so a missing DLL is caught now.
    void logLibraryVersions()
    {
        const int sdlVersion = SDL_GetVersion();
        const int imageVersion = IMG_Version();
        SDL_Log("SDL %d.%d.%d, SDL_image %d.%d.%d",
                SDL_VERSIONNUM_MAJOR(sdlVersion), SDL_VERSIONNUM_MINOR(sdlVersion), SDL_VERSIONNUM_MICRO(sdlVersion),
                SDL_VERSIONNUM_MAJOR(imageVersion), SDL_VERSIONNUM_MINOR(imageVersion), SDL_VERSIONNUM_MICRO(imageVersion));
    }
}

Game::Game()
    : sdl_(SDL_INIT_VIDEO)
    , window_(createWindow())
    , renderer_(createRenderer(window_.get()))
{
    logLibraryVersions();

    // Everything is drawn in a fixed 1280x720 space. SDL scales it to whatever size the window is
    // and adds black bars where needed, so the game keeps its shape on any monitor.
    checkSdl(SDL_SetRenderLogicalPresentation(renderer_.get(), kLogicalWidth, kLogicalHeight,
                                              SDL_LOGICAL_PRESENTATION_LETTERBOX),
             "SDL_SetRenderLogicalPresentation");

    // VSync limits drawing to the monitor's refresh rate. Without it the loop sleeps briefly each frame instead.
    vsyncOn_ = SDL_SetRenderVSync(renderer_.get(), 1);
    if (!vsyncOn_)
    {
        SDL_Log("VSync unavailable, using a short sleep each frame instead: %s", SDL_GetError());
    }

    scenes_.requestSwitchTo(std::make_unique<MenuScene>(scenes_));
    scenes_.applyPendingChanges();
}

// Fixed-timestep game loop, following Nystrom (2014) "Game Loop", Game Programming Patterns,
// https://gameprogrammingpatterns.com/game-loop.html
// Real elapsed time is added to an accumulator, and the game logic is updated in equal steps of
// 1/60 s for as many steps as fit. The game therefore runs at the same speed on a fast lab PC and
// a slow laptop. Drawing happens once per pass of the loop.
void Game::run()
{
    constexpr double kStepSeconds = 1.0 / kUpdatesPerSecond;

    // After a long stall (for example while the window is being dragged), only catch up a quarter
    // of a second. Otherwise the loop would run hundreds of updates at once and fall further behind.
    constexpr double kMaxFrameSeconds = 0.25;

    const double counterTicksPerSecond = static_cast<double>(SDL_GetPerformanceFrequency());
    Uint64 previousCounter = SDL_GetPerformanceCounter();
    double accumulator = 0.0;

    while (!quitRequested_ && !scenes_.empty())
    {
        const Uint64 counter = SDL_GetPerformanceCounter();
        const double frameSeconds = static_cast<double>(counter - previousCounter) / counterTicksPerSecond;
        previousCounter = counter;
        accumulator += std::min(frameSeconds, kMaxFrameSeconds);

        processEvents();

        while (accumulator >= kStepSeconds && !scenes_.empty())
        {
            scenes_.update(kStepSeconds);
            scenes_.applyPendingChanges();
            accumulator -= kStepSeconds;
        }

        // The menu may have just been popped (Esc to quit), leaving nothing to draw.
        if (!scenes_.empty())
        {
            render();
        }

        if (!vsyncOn_)
        {
            SDL_Delay(1);  // give the CPU a rest when VSync is not pacing the loop
        }
    }
}

void Game::processEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)  // the window's close button
        {
            quitRequested_ = true;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.key == SDLK_F11)
        {
            toggleFullscreen();
        }
        else
        {
            scenes_.handleEvent(event);
            scenes_.applyPendingChanges();
        }
    }
}

void Game::render()
{
    SDL_Renderer* renderer = renderer_.get();

    // Clear to black first, so the letterbox bars around the 16:9 game area are black.
    checkSdl(SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE), "SDL_SetRenderDrawColor");
    checkSdl(SDL_RenderClear(renderer), "SDL_RenderClear");

    scenes_.render(renderer);

    checkSdl(SDL_RenderPresent(renderer), "SDL_RenderPresent");
}

void Game::toggleFullscreen()
{
    const bool isFullscreen = (SDL_GetWindowFlags(window_.get()) & SDL_WINDOW_FULLSCREEN) != 0;

    // Failing to change mode is not worth ending the game over, so it is only logged.
    if (!SDL_SetWindowFullscreen(window_.get(), !isFullscreen))
    {
        SDL_Log("Could not toggle fullscreen: %s", SDL_GetError());
    }
}
