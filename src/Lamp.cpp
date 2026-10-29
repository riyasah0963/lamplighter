#include "Lamp.h"

#include "DrawHelpers.h"
#include "GameConstants.h"

namespace
{
    constexpr float kLampSize = 16.0f;
}

Lamp::Lamp(TilePos tile)
    : Entity(static_cast<float>(tile.x) * kTileSize + (kTileSize - kLampSize) / 2.0f,  // centred in its tile
             static_cast<float>(tile.y) * kTileSize + (kTileSize - kLampSize) / 2.0f,
             kLampSize, kLampSize)
{
}

void Lamp::render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const
{
    const SDL_FRect screenRect{ mapOrigin.x + x_, mapOrigin.y + y_, width_, height_ };
    if (lit_)
    {
        fillRect(renderer, screenRect, 255, 225, 150);  // bright flame
    }
    else
    {
        fillRect(renderer, screenRect, 120, 104, 60);   // dull brass
    }
}
