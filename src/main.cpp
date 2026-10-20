// Lamplighter entry point: create the Game and run it. Any start-up or runtime error that is
// thrown ends up here and is shown to the player instead of crashing.

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>  // Must be included where main() is defined: on Windows it supplies the WinMain that calls our main()

#include <exception>

#include "Game.h"

// SDL's entry point requires this exact signature, so char* is unavoidable here.
int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        Game game;
        game.run();
        return 0;
    }
    catch (const std::exception& error)
    {
        // Release builds have no console, so show a message box as well as logging.
        // SDL_ShowSimpleMessageBox works even after the Game (and SDL) has shut down.
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", error.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lamplighter", error.what(), nullptr);
        return 1;
    }
}
