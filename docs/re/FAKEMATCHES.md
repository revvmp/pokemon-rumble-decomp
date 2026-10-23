# Fake-match ledger

Functions matched with source that is probably not the original, under [D-0016](../DECISIONS.md#d-0016-documented-fake-matches-for-the-remaining-long-tail). Replace an entry with plausible source when one is found, then remove its row.

| Unit | Function | Fake construct | Honest attempts |
| --- | --- | --- | --- |
| CDlgPiiList | `.data` order of the `shared_ptr_deleter<CUIManager>` vtable | `typedef char UIManagerPtrStorage[sizeof(std::tr1::shared_ptr<CUIManager>)];` before `PiiItemVector`, which completes `shared_ptr<CUIManager>` before the `mItems` vector | [sprint-ida](findings/sprint-ida.md#fixes-from-the-follow-up-session), [sprint-a17](findings/sprint-a17.md#cdlgpiilist-flip-attempt) |
| CDlgPiiList | `CDlgPiiListImpl::OnAccept` (inlined `CountRemoteUnmarked`) | dead `if (false)` block in `CountRemoteUnmarked` that dereferences and increments a null `const_iterator*`, so `*` and `++` are referenced before `begin` | [sprint-ida](findings/sprint-ida.md#fixes-from-the-follow-up-session), [sprint-a17](findings/sprint-a17.md#cdlgpiilist-flip-attempt) |
| ScriptGame | `MakeMapID` | `#pragma opt_dead_assignments off` around the function | [sprint-a13](findings/sprint-a13.md), [sprint-ra](findings/sprint-ra.md#diagnosed-not-fixed) |
| ScriptGame | `IncMapClearedNum` | `#pragma opt_common_subs off` around the function | [sprint-a03](findings/sprint-a03.md), [sprint-a05](findings/sprint-a05.md) |
| main | `HandleFileError` (inlined `UsesAlternateErrorMessage`) | `int number = drive;` copy so `drive < 2 \|\| number >= 512` is not folded into one range compare | [sprint-a12](findings/sprint-a12.md#unsolved) |
| main | `main` (call to `ResourceFileConfig::SetErrorHandler`) | `__declspec(noinline)` on `SetErrorHandler` | [sprint-a12](findings/sprint-a12.md) |
| CFileData | `CFileData::CWorker::CWorker` | `BThread` derives from an empty `BThreadHead` whose constructor clears `mpHead`, so that store precedes the vtable store | [sprint-a02](findings/sprint-a02.md#diagnosed-not-fixed) |
| `game/CEffect` | `InitializeEffect` | `GetEffectHeapSize` inline helper for the heap size, no `numEffects` local, and the pool setup moved into `CreatePools`/`CreatePoolsImpl` defined after `AllocHeap` (fixes the `EmitterWithParam` constructor emission order) | [sprint-a15](findings/sprint-a15.md), [sprint-a17](findings/sprint-a17.md), [sprint-w10](findings/sprint-w10.md) |
| CFileData | `map_base<CNTHandleWithMutex*, weak_ptr<CNTHandleWithMutex>>::erase` | explicit instantiation at the top of `CFileData.cpp` under `#pragma opt_dead_assignments off` (with the plausible named `next` copy in `map_compact_tree::erase_next`) | [sprint-a03](findings/sprint-a03.md#fixes), [sprint-a02](findings/sprint-a02.md#fake-match-wave-d-0016) |
| CFileData | `shared_ptr<CWorker>::reset<CWorker>` (inlined `PushFinished`) | `pHead = static_cast<CJob*>(mFinished.mpHead)` copied into `pTail` | [sprint-a02](findings/sprint-a02.md#fake-match-wave-d-0016) |
| `TMC_JPEG/tmcjpeg_rgb565` | `ConvertRGB565MCU420` | `(u64)` cast on the second texel load of each pixel pair | [764-tmc-jpeg-converters](findings/764-tmc-jpeg-converters.md), [sprint-perm](findings/sprint-perm.md) |
| `TMC_JPEG/tmcjpeg_rgba8` | `ConvertRGBA8MCU422` | second pixel luma written out instead of `RGBA8_LUMA`, with a `(u32)` cast on the AR row term | [764-tmc-jpeg-converters](findings/764-tmc-jpeg-converters.md), [sprint-perm](findings/sprint-perm.md) |
| `TMC_JPEG/tmcjpeg_rgba8` | `ConvertRGBA8MCU422` | redundant `(u32)` cast on the row offset of the second pixel's AR texel (from decomp-permuter) | [F-764-1](findings/764-tmc-jpeg-converters.md) |
| `TMC_JPEG/tmcjpeg_rgb565` | `ConvertRGB565MCU420` | `(u64)` cast on the second texel load (from decomp-permuter) | [F-764-1](findings/764-tmc-jpeg-converters.md) |
