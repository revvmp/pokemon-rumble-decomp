# Exception data correspondence, 2026-11-05

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

## Scope

The object-data audit initially reported thirteen discrepant exception records despite exact source-selected CPU code and a byte-identical final executable. Two Gecko runtime records had separate size corrections owned by the coordinator. This investigation closes the other eleven records across six game units without changing source, original bytes, expected hashes, section extents, or the data denominator.

The evidence is the frozen original DOL, compiler source objects, dtk split objects, and the existing linked ELF. The independent `tools/re/map_exception_symbols.py` audit compares each table's complete raw bytes and every normalized relocation, with index records paired by their owning functions. A passing function percentage alone is not used as exception identity proof.

## Four stale cleanup identities

The following inferred target names disagree with the source-selected symbol at the same original address:

| Original address | Inferred target identity | Source identity proved by the linked symbol |
| --- | --- | --- |
| `0x800BCAA0` | `dtor_800BCAA0` | CGameStage's anonymous `nw4r::ut::NonCopyable` destructor, 64 bytes |
| `0x800F7710` | `__vec_deleter<shared_ptr<CPii>, allocator<shared_ptr<CPii>>>` | The allocator-reference specialization, 196 bytes |
| `0x80103040` | `__vec_deleter<int, allocator<int>>` | `__vec_deleter<MTX34, allocator<MTX34>>`, 112 bytes |
| `0x80103400` | Allocation guard for an integer-list node | Allocation guard for the map tree node containing the integer list, 96 bytes |

Every affected original exception pointer equals the corresponding source-linked function address. Function bodies and sizes agree. Some discarded specializations have identical instruction and exception bodies, so the old name-based matching could select the wrong source record without failing raw byte equality. Correcting the function correspondence therefore also requires six associated exception-table/index mapping updates in CPiiTex and CPpuEngine.

The existing CGameStage source emits `__dt__Q44nw4r2ut24@unnamed@CGameStage_cpp@11NonCopyableFv`; this audit uses that actual compiler identity, without asserting that earlier inferred namespace descriptions were original symbols.

## Thirteen missing pointer relocations

These are real pointers whose original split metadata omitted relocations, not pointer-shaped packed actions:

| Exception source addresses | Target | Proof |
| --- | --- | --- |
| `0x8002F5A4` | `0x800705F0`, `shared_ptr<CLayout>` destructor | ScriptLayout's source cleanup relocation and original word `800705F0` |
| `0x800352D8`, `0x800352E8`, `0x800352F8`, `0x80035308`, `0x80035318`, `0x80035328`, `0x8003533C`, `0x80035394` | `0x803BE408` | Source exception specification/catch relocations and exact `!CFiber::Terminated!` string |
| `0x800384AC`, `0x800384E0`, `0x800384F0`, `0x80038504` | `0x803BF3B8` | Source allocation exception-specification relocations and exact `!std::bad_alloc!` string |

The CFiber filter string is 21 bytes including its terminator, independently documented in [CFiber](307-cfiber.md). The allocation filter is 17 bytes, documented in [allocation interfaces](315-allocation-interfaces.md). The proposed descriptive target label `fiberTerminatedExceptionName` identifies the former; `stdBadAllocExceptionName` already identifies the latter. String bytes and source meanings remain unchanged.

## Private regeneration and exact audit

`tools/local/data-match-20261105/extab-corrections.json` records the correction packet, original word bytes, original DOL SHA-256, source-linked cleanup addresses/sizes, and mapping changes. Its original digest is the frozen `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

A private snapshot of the target configuration and symbols incorporates these proposals and runs the real pinned dtk command:

```powershell
tools/local/dtk.exe dol split --no-update -j 4 tools/local/data-match-20261105/extab-probe/config.yml tools/local/data-match-20261105/extab-probe/build
python tools/re/map_exception_symbols.py --project tools/local/data-match-20261105/extab-probe --output tools/local/data-match-20261105/extab-probe/audit-corrected.json
```

Both exit zero. dtk produces 897 objects. The audit has no errors or missing functions and reports:

| Unit | Exact tables | Exact indexes |
| --- | ---: | ---: |
| CGameStage | 17/17 | 17/17 |
| CPiiTex | 16/16 | 16/16 |
| CPpuEngine | 43/43 | 43/43 |
| ScriptLayout | 11/11 | 11/11 |
| CFiber | 8/8 | 8/8 |
| new | 2/2 | 2/2 |
| Total | 97/97 | 97/97 |

This proves target-object corrections, not production final-link acceptance. The coordinator owns shared configuration integration, normal Ninja, and frozen full-artifact verification. The private target objects are diagnostics and are never final-link inputs.

## Traps

- dtk relocation additions require `type: abs` and a symbolic `target`. A section-qualified address such as `.data:0x803BF3B8` in that field is accepted as a literal undefined symbol name, not resolved as an address. Objdump and normalized relocation comparison expose the error even though dtk exits zero.
- Correcting an identical-body cleanup correspondence can invalidate its existing exception-table mappings. Derive both table and index identity from the corrected function and independently compare all relocations afterward.
- Exact final executable bytes can coexist with missing relocations in original split metadata. Restore only pointers proved by source exception semantics and frozen original words; do not infer pointerhood from numeric appearance alone.
