#include "LevelLoader.h"

#include <SDL3/SDL.h>

#include <fstream>
#include <string>
#include <utility>
#include <vector>
namespace
{
    constexpr int kMinSize = 3;    // smallest map: a single floor tile inside a wall border
    constexpr int kMaxSize = 200;  // largest map in either direction

    // Builds a message such as "levels/district1.txt, line 4: row is 38 tiles wide, expected 40".
    LevelLoadError errorAt(const std::string& path, int lineNumber, const std::string& problem)
    {
        return LevelLoadError(path + ", line " + std::to_string(lineNumber) + ": " + problem);
    }
}
Level LevelLoader::load(const std::string& path)
{
    // 1. Open the file.
    std::ifstream file(path);
    if (!file)
    {
        throw LevelLoadError("Cannot open level file: " + path);
    }
    // 2. Read every line into a list of rows.
    std::vector<std::string> rows;
    std::string line;
    while (std::getline(file, line))
    {
        // Files saved on Windows end each line with \r\n; getline keeps the \r, so remove it.
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        rows.push_back(line);
    }

    // Ignore blank lines at the end of the file (editors often add one).
    while (!rows.empty() && rows.back().empty())
    {
        rows.pop_back();
    }
    // 3. Check the size and that every row is the same width.
    if (rows.empty())
    {
        throw LevelLoadError("Level file is empty: " + path);
    }

    const int width = static_cast<int>(rows.front().size());
    const int height = static_cast<int>(rows.size());

    if (width < kMinSize || height < kMinSize || width > kMaxSize || height > kMaxSize)
    {
        throw LevelLoadError(path + ": map is " + std::to_string(width) + "x" + std::to_string(height)
                             + " tiles; it must be between 3x3 and 200x200");
    }

    for (int y = 0; y < height; ++y)
    {
        const int rowWidth = static_cast<int>(rows[static_cast<std::size_t>(y)].size());
        if (rowWidth != width)
        {
            throw errorAt(path, y + 1, "row is " + std::to_string(rowWidth) + " tiles wide, expected "
                                       + std::to_string(width));
        }
    }
    // 4. Turn each character into a tile and record where everything starts.
    std::vector<Tile> tiles;
    tiles.reserve(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
    LevelObjects objects;
    int playerCount = 0;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const char c = rows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
            const TilePos pos{ x, y };
            Tile tile = Tile::Floor;  // everything except walls stands on floor

            switch (c)
            {
            case '#': tile = Tile::Wall; break;
            case '.': break;
            case 'P': objects.playerStart = pos; ++playerCount; break;
            case 'L': objects.lamps.push_back(pos); break;
            case 'O': objects.oilCans.push_back(pos); break;
            case 'C': objects.creeperSpawns.push_back(pos); break;
            case 'M': objects.mothSpawns.push_back(pos); break;
            default:
                // Project rule: unknown characters are logged and treated as floor, never a crash.
                SDL_Log("%s, line %d, column %d: unknown character '%c' treated as floor",
                        path.c_str(), y + 1, x + 1, c);
                break;
            }
            tiles.push_back(tile);
        }
    }
    // 5. Check the rules every district must follow.
    if (playerCount != 1)
    {
        throw LevelLoadError(path + ": needs exactly one player start 'P', found " + std::to_string(playerCount));
    }
    if (objects.lamps.empty())
    {
        throw LevelLoadError(path + ": needs at least one lamp 'L'");
    }

    return Level(width, height, std::move(tiles), std::move(objects));
}
