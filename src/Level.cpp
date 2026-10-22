#include "Level.h"

#include <utility>

Level::Level(int width, int height, std::vector<Tile> tiles, LevelObjects objects)
    : width_(width)
    , height_(height)
    , tiles_(std::move(tiles))
    , objects_(std::move(objects))
{
}
Tile Level::tileAt(int x, int y) const
{
    // Anything outside the map counts as wall, so nothing can ever walk or look off the edge.
    if (x < 0 || y < 0 || x >= width_ || y >= height_)
    {
        return Tile::Wall;
    }
    return tiles_[static_cast<std::size_t>(y * width_ + x)];
}
