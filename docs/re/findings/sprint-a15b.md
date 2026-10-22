# Sprint a15b: ScriptGame unit (issues 239-241)

## Traps

- The Game/event script unit (0x80138BA0-0x80140C40) is now split as `game/ScriptGame.cpp` (NonMatching). Its `.sdata` starts at 0x804B79B8, `.sdata2` is 0x804BA208-0x804BA220, `.bss` is the 2 KiB message buffer at 0x803E9100.
- Pre-declaring locals changes MWCC register order and stack slot order. `int frames;` declared before `int step = -1;` fixed StartTimer. In map-record bindings, declaring `int stage; int area; int mode;` then assigning mode, area, stage reproduces the native pre-call field extraction (IsStageCleared 100%).
- A `MapIndex` anonymous union with `mValue` makes IsStageCleared worse (64%) than `*reinterpret_cast<int*>(&index)`, and breaks aggregate initialisation in CPauseMenu. Keep the bitfield-only struct.
- Boolean pushes: `bool shown = false; if (p) shown = p->M(); PushInt(shown);` matches where the `?:` form leaves branch order wrong (IsMessageBalloonShown).
- IGameSession::IsReplacePiiAvailable takes only the player (native caller passes one argument; vtable offset 0x7c).
- Shared scratchpad files are overwritten by other agents; keep experiment scripts in a private subdirectory.

## Unresolved (all within 0.5-4 percent)

MakeMapID (register priority of pEngine vs area), MapStatus_allFlags and MapStatus_setFlags (native keeps the packed index in the lowest saved register), GetStageClearedNum (stack temp order for min/max), IncMapClearedNum (record pointer register), ShowDlgPiiList (two-pop register order), AddMessageBalloon (stack slot order), OpenScriptChoices (register allocation r20 vs r25).
