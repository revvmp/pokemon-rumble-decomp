# PiiPropData primary MSL reference recovery

The bounded interface review follows [the count-lifetime investigation](piipropdata-message-descriptor-tail.md) and [the allocation-tag investigation](piipropdata-allocation-tag-tail.md). It recovers the same original library distribution previously recorded by [the photo-saving investigation](280-photo-saving.md), rather than treating an adapted public decompilation header as an original interface.

## Recovered input and provenance

The reading reference is the September 2006 CodeWarrior archive [CW_GCN-3_0_Alpha5-2-060920.zip](https://archive.org/download/cw_consoles/Wii/CW_GCN-3_0_Alpha5-2-060920.zip). The downloaded 47,131,387-byte archive has SHA-256 `6bac434a2ff28f75c44fdd9606b3d58c3ff302b081a8717e137417af1c98a515`, exactly the earlier recorded value. No expected hash was changed.

The primary headers are under `CW_RVL/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include/` within that archive. They retain this notice: “Copyright © 1995-2006 Freescale Corporation. All rights reserved.” They were read privately as interface and code-generation evidence; no full proprietary header is added to tracked source.

| Header | Revision | SHA-256 |
| --- | --- | --- |
| `stdexcept` | 1.25.2.2, 2006-02-09 | `1b63e79a2a1e3a523c17286206fc26bfb6d5d57be37f9e4870f2d3e634e2bd08` |
| `exception` | 1.32.2.2, 2006-02-09 | `bc86a918678dc565ce4f74923bd227860d5afe8a9d224e68d3810d3cf62680de` |
| `MSLstring.h` | 1.26.2.3, 2006-02-09 | `388315c6fc7b7554e4d32040acec15a8548c45a9e278a042f6a087a613ac25b8` |
| `vector` | Primary archive member | `ed11b3e9b051ed59c78e57f853e985012814be2100f676ee6279aca5a508f3ee` |
| `msl_utility` | Primary archive member | `82b8dead34ea950f0160e8defee1a4c73eef8fb82bc74363e5ca56ba91780f51` |

The vector digest independently agrees with the earlier primary-reference digest in [sprint-cx](sprint-cx.md). The private archive, extracted reading references and manifests are under `tools/local/tail-20261104/vm_pii_primary/`; `vm_pii_primary_fetch.py` reproduces acquisition and verifies the archive digest before reading members. Additional header receipts are in `more_manifest.json`.

The pinned public [KooShnoo stdexcept reconstruction](https://github.com/KooShnoo/pokemon-rumble/blob/11c112f7548b26ab2cecfcf52e201b8d3b76120f/libs/PowerPC_EABI_Support/MSL/MSL_C%2B%2B/stdexcept) and [Swiftshine stdexcept reconstruction](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/PowerPC_EABI_Support/MSL/MSL_C%2B%2B/stdexcept) omit message copying in their logic-error bodies. They are not sufficient evidence for the target's complete exception construction. The separate [HeartGold reconstruction](https://github.com/ctrl-cheeb-del/pokeheartgold/blob/29190e5fb7238c424ea97075e711f58cde55c5d8/lib/MSL_C/src/msl_vector_length_error.cpp) uses a similar descriptive `__error_string` holder, but is an ARM reconstruction and is not substituted for the original MSL header.

## Exception interface evidence

The primary `stdexcept` declares both `const char*` and `const string&` constructors. Its character-pointer overloads directly delegate from `length_error` to `logic_error`, then to the owned message holder. `logic_error` and `length_error` have implicit destructors. A string-reference overload exists, but its presence does not mean the literal throw takes that overload.

The primary holder's name is `_MSLstring`, whereas this reconstruction uses the descriptive `__error_string`. Its public declarations are, in order, the character-pointer constructor, the string-reference constructor and `c_str`. The character-pointer constructor is not explicit. That constructor and the getter have explicitly-inline out-of-class definitions; the string-reference constructor is declared out of line. No copy constructor, copy assignment or destructor is declared. The sole member is `tr1::shared_ptr<char>`. The body allocates and copies the character message, using an array deleter and a const-pointer conversion followed by removal of const on the copy destination.

The original `exception` explicitly declares empty default/copy constructors, an assignment returning the receiver, a virtual destructor and `what`, with the library's nonthrowing specification. The archive's default `_MSL_NO_THROW` expansion is `throw()`; `_MSL_CANT_THROW` is a distinct compiler attribute. No annotation claiming that an owning destructor has no side effects is supported by this reference.

The target's independently retained constructor chain corroborates the selected character-pointer interface:

| Native address | Current descriptive identity | Native behavior |
| --- | --- | --- |
| `0x80087010` | `logic_error(const char*)`, 84 bytes | Preserves the second argument, constructs the exception base, then forwards that argument unchanged to the member at object offset four. |
| `0x80087080` | `__error_string(const char*)`, 120 bytes | Sends the unchanged second argument directly to `strlen`, allocates its character buffer, constructs the shared owner, then passes the same argument to `strcpy`. |
| `0x80087120` | `length_error(const char*)`, 60 bytes | Forwards the original second argument unchanged to logic_error and installs the derived vtable. |

These routines belong to the already source-linked CDlgMsgWin unit. Their native reads do not dereference a string object to obtain character data. Together with the primary overload declarations and literal caller, they support the existing selected interface. The holder class spelling is reference evidence; no global symbol rename is part of this Pii investigation.

## Bounded source controls

`vm_pii_primary_context.py` applies only evidenced differences to private reconstructed-header overlays: out-of-class inline holder definitions; the primary copy-destination casts; the non-explicit constructor; both string-reference overload declarations/delegation; exact constructor/getter declaration order; and the recovered `_MSLstring` identity with a compatibility alias. All six valid variants retain the strongest 99.73283/99.77778 percent constructor/Reset pair, native 1,048/1,260-byte extents and all 46 surrounding original functions.

`vm_pii_primary_exception.py` adds the original exception copy/assignment members, then the complete valid nonthrowing contracts with compatible visible derived overrides. Both controls are neutral. All eight candidates were audited against all 26 original exception records: the same two saved exception-object register bytes remain different in the constructor and Reset, and the other 24 records and all cleanup identities are exact. No alternate string-reference or conversion-wrapper API is justified by these controls.

The unchanged results reject these particular exception-header differences as a sufficient explanation for Pii's remaining register swap. They do not assert that every reconstructed MSL header is equivalent to the primary library.

## Vector boundary handoff

The recovered vector header was passed to the independent animation lane. It supplied concrete copy-constructor, member-template initialization and copy-algorithm boundaries that the specialized reconstruction had flattened. The combined private candidate `animcurve_pii_primary_copy_1_0_0`, using the `animcurve_pii_primary_init_0_1_0/vector` overlay, reproduces all 48 original functions and all 26 original exception records with cleanup relocations. Each boundary alone had regressed; the combination is material. The [primary-boundary finding](piipropdata-primary-msl-tail.md) records the complete raw-byte and relocation proof, compiler trace, negative controls and normal-build acceptance.

## Production fidelity and retained data review

The reviewed production packet is `game_PiiPropData/pf_pii_production` under the private tail workspace. Its three ordinary source files are `PiiPropData.cpp`, `PiiPropDataAccessors.cpp` and `PiiPropDataIterators.cpp`. The [emission finding](piipropdata-emission-tail.md) records their exact ordering, rejected alternatives and final source scopes. Primary-source recovery and the remaining source controls have different evidential status:

| Source feature | Evidence and status |
| --- | --- |
| Copy constructor delegates through `base::init`, whose unsigned count owns the allocation and construction calls | Recovered directly from the verified primary vector header. |
| Trivial range construction uses `std::copy`, then its own unsigned pointer difference; the POD copy helper has a separate unsigned `size_t` count | Recovered directly from the verified primary vector and algorithm headers. |
| Ordinary length-error throw, owning message copy and implicit derived cleanup | Supported by the primary exception declarations and independently retained native character-pointer constructor chain. No explicit Runtime throw substitute is retained. |
| Caller-owned `PiiPropertyAllocationTag`, const-reference forwarding and volatile representation-byte write before allocation | Remaining D-0016 accommodation, documented in the [allocation-tag finding](piipropdata-allocation-tag-tail.md). It is not represented as recovered original source. |
| Short-vector growth and assignment helper boundaries | Earlier source-only frame/register controls remain; primary copy recovery does not establish their original spelling. |
| Deferred SpeciesMoves/vector bodies, two-level assignment wrapper, discarded iterator requests, native sections and separate strong typed accessor/copy definitions | Remaining D-0016 emission controls. Their purpose is retaining the native function order and exception/literal ownership without an original object or byte patch. |

The shared-header additions are scoped by the Pii copy, emission, accessor and iterator macros. The retained main source uses ordinary class construction, allocation and throw expressions. The obsolete caller-message output, address enum, manual Runtime throw alternatives and register-binding experiments are absent.

An independent audit reads the compiler objects, the native linked ELF and the frozen original DOL. It verifies the exact retained data rather than relying on function scores:

| Address | Bytes | Compiled and linked content |
| --- | ---: | --- |
| `0x803A91C8` | 20 | Nul-terminated `vector length error` |
| `0x803A91DC` | 55 | Nul-terminated `!std::exception!!std::logic_error!!std::length_error!!` |
| `0x804B70E8` | 4 | Short-vector sort shuffle state, signed value -4 |
| `0x804B70EC` | 4 | LevelMove value-comparator sort shuffle state, signed value -4 |
| `0x804B70F0` | 4 | LevelMove reference-comparator sort shuffle state, signed value -4 |

The following four bytes at `0x804B70F4` are native linker fill for the next eight-byte-aligned RTTI at `0x804B70F8`; no source array was introduced for that gap. The two accessor/iterator fragments contain no allocated data. All allocation/throw code remains in the primary object and retains one 75-byte literal pool. The compiler's extra unselected metadata is not mistaken for additional retained data.

`vm_pii_retained_data.py` and `vm_pii_production_retained_data.py` record this independent audit for the first exact packet and the final production basenames. Their JSON receipts also prove that the native main input is byte-identical to the compiled main object and that both complete trial DOLs equal the frozen input. The production main object has SHA-256 `2ee79b9f875f87fb89c1fd2879c52dbcafc18e50894a94fe69ffdbe75031d7bd`. The production native verification receipt is `links/pf-piiprop-production/verification.json`: PASS, 4,094,880 bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The coordinator subsequently accepted the same source group through the normal configure/build/verification workflow. The complete executable passes the unchanged frozen hash; all 48 Pii functions and 26 original exception records remain exact. This completes the Pii CPU-source unit while leaving the separately disclosed coprocessor-code and package scope unchanged.

## Traps

- An adapted public library header can omit an essential operation while retaining plausible class and method names. Confirm its body and provenance before treating it as an original interface.
- Missing private research archives need not mean their evidence is lost. The archive URL and immutable digest in the earlier photo-saving finding recovered the same primary vector header, independently confirmed by its recorded digest.
- A neutral exception-interface audit does not rule out adjacent library boundaries. The recovered primary vector and copy-helper boundaries supplied the successful combined control after the exception-only alternatives remained unchanged.
