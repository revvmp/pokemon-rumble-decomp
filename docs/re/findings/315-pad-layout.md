# F-315-4: pad unit ownership and rumble layout probes

## Scope

The pad unit begins at `0x801B15A0`, four callbacks before issue #316's survey boundary. Issue #315 owns that first function and consequently the complete source file and class declarations. The source continues through the initializer at `0x801B52E0-0x801B5308`, including the first fourteen functions surveyed under #317. The next reset unit begins at `0x801B5310`; it is separate and remains under #317 ownership. No pad source, split, header, or matching credit is published by this investigation.

The complete pad inventory comprises 48 surveyed functions: four from #315, thirty from #316, and fourteen from #317. Native code sizes total 15,452 bytes. The original RTTI spells the anonymous namespace as `@unnamed@pad_cpp@`, with `BRumble`, `CWiiRemoteRumble`, `CGCPadRumble`, and `PadData`. The available pinned `pad.cpp` is empty and supplies no implementation.

## Rumble layout

Constructor stores, alarm calls, array destructor strides, and virtual calls establish an 88-byte rumble object. Its enabled flag occupies offset 0, block count 4, controller channel 8, state 12, pending value 16, cooldown float 20, canonical 48-byte `OSAlarm` 24, start time 72, and virtual pointer 80. The two derived classes add no storage. Member names describe behavior; the class names are original RTTI identities. The cooldown initializer is the native float `0.2f` at `0x804BA3B0`.

The ordinary C++ probe declares these data members before its first virtual method. Under the game's actual Wii/1.1 flags, CodeWarrior naturally places the virtual pointer at offset 80 and produces the 88-byte array stride. There is no invented storage base, explicit virtual-pointer member, or manual dispatch. Both derived constructors reproduce all 140 instruction bytes and their actual relocation targets, including the base and derived vtables, float constant, memset, OSCreateAlarm, and OSSetAlarmUserData.

| Class | RTTI | Vtable | Constructor | Destructor |
| --- | --- | --- | --- | --- |
| BRumble | 804B8210 | 803BF500 | Inlined into derived constructors | 801B2CC0 |
| CWiiRemoteRumble | 804B8208 | 803BF4B4 | 801B2C30 | 801B2D00 |
| CGCPadRumble | 804B8200 | 803BF46C | 801B2D40 | 801B2DD0 |

The vtable method order is destructor, capability predicate, start, stop, and hard stop. The three GameCube motor methods tail-call `PADControlMotor` with modes 1, 0, and 2. The Wii methods tail-call `WPADControlMotor` with modes 1, 0, and 0. The base capability predicate returns enabled and block-count-zero; the Wii override additionally requires `WPADIsMotorEnabled`.

The canonical-header-only diagnostic proves twelve functions and 584 instruction bytes with paired targets: both constructors, all three destructors, six motor methods, and the base capability predicate. The Wii predicate still differs in three instruction scheduling positions despite equivalent behavior, so it is explicitly excluded. No exception, initialized-data, whole-unit, or source-linked acceptance is claimed for this probe.

## Memory callback diagnostic

The unresolved memory callback from F-315-3 contains three native unexpected-handler scopes. A direct call to the existing general restricted allocator produces two scopes. A second ordinary fixed-alignment restricted delegate between them reproduces the complete callback's 144 instruction bytes, all targets, and exception records. In that diagnostic all twelve memory bodies and all 604 exception bytes match.

This does not establish the original delegate's name, public overload, or reusable responsibility. Independent callers at `0x8019E320` and `0x801C49C0` pass alignment 32 directly to the general allocator; thread construction at `0x801A22B0` passes negative 32. These callers prove alignment use, but have not independently established the additional delegate. Consequently the delegate remains outside the repository and the actual memory source remains NonMatching. No extra public API is published merely to obtain the third unwind scope.

## Reproduction

Outside the checkout, `C:/MassStorage/GitHub/315-rumble-probe.cpp`, `315-compile-rumble.py`, and `315-audit-rumble.py` reproduce the canonical-header-only rumble probe. `315-rumble-native-proof.json` records every audited body, actual relocation target, and the excluded Wii predicate differences. Constructors are naturally retained as callbacks for four-element C++ arrays, matching the native destructor stride evidence.

`315-pad-native.json` records the isolated IDA reading aid, not publishable source. `315-api-callers.json` records the independent caller inspection. `315-callback-variants.py` rejects twelve ordinary early-return, assignment, and conditional forms using the two per-heap variants; `315-callback-variants.json` records their sizes and differing instruction offsets. `315-callback-nested.cpp` and `315-callback-nested-proof.json` record the exact but structurally unresolved extra-delegate diagnostic.

## Traps

- A virtual pointer after nonvirtual fields does not prove an additional base class. This compiler naturally places it after the data members when the first virtual declaration follows them; confirm with ordinary typed construction before inventing inheritance.
- Matching a nested exception scope does not recover a public overload. Preserve the exact diagnostic while independently establishing the source responsibility; keep the original object selected until the reconstruction is justified.
- The isolated IDA database lacks function starts at `0x801B4640`, `0x801B4840`, and `0x801B4BF0` despite the survey inventory. Use native bytes, exception indices, and entry references to investigate those boundaries; an absent IDA function is not evidence that the original function is absent.
