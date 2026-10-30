#include "Creeper.h"

#include <memory>

#include "DrawHelpers.h"
#include "GameConstants.h"

namespace
{
    constexpr float kCreeperSize = 20.0f;
}

Creeper::Creeper(const Level& level, const Balance& balance, TilePos spawn)
    : Enemy(level, balance.creeperSpeedTilesPerSecond * kTileSize,
            static_cast<float>(spawn.x) * kTileSize + (kTileSize - kCreeperSize) / 2.0f,  // centred in its tile
            static_cast<float>(spawn.y) * kTileSize + (kTileSize - kCreeperSize) / 2.0f,
            kCreeperSize, kCreeperSize, std::make_unique<PatrolState>())
{
}

void Creeper::render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const
{
    const SDL_FRect screenRect{ mapOrigin.x + x_, mapOrigin.y + y_, width_, height_ };
    fillRect(renderer, screenRect, 150, 120, 220);  // placeholder until the Creeper sprite
}
