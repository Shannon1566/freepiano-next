# Repository Guidelines

## Agent 权限与安全约束

- 允许修改当前工程目录下的代码与文件；对项目外的文件只读。

## Project Structure & Module Organization

This repository currently contains project documentation and the initial implementation plan. Keep planning notes under `doc/`; `doc/freepiano-next-plan.md` is the source of truth for v1 architecture. `README.md` is reserved for user-facing overview content.

The planned application is a Windows x64 virtual piano built with Qt 6/QML and C++20. When source code is added, follow the planned module boundaries: `ui` for QML views, `core` for `PianoController` and QML-facing APIs, `input` for keyboard and mouse mapping, `midi` for event routing, `audio` for WASAPI output, and `vst3` for VST3 hosting.

## Build, Test, and Development Commands

never run cmake directly from a normal shell.

运行`build.bat`构建项目

## Coding Style & Naming Conventions

Use C++20 for backend code and QML for the UI. Prefer Qt idioms: `QObject`, `Q_PROPERTY`, signals, slots, and `Q_INVOKABLE` for QML-facing APIs. Keep QML calls high-level, such as `noteOn`, `noteOff`, `setSustain`, and `panic`.

Use 4-space indentation for C++ and QML. Name C++ classes in `PascalCase`, functions and variables in `camelCase`, and interfaces with an `I` prefix where already planned, for example `IMidiInputProvider`.

## Testing Guidelines

The current test plan is build validation plus manual testing for mouse input, keyboard input, audio output, and VST3 loading. When automated tests are introduced, place them under `tests/` and name them after the behavior under test, for example `test_midi_event_router.cpp`.

Keep audio-thread behavior testable without QML. Tests should cover duplicate keydown handling, all-notes-off behavior, sustain state, and MIDI event ordering.

## Commit & Pull Request Guidelines

Recent history uses short messages, including the `AI(scope): message` pattern, for example `AI(plan): 修改首次Plan内容`. Keep commits concise and scoped to one logical change.

Pull requests should include a summary, changed modules or documents, build/test commands run, and manual validation performed. For UI changes, include screenshots or a short recording. Link related issues or planning sections when applicable.

## Agent-Specific Instructions

Do not invent source layout or commands that are not present without updating documentation. Preserve the v1 constraints in `doc/freepiano-next-plan.md`: Windows x64, Qt 6/QML, C++20, WASAPI shared mode, and the default `mda Piano` VST3 target.
