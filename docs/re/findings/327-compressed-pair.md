# F-327-4: Independently reconstructed compressed-pair storage

The header include/MSL_CPP/compressed_pair.h supplies the evidenced Metrowerks::compressed_pair interface for the #327 list and #53 vector investigations. It is independently authored from C++ empty-base rules and native member accesses. No restricted standard-library implementation body is reused. It is not included by any existing source unit, changes no source-unit status and adds no source-linked byte credit.

## Native consumer evidence

The native BUI list has a four-byte allocator/count pair at offset0 and an eight-byte allocator/sentinel pair at4. Declaration-only inspection of MSL-family list blob ef288c73338dc06514b30d9737fc2fea667fe8fb in Swiftshine/key independently identifies the pair member types in __list_deleter. The exact class hierarchy and allocation algorithms remain outside this header; their separate incomplete diagnostics are recorded in F-327-3, currently published in PR #890.

Actual game-profile Wii/1.1 compilation with this exact header reproduces all 148 BUI constructor bytes after applying all seven independently paired relocations: vtable803C0250, weak constructor801C88F0, reused weak destructor8007DAD0 and array constructor8033FECC. The seven native leaf/traversal bodies also remain byte-exact at420 bytes without relocations; the two independent four-byte rendering hooks remain unchanged. This proves the first-empty policy in actual container layout and generated instructions. The list destructor remains eleven register-word differences from its220-byte native body; no complete list acceptance is claimed.

The #53 owner independently compiled the same header for vector's capacity/allocator pair. This exercises the second-empty policy: capacity storage occupies4 bytes and the complete native pointer/count/capacity record occupies12. The vector destructor remains incomplete and receives no source credit. The #326 owner additionally verified pointer/pointer construction in custom-deleter control blocks. Its ten owned reset, ownership-guard and control-block consumers reproduce936 code bytes,18 text relocation sites,160 exception bytes, eight exception indices and264 initialized bytes with14 data relocations. This exercises nonempty member storage and pointer argument passing. Both independent consumers were repeated with the final exact proposed header. Distinct-empty storage remains without an accepted target consumer.

## Implementation scope

Empty-class detection uses a pointer-to-member substitution check followed by sizeof a derived class containing one byte. The actual Wii/1.1 compiler accepts these language constructs; the modern __is_empty intrinsic is unavailable in this compiler. Storage uses members for two nonempty types, one empty base plus the other member for a single empty type, and distinct empty bases when both types are empty and different. Equal empty types retain separate base/member subobjects. The public interface provides default and two-value construction plus const and mutable first/second accessors.

The two-value constructor uses the evidenced first_param and second_param aliases from call_traits. Declaration-only inspection of pair blob c58d840d and call-traits blob f3c956bc in KooShnoo/pokemon-rumble establishes the parameter policy: values no larger than a pointer pass by value, larger values pass by const reference, pointers pass by value and references preserve their reference type. The implementation independently expresses that policy with separate size selection and pointer/reference specializations. No restricted implementation body is reused. The actual #326 consumer proves the pointer/pointer policy; no additional native consumer is claimed for every other type combination.

This is a bounded internal utility for the observed container interfaces. Further instantiated policies or interfaces still require their own native consumer evidence before they can establish reconstruction credit.

## Repository preservation

On main813a7c745d081a82bf86e3ef76d7fbd54fc02e68, configure --map, complete plain Ninja, explicit all_source and frozen verification of the actual default-path DOL all exit0. The exact accepted879 snapshot a70262465efb08731e782639dfbb20d1056ac232 has the identical tracked tree as that main commit. Independent manifest validation and comparison prove all535 Ninja-declared source objects whole-file identical, and the complete ELF and DOL identical. The tracked-header BUI constructor/leaf/hook audits pass against this same checkout. This prerequisite does not incorporate the incomplete list or vector implementations.

## Traps

The initial const-reference-only two-value constructor caused an extra readonly zero load in one list diagnostic and incorrect instruction ordering in the independently investigated pointer/pointer reset consumer. Declaration evidence subsequently established call_traits parameter types. The corrected constructor retains the exact148-byte BUI constructor and its seven targets while also reproducing both200-byte reset bodies and their exception/data context. The list diagnostic continues to default-initialize its count pair and establish sentinel links in the owning container constructor. No artificial padding, register constraints, redundant guards or compiler-profile change is involved.
