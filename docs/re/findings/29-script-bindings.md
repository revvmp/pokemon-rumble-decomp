# Issue 29: original script binding names

## F-29-1: Complete native binding table

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This research recovers names, not source implementations or matching-code credit.

The original table is one 8,040-byte `.rodata` object at `0x80352EB8–0x80354E20`. It contains 670 records of twelve bytes. Index0 at `0x80352EB8` consists of three zero words. The other669 records contain a native function pointer, a pointer to a null-terminated ASCII binding name, and one metadata word. All669 names and669 native function addresses are unique. Every function pointer is an existing exact `.text` function start, with its existing size and attributes retained. The first live record is `0x80352EC4`; the final live record starts at `0x80354E14` and occupies twelve bytes. Thus the address range in F-0012 describes record starts, rather than an exclusive byte end.

The entire table's SHA-256 is `f5b7cc1fc2a1573c94cd016370fa79b8c6c2aa73ce1251b18dc785da317e6640`. [script-bindings.tsv](../script-bindings.tsv) records every index, table address/file offset, function address/file offset/size, original spelling, string address/file offset, raw metadata, chosen symbol and existing owning issue. These669 functions occupy141,152 existing code bytes; none of those bytes becomes reconstructed source through this change.

At `0x8010505C–0x8010506C`, native instructions load the table base into r5, load the word at `0x804B72B4` into r6, set r4 to0x400, and call `0x80170370`. The count word is670. The callee preserves the supplied count at object offset8 and the table pointer at offset0xC (`0x801703E8–0x801703F0`). This independently confirms the null slot belongs to the table and that the table has670 entries. Names `gScriptNativeBindings` and `gScriptNativeBindingCount` are descriptive chosen labels for these evidenced objects; their original linker spellings are unknown. No data size, type, section, alignment or split is changed.

The metadata values are0(107 records),1(239),2(188),3(54),4(33),5(6),6(5),7(4), and0xFFFFFFFF(33). Their distribution is consistent with an argument-count field and a special sentinel, but that interpretation is not established here. The TSV preserves the raw values without assigning a type or behavior.

The two strings from the issue question map as follows:

| Original binding spelling | Index | Record | Native function | String virtual/file address | Existing issue |
| --- | --- | --- | --- | --- | --- |
| `DEBUG_GetFixedPopPokemonMonsno` | 226 | `0x80353950` | `0x8013FFB0` | `0x803AAA70` / `0x3A6B70` | #244 |
| `GetPokemonVoiceId` | 613 | `0x80354B74` | `0x8015D080` | `0x803ACB88` / `0x3A8C88` | #274 |

## F-29-2: Naming confidence and existing decompilation scope

All669 function symbols previously had `fn_` address labels. Each now uses `ScriptBinding_` followed by the exact embedded spelling, including capitalization, underscores and existing typos. The prefix is a descriptive project namespace to distinguish VM entry functions from ordinary game functions; it is not a recovered native C++ namespace, mangling, function signature or class declaration. The binding strings themselves are original and high-confidence, and each pointer association is direct immutable-table evidence. We do not infer source filenames, types, class membership or callable ABI from a name alone.

Every renamed function remains covered by exactly one existing block issue in `docs/WORKSTREAMS.md`. The TSV identifies that owner. There are40 represented existing issues, including the VM's built-in bindings in283/284. This research establishes no new translation-unit boundary, so no newly scoped source unit requires another decompilation issue; the existing issues remain the follow-up work and retain all source ownership.

| Existing issue | Named functions |
| --- | --- |
| #231 | 1 |
| #232 | 5 |
| #233 | 17 |
| #234 | 2 |
| #235 | 4 |
| #237 | 23 |
| #238 | 27 |
| #239 | 31 |
| #240 | 16 |
| #241 | 35 |
| #242 | 21 |
| #243 | 25 |
| #244 | 32 |
| #245 | 8 |
| #246 | 15 |
| #247 | 23 |
| #248 | 23 |
| #249 | 29 |
| #250 | 23 |
| #251 | 16 |
| #252 | 14 |
| #253 | 6 |
| #254 | 6 |
| #257 | 9 |
| #258 | 16 |
| #261 | 24 |
| #262 | 18 |
| #263 | 3 |
| #264 | 6 |
| #265 | 4 |
| #266 | 5 |
| #269 | 23 |
| #270 | 24 |
| #271 | 21 |
| #272 | 25 |
| #273 | 24 |
| #274 | 28 |
| #275 | 12 |
| #283 | 9 |
| #284 | 16 |

The script archive input contains87 files under `orig/wpse-v1/extraction/expanded/00189/files/`; sampled files begin with bytes70 6B 1A20 and contain compiled binary data. A read-only exact-byte search found neither example name in these files. The table association is established by the DOL, not inferred from a raw script-string search. PKC instruction decoding and runtime call semantics remain the existing VM issues' work.

## Validation

The recovery procedure reads the original DOL header's eighteen file-backed section mappings to convert each virtual address to its file offset; it does not use a constant address bias. It validates the frozen original digest, null first record, count670, all669 unique readable names, all669 exact function starts, all existing function sizes and attributes, and unique existing issue ownership. After renaming, a complete address/section/attribute sequence comparison confirms that only names changed. The TSV is independently reparsed against the original table and current symbol file before acceptance.

Unmodified baseline `c4e4aff76d520311777a356164f8e55deff57110` passed `python configure.py`, plain `.venv/Scripts/ninja`, and the explicit frozen verifier, each exit0. The renamed-symbol configure, plain Ninja and explicit frozen verification also exit0, followed by independent reparsing of all669 TSV entries against the original table and current symbols. Exact committed-head/latest-main gates are recorded in the pull request after execution. No maintainer-only files or reference artifacts change. No source reconstruction, new unit split or percentage increase is claimed.

## Traps

- The first live table entry is not its base. Scanning from0x80352EC4 finds669 bindings but misses the null index0. Native caller/count evidence proves the actual base0x80352EB8 and670 slots. An inclusive final record address0x80354E14 also needs twelve bytes before the exclusive table end0x80354E20.
- A script-visible name does not establish a native C++ signature. Preserve original binding spelling while explicitly identifying the descriptive symbol prefix; do not fabricate mangled class or parameter names from the string alone.
- A failed raw string search in compiled PKC files does not invalidate an immutable function/name pointer pair. Treat table evidence and undecoded bytecode semantics separately.
