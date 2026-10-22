#include "GamePaths.h"

#include <SDL3/SDL.h>

std::string gamePath(const std::string& relativePath)
{
    // SDL_GetBasePath returns the exe's folder ending in a slash. SDL owns the string; it must not be freed.
    const char* basePath = SDL_GetBasePath();
    if (basePath == nullptr)
    {
        SDL_Log("SDL_GetBasePath failed (%s); looking in the working directory instead", SDL_GetError());
        return relativePath;
    }
    return std::string(basePath) + relativePath;
}
