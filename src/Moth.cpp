#include "Moth.h"

#include <memory>

#include "DrawHelpers.h"
#include "GameConstants.h"

namespace
{
    constexpr float kMothSize = 14.0f;
}

Moth::Moth(const Level& level, const Balance& balance, TilePos spawn)
    : Enemy(level, balance.mothSpeedTilesPerSecond * kTileSize,
            balance.mothSightTiles * kTileSize, 0.0f,  // sees an open lantern from far off; a shut one not at all
            static_cast<float>(spawn.x) * kTileSize + (kTileSize - kMothSize) / 2.0f,  // centred in its tile
            static_cast<float>(spawn.y) * kTileSize + (kTileSize - kMothSize) / 2.0f,
            kMothSize, kMothSize, std::make_unique<PatrolState>())
{
}

void Moth::render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const
{
    const SDL_FRect screenRect{ mapOrigin.x + x_, mapOrigin.y + y_, width_, height_ };
    fillRect(renderer, screenRect, 225, 205, 255);  // pale wings, placeholder until the Moth sprite
}
