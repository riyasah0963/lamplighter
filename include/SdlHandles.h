#pragma once

// RAII ownership for SDL objects.
// SDL is a C library: every Create call must be matched by a Destroy call. Wrapping the
// pointers in std::unique_ptr with a custom deleter makes that automatic, including when
// an exception is thrown part-way through start-up.

#include <SDL3/SDL.h>

#include <memory>
#include <stdexcept>
#include <string>

// std::unique_ptr calls these instead of `delete` when the pointer goes out of scope.
struct SdlWindowDeleter
{
    void operator()(SDL_Window* window) const noexcept { SDL_DestroyWindow(window); }
};

struct SdlRendererDeleter
{
    void operator()(SDL_Renderer* renderer) const noexcept { SDL_DestroyRenderer(renderer); }
};

struct SdlTextureDeleter
{
    void operator()(SDL_Texture* texture) const noexcept { SDL_DestroyTexture(texture); }
};

struct SdlSurfaceDeleter
{
    void operator()(SDL_Surface* surface) const noexcept { SDL_DestroySurface(surface); }
};

using WindowPtr = std::unique_ptr<SDL_Window, SdlWindowDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, SdlRendererDeleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, SdlTextureDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SdlSurfaceDeleter>;

// Most SDL3 functions return false on failure; this turns that into an exception carrying SDL's reason.
inline void checkSdl(bool succeeded, const std::string& call)
{
    if (!succeeded)
    {
        throw std::runtime_error(call + " failed: " + SDL_GetError());
    }
}

// Initialises SDL when constructed and shuts it down when destroyed.
// Create it before any other SDL object so it is destroyed after all of them.
class SdlContext
{
public:
    explicit SdlContext(SDL_InitFlags flags)
    {
        if (!SDL_Init(flags))
        {
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        }
    }

    ~SdlContext() { SDL_Quit(); }

    // Copying would call SDL_Quit twice, so it is not allowed.
    SdlContext(const SdlContext&) = delete;
    SdlContext& operator=(const SdlContext&) = delete;
};
