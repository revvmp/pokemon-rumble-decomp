# F-444-4: WaveSoundHandle and combined source checkpoint

Target `wpse-v1`. The complete WaveSoundHandle translation unit retains only DetachSound, 56 native bytes at `0x802917E0`-`0x80291818`. The following eight alignment bytes belong to the following allocation rather than this split. The class has its WaveSound pointer at offset zero, size four; original detach accesses and exact source instructions establish the layout.

The readable source follows CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee`, identifying kiwi515/ogws provenance. It includes the original constructor behavior, which this target does not retain from this unit. The compiler strips that unused body normally. No original bytes implement the source.

## Acceptance

The split-only full default Ninja and explicit frozen verifier exit 0. NonMatching all_source compilation uses unchanged Wii/1.1 cflags_nw4r. The retained function and .text score 100% in objdiff. Normal Matching selection passes full default Ninja and explicit actual-output frozen verification. The independent audit compares all 56 raw native bytes, confirms neither original nor source function has relocations, checks the final function address/extent, verifies source selection without the original unit, and directly compares the complete rebuilt DOL to the frozen reference.

The combined checkpoint includes seven source-selected units: WaveArchive (2 functions / 252 bytes), WaveFile (4 / 600), WaveSound (15 / 864), WaveSoundHandle (1 / 56), WsdFile (4 / 956), WsdPlayer (25 / 4,032) and ADPCM (1 / 140). All 52 functions / 6,900 native bytes pass their respective independent audits on the same build, including all 233 paired code relocations, 31 vtable/constructor-pointer relocations and retained data. No unit selects its original fallback object. Their complete DOL equals all 4,094,880 frozen bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

This supplies all 37 original #444 entries / 5,236 native bytes. The complete #443 inventory additionally needs seven Util tail entries / 748 native bytes from the separately accepted complete Util dependency in PR #843. Final acceptance of both issues waits for that dependency on main, separate complete original-inventory audits on the same committed head, and final exact latest-main configure/full Ninja/frozen verification. Bytes are credited once. Scope covers only main.dol.

## Traps

- Ending this split at the next function start would assign following alignment padding to the source unit. The native end is `0x80291818`; both split-only and source-selected full links verify with that exact boundary.
