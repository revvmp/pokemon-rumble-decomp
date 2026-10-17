# F-756-2: Complete HBMBase first-block acceptance audit

Issue #756 lists 22 functions, 20,124 instruction bytes, from `0x8017C470` through `0x80181374`. These bodies were reconstructed in F-756-1, but their complete native source unit remained `NonMatching` while later blocks were unfinished. PR #900 completed #760 and linked the whole unit from source after #757, #758, and #759 supplied its intervening bodies.

A fresh isolated worktree was configured and built with the pinned tools. The audit compared every issue address and size with the real objdiff report, required every listed function to compare at 100%, checked all 37 functions and all four sections of the complete native unit, and confirmed the object is configured `Matching`.

| Address | Size | Current symbol | Objdiff |
| --- | --- | --- | --- |
| `0x8017C470` | `0x1B8` | `initgx__10homebuttonFv` | 100% |
| `0x8017C630` | `0x110` | `__ct__Q210homebutton10HomeButtonFPC11HBMDataInfo` | 100% |
| `0x8017C740` | `0x2D8` | `__dt__Q210homebutton10HomeButtonFv` | 100% |
| `0x8017CA20` | `0x6C` | `__dt__Q36nw4hbm3lyt19ArcResourceAccessorFv` | 100% |
| `0x8017CA90` | `0x4C` | `createInstance__Q210homebutton10HomeButtonFPC11HBMDataInfo` | 100% |
| `0x8017CAE0` | `0x44` | `deleteInstance__Q210homebutton10HomeButtonFv` | 100% |
| `0x8017CB30` | `0xC` | `getInstance__Q210homebutton10HomeButtonFv` | 100% |
| `0x8017CB40` | `0x8B0` | `create__Q210homebutton10HomeButtonFv` | 100% |
| `0x8017D3F0` | `0x170` | `set_config__Q210homebutton10HomeButtonFv` | 100% |
| `0x8017D560` | `0x84` | `set_text__Q210homebutton10HomeButtonFv` | 100% |
| `0x8017D5F0` | `0x540` | `init__Q210homebutton10HomeButtonFv` | 100% |
| `0x8017DB30` | `0x200` | `init_volume__Q210homebutton10HomeButtonFv` | 100% |
| `0x8017DD30` | `0x130` | `init_sound__Q210homebutton10HomeButtonFv` | 100% |
| `0x8017DE60` | `0x26C` | `init_battery__Q210homebutton10HomeButtonFPC17HBMControllerData` | 100% |
| `0x8017E0D0` | `0x23A0` | `calc__Q210homebutton10HomeButtonFPC17HBMControllerData` | 100% |
| `0x80180470` | `0x204` | `calc_battery__Q210homebutton10HomeButtonFi` | 100% |
| `0x80180680` | `0x170` | `draw__Q210homebutton10HomeButtonFv` | 100% |
| `0x801807F0` | `0xE4` | `SpeakerCallback__10homebuttonFP7OSAlarmP9OSContext` | 100% |
| `0x801808E0` | `0x24` | `MotorCallback__10homebuttonFP7OSAlarmP9OSContext` | 100% |
| `0x80180910` | `0xCC` | `RetrySimpleSyncCallback__10homebuttonFP7OSAlarmP9OSContext` | 100% |
| `0x801809E0` | `0x38` | `SimpleSyncCallback__10homebuttonFll` | 100% |
| `0x80180A20` | `0x954` | `update__Q210homebutton10HomeButtonFPC17HBMControllerData` | 100% |

The complete source unit remains `.text` `0x8017C470` to `0x80186334`, `.rodata` `0x80366AA0` to `0x80366E38`, `.data` `0x803BA6A0` to `0x803BB4E0`, and `.bss` `0x803E9BA0` to `0x803E9BC0`. All 37 functions, all 40,428 instruction bytes, and every section compare at 100%. The source-linked executable passes the frozen-manifest hash checks and direct byte comparison.

Validation commands, all exit 0: `python configure.py`; `.venv/Scripts/ninja`; `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`. The independent audit additionally checks the exact 22-function, 20,124-byte issue inventory against `build/wpse-v1/report.json`.

No source bodies, compiler flags, split boundaries, original hashes, or source-coverage credit change in this audit. Source provenance and target differences remain in F-756-1; full-unit completion evidence is in F-760-2. This acceptance supersedes the earlier pending status of F-756-1.

## Traps

No new trap or reverse engineering finding emerged from this acceptance audit.
