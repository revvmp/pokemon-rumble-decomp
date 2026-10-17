# F-327-4: Independently reconstructed compressed-pair storage

The header include/MSL_CPP/compressed_pair.h supplies the evidenced Metrowerks::compressed_pair interface for the #327 list and #53 vector investigations. It is independently authored from C++ empty-base rules and native member accesses. No restricted standard-library implementation body is reused. It is not included by any existing source unit, changes no source-unit status and adds no source-linked byte credit.

## Native consumer evidence

The native BUI list has a four-byte allocator/count pair at offset0 and an eight-byte allocator/sentinel pair at4. Declaration-only inspection of MSL-family list blob ef288c73338dc06514b30d9737fc2fea667fe8fb in Swiftshine/key independently identifies the pair member types in __list_deleter. The exact class hierarchy and allocation algorithms remain outside this header; their separate incomplete diagnostics are recorded in F-327-3, currently published in PR #890.

Actual game-profile Wii/1.1 compilation with this exact header reproduces all 148 BUI constructor bytes after applying all seven independently paired relocations: vtable803C0250, weak constructor801C88F0, reused weak destructor8007DAD0 and array constructor8033FECC. The seven native leaf/traversal bodies also remain byte-exact at420 bytes without relocations; the two independent four-byte rendering hooks remain unchanged. This proves the first-empty policy in actual container layout and generated instructions. The list destructor remains eleven register-word differences from its220-byte native body; no complete list acceptance is claimed.

The #53 owner independently compiled the same header for vector's capacity/allocator pair. This exercises the second-empty policy: capacity storage occupies4 bytes and the complete native pointer/count/capacity record occupies12. The vector destructor remains incomplete and receives no source credit. The two complete exercised storage policies are the reason for this prerequisite; unused combinations are ordinary C++ implementation support rather than claims of additional recovered target types.

## Implementation scope

Empty-class detection uses a pointer-to-member substitution check followed by sizeof a derived class containing one byte. The actual Wii/1.1 compiler accepts these language constructs; the modern __is_empty intrinsic is unavailable in this compiler. Storage uses members for two nonempty types, one empty base plus the other member for a single empty type, and distinct empty bases when both types are empty and different. Equal empty types retain separate base/member subobjects. The public interface provides default and two-value construction plus const and mutable first/second accessors.

This is a bounded internal utility for the observed container interfaces. Further instantiated policies or interfaces still require their own native consumer evidence before they can establish reconstruction credit.

## Repository preservation

On main813a7c745d081a82bf86e3ef76d7fbd54fc02e68, configure --map, complete plain Ninja, explicit all_source and frozen verification of the actual default-path DOL all exit0. The exact accepted879 snapshot a70262465efb08731e782639dfbb20d1056ac232 has the identical tracked tree as that main commit. Independent manifest validation and comparison prove all535 Ninja-declared source objects whole-file identical, and the complete ELF and DOL identical. The tracked-header BUI constructor/leaf/hook audits pass against this same checkout. This prerequisite does not incorporate the incomplete list or vector implementations.

## Traps

Passing an integer literal through a const-reference pair constructor caused an extra readonly zero load in the list constructor. Default-initializing the scalar pair and establishing the sentinel in the owning container constructor reproduces the native constructor instead. This is ordinary value/container initialization; no artificial padding, register constraints, redundant guards or compiler-profile change is involved.
