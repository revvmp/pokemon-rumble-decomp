# Sprint a13

## Traps

- **Unit scores near 0% with fully written source can be a symbol-name mismatch.** The #863 rename gave every script binding the placeholder `ScriptBinding_<name>`, but reconstructed sources define mangled names (`<name>__FPQ24NPpu7CEngine`). Unitdiff then shows "missing" for every function. ScriptGame went from 1/170 to 140/170 at 100%, and ScriptEffect, ScriptLayout and ScriptCameraMotion gained 22, 12 and 25 functions, by renaming the `ScriptBinding_` symbols in the unit's range to the mangled names its object produces (match by `x__F` prefix, unique). Check this before assuming code is missing in any script unit.
- **Out-of-line helper placeholders pair by size.** A `fn_` that unitdiff lists as missing is often a static helper the source already emits under a mangled name (`GetWazaEffectParam`, `LayoutMotion`, `PopCameraMotion`). Rename it by address once the unique same-size function in the rebuilt object is identified.
- **Do not rename template instantiations that another object already defines.** Naming CLayout `shared_ptr_deleter`/`move_ptr` functions in ScriptLayout, or `shared_ptr<CSimpleGround>::~shared_ptr` in ScriptGame, gave multiply-defined linker errors. Keep their placeholders.
- **name_callees applied outside the unit can fail the link.** Its 16-17 renames of addresses such as 0x8014FB20 and 0x80155D00 broke `ninja`; they were left out.

## CPpuEnv

`0x801035D0`-`0x8010623C` is `game/CPpuEnv.cpp`: `CPpuEnv` (derived from `NPpu::CEnvironment` and `enable_shared_from_this`, a `std::map<std::string, shared_ptr<CCode>>` script cache at `0x2C`, a loading flag at `0x28`), `CPpuCode` (a `CCode` with a file reference and name), `CreateScriptEnvironment`, `LoadScriptCode` and the `CEnvironment::CollectGarbage` cache clear. The unit also owns the 670-entry native binding table in `.rodata` and the binding-name strings in `.data`; those are not reconstructed. `LoadScriptCode` is at 89.8%, `CreateScriptEnvironment` and the map find/insert/deleter instantiations are 100%. The tree `destroy` instantiation is 84.8% and `CollectGarbage` 0% because the rebuilt `destroy` inlines the pair destructor at one extra site (inline budget); not resolved.
