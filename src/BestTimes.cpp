#include "BestTimes.h"

#include <SDL3/SDL.h>

#include <fstream>
#include <sstream>
#include <utility>

#include "GameConstants.h"

BestTimes::BestTimes(std::string path)
    : path_(std::move(path))
{
    std::ifstream file(path_);
    if (!file)
    {
        return;  // no save file yet: nothing has been won
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line))
    {
        ++lineNumber;
        std::istringstream fields(line);
        int district = 0;
        double seconds = 0.0;
        std::string extra;

        // A good line is exactly two numbers: a district that exists and a time above 0.
        const bool ok = (fields >> district >> seconds) && !(fields >> extra)
                        && district >= 1 && district <= kDistrictCount && seconds > 0.0;
        if (ok)
        {
            seconds_[district] = seconds;
        }
        else if (!line.empty() && line != "\r")
        {
            SDL_Log("%s, line %d: not a best time, skipped", path_.c_str(), lineNumber);
        }
    }
}

std::optional<double> BestTimes::best(int district) const
{
    const auto found = seconds_.find(district);
    if (found == seconds_.end())
    {
        return std::nullopt;
    }
    return found->second;
}

bool BestTimes::record(int district, double seconds)
{
    const std::optional<double> old = best(district);
    if (old && *old <= seconds)
    {
        return false;
    }
    seconds_[district] = seconds;
    save();
    return true;
}

void BestTimes::save() const
{
    std::ofstream file(path_);
    if (!file)
    {
        // For example a read-only folder. The time still shows this session, so only log it.
        SDL_Log("Could not save best times to %s", path_.c_str());
        return;
    }
    for (const auto& [district, seconds] : seconds_)
    {
        file << district << ' ' << seconds << '\n';
    }
}
