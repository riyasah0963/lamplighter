#pragma once

#include <stdexcept>
#include <string>

#include "Level.h"
// Thrown when a level file is missing or broken. The message names the file and, where it
// helps, the line, so it can be shown to the player as it is.
class LevelLoadError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};
// Reads a district from a text file: one character per tile (# wall, . floor, P player,
// L lamp, O oil can, C Creeper, M Moth).
class LevelLoader
{
public:
    // Throws LevelLoadError if the file cannot be read or breaks one of the rules.
    static Level load(const std::string& path);
};
