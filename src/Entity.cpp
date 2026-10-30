#include "Entity.h"

#include <cmath>

#include "GameConstants.h"

namespace
{
    // Which tile column or row a map-pixel coordinate falls in.
    int tileIndex(float pixels)
    {
        return static_cast<int>(std::floor(pixels / kTileSize));
    }
}

bool Entity::moveAlongAxis(const Level& level, float dx, float dy)
{
    x_ += dx;
    y_ += dy;

    // The range of tiles the box now overlaps. The tiny margin stops a box that exactly
    // touches the next tile's edge from counting as being inside it.
    constexpr float kEdgeMargin = 0.001f;
    const int left = tileIndex(x_);
    const int right = tileIndex(x_ + width_ - kEdgeMargin);
    const int top = tileIndex(y_);
    const int bottom = tileIndex(y_ + height_ - kEdgeMargin);

    bool hitWall = false;
    for (int ty = top; ty <= bottom; ++ty)
    {
        for (int tx = left; tx <= right; ++tx)
        {
            if (!level.isWall(tx, ty))
            {
                continue;
            }

            // Push back out of the wall on the side the entity came from.
            if (dx > 0.0f) { x_ = static_cast<float>(tx) * kTileSize - width_; }
            if (dx < 0.0f) { x_ = static_cast<float>(tx + 1) * kTileSize; }
            if (dy > 0.0f) { y_ = static_cast<float>(ty) * kTileSize - height_; }
            if (dy < 0.0f) { y_ = static_cast<float>(ty + 1) * kTileSize; }
            hitWall = true;
        }
    }
    return hitWall;
}
