#pragma once

// Gameplay balance values in one place. The defaults are set here; ConfigLoader will read
// config/balance.txt over them, so values can be tuned after playtesting without recompiling.
// Oil amounts are percentages of a full tank (0 to 100).
struct Balance
{
    float playerSpeedTilesPerSecond = 4.0f;

    float oilFullTankSeconds = 45.0f;      // how long a full tank lasts with the lantern open
    float oilCanRefill = 50.0f;            // oil gained from picking up a can
    float lampRefillPerSecond = 30.0f;     // oil gained per second while standing in a lit lamp's light

    float lanternMaxRadiusTiles = 5.5f;    // lantern light radius with a full tank
    float lanternMinRadiusTiles = 2.0f;    // lantern light radius just before the oil runs out
    float lampRadiusTiles = 4.5f;          // light radius of a lit street lamp
    float lampReachTiles = 1.5f;           // how close the player must stand to light a lamp with E

    int moonlight = 45;                    // brightness of the unlit dark, 0 (pitch black) to 255

    // A light's glow fades to nothing at its edge, so the part that counts as "in the light"
    // (repels enemies, refills oil) is this fraction of its radius: the bright part you can see.
    float lightCoreFraction = 0.6f;

    float creeperSpeedTilesPerSecond = 3.0f;
    float creeperSightTiles = 7.0f;            // how far a Creeper notices the player with the lantern open
    float creeperSightTilesLanternShut = 3.0f; // a shut lantern hides the player, so it must get much closer
};
