# CPiiListFilterSort source-link acceptance

The normal configure/ninja pipeline now links CPiiListFilterSort from source. All 71 real functions, 27,656 code bytes and 6,753 data bytes are accepted. Ninja exits zero, and the frozen 4,094,880-byte main.dol passes direct comparison and SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. All 46 exception records and their index order agree in the accepted native link. The broader original-address audit finds no previously exact function regression or missing exact function.

The last function was the shared-pointer comparator-reference sort, completed by the scoped control-block load documented in [sort allocation](sort-tail-20261104.md#shared-pointer-sort-allocation-and-final-exact-control). Function completion alone did not establish the source link: the first complete candidate still emitted functions in a different order and shortened retained data.

## Recovered enum and menu layout

The original RTTI string at `0x803A8E30` spells the three-argument bound callback with `CPiiListFilterSort::FilterType` in both its member-function and bound-tuple types. The reconstruction had substituted `int`, preserving every instruction but shortening the type name twice. The original enum spelling is direct embedded evidence.

`PreviewTypeFilter` now receives the actual nested enum, the bind expression retains that type, and `mFilterType` plus its saved values use FilterType. The member remains four bytes at class offset zero. Ten inferred helper identities in `symbols.txt` now use the compiler's corresponding enum mangles. They lie at `0x800E8AB0`, `0x800E8CE0`, `0x800E8D90`, `0x800E8E60`, `0x800E8FC0`, `0x800E9070`, `0x800EBF30`, `0x800EC010`, `0x800EC110` and `0x800EC270`. Their native addresses and bytes remain unchanged.

The original type-menu parameter block at `0x80352DC8` has 17 items, three columns and six rows. The previous source used one column and 17 rows. Both forms scored the functions identically; only retained-data comparison rejected the wrong layout. The source now uses the proven 3-by-6 grid.

## Natural function and metadata order

The comparator definitions now follow their original order: favorite first, oldest, newest, descending BP, ascending BP, species name and species order. IsNameBefore belongs immediately before ShowPrefixFilterMenu. These ordinary source-order corrections preserve every function body.

A discarded source-local RequestPiiMaskAccessors function requests the two existing item-mask accessors before the filter constructor. Its named-false body emits no retained code or data. The generic one-argument function call operator is declaration-only under `GAME_PII_FILTER_EMISSION_LAYOUT`, with its unchanged typed body after the source definitions. This moves its weak generation after the callback copy constructor. Including CUIMenuItem before CUIMenu restores their original reverse RTTI declaration order. All 71 retained functions then appear naturally in the original order; no native custom code sections or fragments are needed.

The discarded request and scoped template deferral are D-0016 controls. Earlier ordinary definition order was insufficient for these two compiler-generated waves. The source still uses the real item accessors and callback implementation; it does not introduce replacement behavior.

## Placeholder ownership and denominator correction

Four callback bind expressions read the unit-local `std::tr1::placeholders::_1` at `0x804B9028`. The shared tuple header already declares this empty type as static storage. Its earlier inferred symbol `lbl_804B9028` incorrectly included seven following alignment bytes, and its automatic split also included four preceding alignment bytes.

Selecting the source while retaining that automatic split emitted a second placeholder at `0x804B9030`, shifting later small-BSS globals by eight bytes. The source object's native eight-byte section alignment supplies the original leading gap, and the next object's alignment supplies the trailing gap. An isolated native link omitting the obsolete automatic input reproduced every original byte. The normal split now assigns only `0x804B9028` through `0x804B9029` to CPiiListFilterSort, with the proven local placeholder name and one-byte size.

dtk consequently removes 11 padding bytes from its data denominator and one automatic unit: total data becomes 1,788,735 bytes across 1,388 units. The executable layout and frozen hashes do not change. No padding array or copied original data is added, and padding is not credited as reconstructed source.

## Evidence and reproduction

Private probes are under `tools/local/tail-20261104/game_CPiiListFilterSort/`. `filter_callback_deferred` establishes all 71 function positions. `filter_enum` corrects the proven type. `filter_data_order` adds RTTI order and the grid correction. `filter-aliases.json` records the ten address-checked enum identities. The native `links/filter-placeholder-owner/verification.json` passes, and the canonical build is recorded in `filter-source-build.log` plus `build/wpse-v1/verification.json`.

The accepted source uses the normal build flags and existing headers with narrowly scoped declarations. The selected final-link objects and unique-basename aliases remain byte-identical. No object, relocation, instruction or expected-hash patch is part of the build.

## Traps

- A callback reconstructed with int can have exact instructions while its original RTTI proves an enum parameter. Inspect complete template type strings before accepting metadata or guessing padding.
- A perfect function score does not verify a copied menu-parameter block. The target's column and row counts required a separate retained-data comparison.
- An empty placeholder object still has local ownership. Keeping its old automatic split after selecting source duplicates storage and shifts unrelated globals; claim only the evidenced byte and let native alignment reproduce the gaps.
