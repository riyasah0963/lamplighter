#pragma once

#include <string>

// Turns a path such as "levels/district1.txt" into a full path inside the folder that holds the exe.
// The game then finds its files however it is started: from Visual Studio, by double-clicking the
// exe, or from the unzipped submission.
std::string gamePath(const std::string& relativePath);
