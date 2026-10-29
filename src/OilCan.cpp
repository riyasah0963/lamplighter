#include "OilCan.h"

#include "DrawHelpers.h"
#include "GameConstants.h"

namespace
{
    constexpr float kCanSize = 12.0f;
}

OilCan::OilCan(TilePos tile)
    : Entity(static_cast<float>(tile.x) * kTileSize + (kTileSize - kCanSize) / 2.0f,  // centred in its tile
             static_cast<float>(tile.y) * kTileSize + (kTileSize - kCanSize) / 2.0f,
             kCanSize, kCanSize)
{
}

void OilCan::render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const
{
    if (collected_)
    {
        return;
    }
    const SDL_FRect screenRect{ mapOrigin.x + x_, mapOrigin.y + y_, width_, height_ };
    fillRect(renderer, screenRect, 200, 120, 40);
}
