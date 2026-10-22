#pragma once

#include <vector>

// What a single map tile is made of. Things that can move or be picked up (player, lamps,
// oil cans, enemies) are kept as positions in LevelObjects; the tile under them is floor.
enum class Tile
{
    Floor,
    Wall,
};
// A position on the map in tiles: column x and row y, counted from the top-left corner.
struct TilePos
{
    int x = 0;
    int y = 0;
};
// Where everything starts in a district, as read from the level file.
struct LevelObjects
{
    TilePos playerStart;
    std::vector<TilePos> lamps;
    std::vector<TilePos> oilCans;
    std::vector<TilePos> creeperSpawns;
    std::vector<TilePos> mothSpawns;
};
// One district: a grid of tiles plus the starting positions of everything on it.
class Level
{
public:
    Level(int width, int height, std::vector<Tile> tiles, LevelObjects objects);

    int width() const { return width_; }
    int height() const { return height_; }

    Tile tileAt(int x, int y) const;
    bool isWall(int x, int y) const { return tileAt(x, y) == Tile::Wall; }

    const LevelObjects& objects() const { return objects_; }
private:
    int width_;
    int height_;
    std::vector<Tile> tiles_;  // stored row by row: the tile at (x, y) is tiles_[y * width_ + x]
    LevelObjects objects_;
};
