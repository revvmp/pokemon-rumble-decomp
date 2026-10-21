# Issue 121: CGameMap.cpp 6/10 (0x800AA4F0 to 0x800AAA34)

Target `wpse-v1`. The block lies inside `game/CGameMap.cpp` (F-119-1). Neighbouring blocks: #120 (preceding), #122 (following, same file).

## F-121-1: the 24 functions

The source already compiled every function of this block as part of #119 and #120 (the inline slot bodies, the `shared_ptr` helpers and `weak_ptr<IDlg>::lock`); the target symbols were still placeholders, so objdiff reported them as missing. Naming them in `symbols.txt` was the whole change.

| Address | Size | Identity | Match |
| --- | --- | --- | --- |
| `0x800AA4F0`-`0x800AA5B0` | `0x8` | `CGameMap::GetPauseMenu`, `GetLayoutManager`, `GetComboDisplay`, `GetBattleRoyalDisplay`, `GetMoneyLifeDisplay`, `GetNoticeDisplay`, `GetDlgMsgWin`, `GetDlgInfo`, `GetDlgCollection`, `GetDlgPiiList`, `GetDlgPiiSelector`, `GetMiniMap`, `GetWorldObjStatDisplay` | 100% |
| `0x800AA5C0` | `0x8` | `CGameMap::GetWorld() const` | 100% |
| `0x800AA5D0` | `0x4C` | `shared_ptr_deleter<CGameMap>::get_deleter` | 99.74% |
| `0x800AA620` | `0x24` | `shared_ptr_deleter<CGameMap>::dispose` | 100% |
| `0x800AA650` | `0x4C` | `shared_ptr_deleter<CDividedGround>::get_deleter` | 99.74% |
| `0x800AA6A0` | `0x24` | `shared_ptr_deleter<CDividedGround>::dispose` | 100% |
| `0x800AA6D0` | `0x60` | `weak_ptr<IDlg>::~weak_ptr` | 100% |
| `0x800AA730` | `0xCC` | `weak_ptr<IDlg>::lock` | 100% (named in #120) |
| `0x800AA800` | `0x84` | `shared_ptr<CGameMap>::~shared_ptr` | 100% |
| `0x800AA890`, `0x800AA9B0` | `0x84` | `shared_ptr<IWorldObjStatDisplay>`, `shared_ptr<ITutorialMsg>` destructors | 100% |
| `0x800AA920` | `0x88` | `shared_ptr<IWorldObjStatDisplay>::operator=` | 100% |

Evidence:

- **Getters.** The unit emits the inline virtuals in reverse declaration order: `GetPostEffect` to `GetTutorialMsg` end `#120`'s range, and `GetPauseMenu` to `GetWorld` follow with the same 8-byte `lwz r3, offset(r3)` body. The order of the source object's symbols is the order of the target addresses.
- **Deleters.** `get_deleter` compares the argument's type name with `strcmp` against the deleter's RTTI name and returns `this + 0xC`; `dispose` deletes the held pointer. The `CGameMap` and `CDividedGround` deleters are the two this unit defines (F-119-1). The target for `0x800AA650` mentions `CDividedGround` in its string reference.
- **Destructors.** `0x800AA6D0` calls `release_weak` and `__dl`; `0x800AA800` calls `release` and `__dl`. The first precedes `weak_ptr<IDlg>::lock` and takes its type. The second is the `shared_ptr` destructor the unit instantiates first, ahead of `IWorldObjStatDisplay`, which fits `shared_ptr<CGameMap>` (the `CDividedGround` destructor already matches elsewhere in the unit). Every `shared_ptr` destructor body is identical apart from relocation names, so the type is fixed by instantiation order, not by the bytes.

The residual 0.26% in both `get_deleter` functions is the relocation of the RTTI name, which stays a placeholder (`lbl_804B67xx`) in `symbols.txt`; the instruction stream is identical.

## Traps

- **Python resolves relative executable paths differently from Git Bash.** A script that runs `tools/local/binutils/powerpc-eabi-nm.exe` by relative path failed with WinError 2 under Windows Python; pass absolute paths with forward slashes.
