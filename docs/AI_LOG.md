# AI Development Log

The brief asks for: tools used, categories of use, example prompts, how each output was verified, and repository evidence that the work went beyond what the AI produced.
Add an entry every time AI output ends up in the project. Keep the prompts real and the "what I changed" column honest; that column is where the marks are.

## Tools used

| Tool | Version or model | Free tier, paid or open-source | Used for |
| --- | --- | --- | --- |
| Claude (claude.ai) | Claude Opus 5.5 | | Planning, proposal layout |
| Claude in VS Code (Claude Code) | Claude Opus 5.5 | Paid plan | Code assistant |
| Stable Diffusion | | Open-source | Sprite drafts |

## Log

| Date | Tool | Category | Prompt (representative) | What the AI produced | How I checked it | What I changed | Commits |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 19 Oct 2026 | Claude (claude.ai) | Idea generation, planning, documentation | "What is the best game idea we should proceed with?" then "Give it the best" (asked for a 1-page MDA proposal) | Three game concepts and a recommendation (Lamplighter); a one-page MDA proposal with a gameplay sketch, core loop and UML sketch | Checked every section against the CW1 brief's proposal list and the DLE task (1 page, MDA, sketches); checked the scope against the 18 days to 6 Nov | Reworded some parts: my name and student ref number, the MDA framework, the technical features and the development roadmap. | [44a5151](https://github.com/riyasah0963/lamplighter/commit/44a5151) |
| 19 Oct 2026 | Claude in VS Code (Claude Code) | Code generation, build configuration | "Next roadmap milestone: set up the Visual Studio 2022 project. Show me your plan before writing any code." (x64 Debug/Release, C++20, SDL3 + SDL3_image from the official releases, relative paths, post-build copy, smallest window and loop with unique_ptr deleters and every SDL call checked) | A plan, then `Lamplighter.sln`/`.vcxproj`/`.filters` (C++20, /W4, Console in Debug and Windows in Release, /MT in Release, post-build copy of DLLs and the assets/levels/config folders); `include/SdlHandles.h` (unique_ptr deleters, `SdlContext` RAII guard for SDL_Init/SDL_Quit); `src/main.cpp` (1280x720 window, loop, Escape or close button quits, errors shown in a message box); SDL3 3.4.18 and SDL3_image 3.4.8 in `external/`; a `.gitignore` fix (the `x64/` rule would have excluded `external/*/lib/x64`) | I approved the plan before any code was written. Claude checked both SDL zips' SHA-256 against GitHub's published digests and every SDL call against the 3.4.18 headers. An AI review pass (3 reviewers, each finding checked by a second agent) led to 2 fixes (VS skipping the data copy after a data-only edit; Release .pdb landing in the zip folder) and 6 rejected findings. `msbuild` Rebuild of Debug and Release: 0 warnings, 0 errors. `dumpbin` shows the Release exe needs only SDL3.dll, SDL3_image.dll and KERNEL32.dll (no VC++ runtime). Both builds opened the window and exited with code 0 on close. I ran it myself in VS 2022 (F5, Debug x64): the window opened, and both Escape and the close button closed it with exit code 0. | Reviewed the plan and chose /MT for Release and dropping the PDBs. Tested the build. No code changes yet. | [407b6c2](https://github.com/riyasah0963/lamplighter/commit/407b6c2), [95b81a5](https://github.com/riyasah0963/lamplighter/commit/95b81a5), [c141d39](https://github.com/riyasah0963/lamplighter/commit/c141d39) |

## Verification notes

Longer notes for entries that need them: bugs found in generated code, why a suggestion was rejected, before and after screenshots.
