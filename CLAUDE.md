# Lamplighter: working notes for Claude

This is university coursework (COMP3016 CW1, due 6 Nov 2026). It is marked on the development process as much as the game, so follow these rules even when a shortcut looks easier. The approved design is in `docs/proposal/PROPOSAL.md`.

## Hard rules from the brief

- **Libraries:** C++ with SDL3, SDL3_image (approval pending) and irrKlang only. Never add another library (no JSON parser, SDL3_ttf, ImGui, fmt). If something seems to need one, stop and ask me.
- **Build:** it must compile and run in Visual Studio 2022 from `Lamplighter.sln` (x64, Debug and Release). Do not replace the solution with CMake or anything else.
- **Modern C++:** `std::string` not `char*`, standard containers, `std::unique_ptr` for ownership, RAII wrappers for every SDL object. No `malloc`, `free`, `printf` or `strcpy`.
- **No crashes on bad input:** missing or malformed level, config and asset files must be caught and shown as an error screen. Unknown level characters are logged and treated as floor.
- **Follow the proposal:** keep the classes and patterns in the UML sketch. If a change would drift from the proposal, tell me first; agreed deviations go in the README table "Proposal and deviations".

## How we work

- Small steps: one feature or fix per change. I review every diff before accepting it, and I must be able to explain all of it, so comment anything non-obvious.
- Commit after each working step with an imperative message that says what changed: "Implement player movement", "Add level loader", "Fix lamp collision at map edge". Never "update", "work" or "fix". Never squash, rewrite history or force-push; the commit history is assessed.
- When a playtest finding drives a change, reference its GitHub Issue in the commit (`Fixes #12`).
- After any change you helped write, add a row to `docs/AI_LOG.md` with the date, my prompt, what you produced and how it was checked. Leave the "What I changed" column for me.
- Never commit build output, executables or zips.

## Building

Open VS Code from **Developer PowerShell for VS 2022** so MSBuild is on the path, then:

```
msbuild Lamplighter.sln /p:Configuration=Debug /p:Platform=x64
```

## Layout

```
src/        .cpp files
include/    .h files
assets/     sprites, font sheet, sounds
levels/     district text files
config/     balance values (oil drain, enemy speeds, clock length)
external/   SDL3, SDL3_image and irrKlang headers, libs and DLLs (committed so markers can build)
docs/       proposal, AI log, playtesting evidence
```

## Level file format

One character per tile: `#` wall, `.` floor, `P` player start, `L` unlit lamp, `O` oil can, `C` Creeper spawn, `M` Moth spawn.
