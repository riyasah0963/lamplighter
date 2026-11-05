#pragma once

#include <map>
#include <optional>
#include <string>

// The fastest winning time for each district, saved to a text file so it survives between runs.
// One line per district: "<district> <seconds>", e.g. "2 74.3".
// A missing or damaged save file is not an error: play goes on without those times, and any bad
// line is logged and skipped. Losing a best time is better than refusing to start the game.
class BestTimes
{
public:
    explicit BestTimes(std::string path);  // reads the file if it exists

    std::optional<double> best(int district) const;  // empty if the district has never been won

    // Records a winning time. Returns true, and saves the file, if it beats the old best.
    bool record(int district, double seconds);

private:
    void save() const;

    std::string path_;
    std::map<int, double> seconds_;  // district number -> best time
};
