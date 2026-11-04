# Message final stack-tag match

## F-346-7: InitMsgResList empty-tag stores

The current Message unit matched 14 of 15 functions. `InitMsgResList` at `0x801EF6F0`, size 632 bytes, differed only in the stack offsets of two zero-byte stores produced by value-initialized empty POD tags in the vector constructor. Its fuzzy score was 99.98734%. The earlier source reconstruction and slot investigations are recorded in [F-346-6](346-cmsgres.md#f-346-6-current-primary-header-and-language-controls) and [sprint-a17](sprint-a17.md).

Twelve helper argument/reference variants did not repair the slots. Six comma/scope/loop forms and twenty named value-initialized or reference-bound tag forms also failed; the named forms remove the otherwise dead initialization stores. A unit-local inline specialization of the `__vec_constructor` for `shared_ptr<CMsgRes>` now declares the construction tag before the allocation tag. Immediately before each original call it writes zero through a volatile unsigned-character pointer to that tag's object representation. This retains exactly the two target byte stores while controlling their stack allocation order. No tag offsets, stack offsets, raw instructions or original bytes appear in source. The allocation and element-construction order remains unchanged, and the type has no state beyond its empty object representation.

This is a D-0016 fake match. The explicit volatile byte writes express the target compiler's dead temporary initialization, rather than plausible original container code. The specialization affects this source unit only; `include/vector` is unchanged. All 15 function bodies, including the static initializer, compare at 100%, and their source order equals the target order. The coordinating parent then completed ninja with exit status zero and verified the frozen executable SHA. Message is accepted as Matching, with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The full source unit now links without its original split fallback.

## Traps

An empty named tag can retain its stack storage while losing the value-initialization byte store. Explicit writes through the unsigned-character object-representation view preserve the store without inventing a data member or changing the tag type. Declaring the two tags in the opposite order controls the two observed stack slots. Keep this accommodation local and verify the complete function; a shared container change would affect other units.
