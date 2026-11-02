#pragma once

#include <stdexcept>
#include <string>

#include "Balance.h"

// Thrown when the config file is missing or broken. The message names the file and the line,
// so it can be shown to the player as it is.
class ConfigLoadError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

// Reads balance values from a text file: one "name = value" per line. Blank lines and lines
// starting with # are ignored. A name left out of the file keeps its default from Balance.h.
class ConfigLoader
{
public:
    // Throws ConfigLoadError if the file cannot be read or a line is wrong.
    static Balance load(const std::string& path);
};
