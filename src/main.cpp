// Lamplighter entry point.
// Week 4 milestone: open the game window and run a basic loop until the player quits.
// The Game class from the UML sketch takes over the loop in a later step.

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // Must be included where main() is defined: on Windows it supplies the WinMain that calls our main()
#include <SDL3_image/SDL_image.h>

#include <stdexcept>
#include <string>

#include "SdlHandles.h"

namespace
{
    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 720;
    constexpr Uint32 kFallbackFrameDelayMs = 16; // about 60 fps, used only if VSync is unavailable

    // Most SDL3 functions return false on failure; turn that into an exception with SDL's reason.
    void check(bool succeeded, const std::string& call)
    {
        if (!succeeded)
        {
            throw std::runtime_error(call + " failed: " + SDL_GetError());
        }
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

    void run()
    {
        // Declaration order matters: objects are destroyed in reverse, so the renderer goes first,
        // then the window, and SDL_Quit runs last.
        SdlContext sdl(SDL_INIT_VIDEO);
        logLibraryVersions();

        WindowPtr window(SDL_CreateWindow("Lamplighter", kWindowWidth, kWindowHeight, 0));
        if (!window)
        {
            throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
        }

        RendererPtr renderer(SDL_CreateRenderer(window.get(), nullptr));
        if (!renderer)
        {
            throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
        }

        // VSync caps the loop to the monitor's refresh rate. Without it, a short delay stops the loop using 100% CPU.
        const bool vsyncOn = SDL_SetRenderVSync(renderer.get(), 1);
        if (!vsyncOn)
        {
            SDL_Log("VSync unavailable, using a frame delay instead: %s", SDL_GetError());
        }

        bool running = true;
        while (running)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                // SDL_EVENT_QUIT is sent when the window's close button is pressed
                if (event.type == SDL_EVENT_QUIT)
                {
                    running = false;
                }
                else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                {
                    running = false;
                }
            }

            check(SDL_SetRenderDrawColor(renderer.get(), 12, 14, 28, SDL_ALPHA_OPAQUE), "SDL_SetRenderDrawColor"); // night blue
            check(SDL_RenderClear(renderer.get()), "SDL_RenderClear");
            check(SDL_RenderPresent(renderer.get()), "SDL_RenderPresent");

            if (!vsyncOn)
            {
                SDL_Delay(kFallbackFrameDelayMs);
            }
        }
    }
}

// SDL's entry point requires this exact signature, so char* is unavoidable here.
int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        run();
        return 0;
    }
    catch (const std::exception& error)
    {
        // Release builds have no console, so show a message box as well as logging.
        // SDL_ShowSimpleMessageBox works even after SDL_Quit has run.
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", error.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lamplighter", error.what(), nullptr);
        return 1;
    }
}
