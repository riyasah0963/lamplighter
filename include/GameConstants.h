#pragma once

// Fixed values the engine is built around.
// Balance values that playtesting will tune (oil drain, enemy speeds, clock length) belong in
// config/ instead, so they can be changed without recompiling.

// The game always draws into this 16:9 space; SDL scales it to fit the window.
inline constexpr int kLogicalWidth = 1280;
inline constexpr int kLogicalHeight = 720;

// How many fixed-step updates the game logic runs per second.
inline constexpr int kUpdatesPerSecond = 60;

// Size of one map tile on screen, in pixels. A 40x22 district fills the 1280x720 game area.
inline constexpr float kTileSize = 32.0f;
