#include "ConfigLoader.h"

#include <cmath>
#include <exception>
#include <fstream>
#include <map>
#include <string>

namespace
{
    // Builds a message such as "config/balance.txt, line 4: 'fast' is not a number".
    ConfigLoadError errorAt(const std::string& path, int lineNumber, const std::string& problem)
    {
        return ConfigLoadError(path + ", line " + std::to_string(lineNumber) + ": " + problem);
    }

    // Removes spaces, tabs and the \r from Windows line endings from both ends of the text.
    std::string trim(const std::string& text)
    {
        const std::size_t first = text.find_first_not_of(" \t\r");
        if (first == std::string::npos)
        {
            return "";
        }
        const std::size_t last = text.find_last_not_of(" \t\r");
        return text.substr(first, last - first + 1);
    }

    // Reads the whole text as a number. Returns false for text such as "fast" or "12abc".
    bool parseNumber(const std::string& text, float& number)
    {
        try
        {
            std::size_t charsUsed = 0;
            number = std::stof(text, &charsUsed);
            return charsUsed == text.size() && std::isfinite(number);
        }
        catch (const std::exception&)
        {
            return false;  // std::stof throws when the text is not a number or is far too big
        }
    }

  
    // Puts a value into the Balance field with this name. Returns false if there is no such field.
    bool applyValue(Balance& balance, const std::string& name, float value)
    {
        // Each name points at the field it sets, so adding a value only needs one new line here.
        const std::map<std::string, float*> floatFields{
            { "playerSpeedTilesPerSecond", &balance.playerSpeedTilesPerSecond },
            { "oilFullTankSeconds", &balance.oilFullTankSeconds },
            { "oilCanRefill", &balance.oilCanRefill },
            { "lampRefillPerSecond", &balance.lampRefillPerSecond },
            { "lanternMaxRadiusTiles", &balance.lanternMaxRadiusTiles },
            { "lanternMinRadiusTiles", &balance.lanternMinRadiusTiles },
            { "lampRadiusTiles", &balance.lampRadiusTiles },
            { "lampReachTiles", &balance.lampReachTiles },
            { "lightCoreFraction", &balance.lightCoreFraction },
            { "creeperSpeedTilesPerSecond", &balance.creeperSpeedTilesPerSecond },
            { "creeperSightTiles", &balance.creeperSightTiles },
            { "creeperSightTilesLanternShut", &balance.creeperSightTilesLanternShut },
            { "clockSeconds", &balance.clockSeconds },
            { "hitSafeSeconds", &balance.hitSafeSeconds },
        };

        const auto field = floatFields.find(name);
        if (field != floatFields.end())
        {
            *field->second = value;
            return true;
        }

        // These two are whole numbers, so they are not in the float list.
        if (name == "moonlight")
        {
            balance.moonlight = static_cast<int>(value);
            return true;
        }
        if (name == "startingHearts")
        {
            balance.startingHearts = static_cast<int>(value);
            return true;
        }
        return false;
    }

    // A zero here could divide by zero or end the game at once, so every value must be above 0.
    bool isAllowed(const std::string& name, float value)
    {
        if (name == "moonlight")
        {
            return value >= 0.0f && value <= 255.0f;  // 0 is pitch black, which is fine
        }
        if (name == "startingHearts")
        {
            return value >= 1.0f;
        }
        return value > 0.0f;
    }
}
  

Balance ConfigLoader::load(const std::string& path)
{
    // 1. Open the file.
    std::ifstream file(path);
    if (!file)
    {
        throw ConfigLoadError("Cannot open config file: " + path);
    }

    // 2. Start from the defaults in Balance.h; each line in the file replaces one of them.
    Balance balance;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line))
    {
        ++lineNumber;
        const std::string text = trim(line);
        if (text.empty() || text.front() == '#')
        {
            continue;  // a blank line or a comment
        }

        // 3. Split the line at the equals sign.
        const std::size_t equals = text.find('=');
        if (equals == std::string::npos)
        {
            throw errorAt(path, lineNumber, "expected name = value");
        }
        const std::string name = trim(text.substr(0, equals));
        const std::string valueText = trim(text.substr(equals + 1));

        // 4. Check the value and store it.
        float value = 0.0f;
        if (!parseNumber(valueText, value))
        {
            throw errorAt(path, lineNumber, "'" + valueText + "' is not a number");
        }
        if (!applyValue(balance, name, value))
        {
            throw errorAt(path, lineNumber, "unknown setting '" + name + "'");
        }
        if (!isAllowed(name, value))
        {
            throw errorAt(path, lineNumber, "'" + valueText + "' is not allowed for " + name);
        }
    }
    return balance;
}
