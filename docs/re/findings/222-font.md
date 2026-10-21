# F-222-1: Font globals and weak-pointer declaration context

Issue #222 owns 32 functions and 4,936 instruction bytes. This finding concerns only its final five functions, the complete Font.cpp unit at 80126240–801263E8. The preceding species, stage, sound and DCIM functions remain outside this reconstruction; no whole-issue completion is claimed.

## Native contract

The setter at 80126240 copies a weak owner into the eight-byte small-data global at 804B9090. Its caller in main.cpp constructs a weak FontMap temporary from the loaded shared owner, invokes the setter, and destroys that temporary. The existing canonical GetDefaultFontMap declaration returns shared_ptr<CLayout::FontMap>; its native eight-byte body passes that same global to weak_ptr::lock at 80126290. The lock returns the shared FontMap owner consumed by layout loading. These caller and public-interface contracts identify the global as weak_ptr<CLayout::FontMap>, independently of generic destructor instruction identity.

The global destructor at 80126360 is therefore the same complete weak FontMap specialization cleaned by main.cpp. Its global initializer at 801263C0 registers that destructor and a twelve-byte runtime registration node at 803E90C8. The .ctors entry is 803529E8. Native lock and destructor exception records occupy 8002C2C0–8002C2F0 and their index occupies 8004C908–8004C920.

## Declaration evidence

The pinned KooShnoo/pokemon-rumble source at commit 11c112f7548b26ab2cecfcf52e201b8d3b76120f, libs/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/detail/msl_smart_pointers.hpp, declares weak_ptr copy assignment in class and defines its ordinary copy/swap body out of class without inline. This header retains 2006 Freescale revision metadata but explicitly says it was adapted for interoperation with reverse-engineered headers. Its declaration structure is supporting evidence, not an untouched original-header claim.

The canonical approximation instead defines that same body in class. In the actual Font.cpp producer that emits the weak destructor before lock. Adopting the evidenced declaration structure preserves the API and identical body while naturally emitting lock before the weak destructor, as native code does. The extra shared FontMap destructor is an ordinary weak duplicate resolved to the existing earlier producer, not a retained fixture.

## Controls

Outside 222-reference preserves Font.cpp, Font.o, Font-primary.o, compile.json, the candidate header, and reusable audits. All five native bodies total 408 instruction bytes; every instruction after applying every relocation matches, including SDA addressing, the registered destructor and its registration node. Their two exception records total 48 bytes and every exception target matches.

The current-consumer control captures all 756 existing source objects before the header edit. After recompiling, every function and every allocated section and relocation is identical. Only compiler-generated anonymous numeric symbol spellings are normalized, by their exact section, value and size. Both the function and whole-section comparison receipts record zero changed objects. The full build and frozen-manifest executable verification exit zero. No source unit is marked Matching by this prerequisite alone.

## Traps

An identical weak destructor does not identify its element type. Use the setter caller conversion, the global shared-return getter and its actual layout consumers to establish FontMap ownership before naming this specialization.

Moving source global definitions to the bottom did not change weak helper emission order and is not adopted. The positive control changes a declaration structure supported by the pinned reference, rather than reordering declarations solely to request an address order.

# F-222-2: Complete Font.cpp source acceptance

The complete Font.cpp split is source-selected with Wii/1.1 and unchanged cflags_game. Its text is 80126240–801263E8, 408 instruction bytes across five functions; alignment padding remains separate from the counting denominator. The weak global owns .sbss 804B9090–804B9098, the runtime registration node owns .bss 803E90C8–803E90D4, and its constructor entry owns .ctors 803529E8–803529EC. The old inferred registration-node symbol included four bytes of next-object padding; its source-proven extent is twelve bytes and that padding remains outside the split.

The NonMatching split build passed the frozen verifier before source selection. The Matching build then passed configure, ninja, the original SHA-1 link check and frozen SHA-256 verification. unitdiff reports all five functions and 408 instruction bytes at 100%. Both native exception records, totaling 48 bytes, compare exactly including cleanup targets. Fresh main.elf.MAP rows attribute each owned function at its exact native address and size uniquely to Font.o. Ninja's actual final link inputs select build/wpse-v1/src/game/Font.o and contain no fallback for these five functions or owned state. The extra ordinary shared FontMap destructor and its exception record are discarded because the existing earlier specialization resolves it.

The complete source supplies native caller evidence for main.cpp's weak FontMap cleanup at 80126360. The original issue inventory remains 32 functions and 4,936 bytes; this publication accepts five functions and 408 bytes only. The preceding 27 functions and 4,528 bytes remain outside this source unit and the issue remains open.

No new source-generation trap beyond F-222-1 arose. A reused seed MAP can predate the current ELF when configure is run without --map; fresh source attribution was taken only after configure --map and a successful link.

# F-222-3: Independent DCIM continuation checkpoint

The unresolved producer state below is superseded by [F-222-5](222-dcim.md): indexing the typed filename field before pointer conversion resolves the final code difference, and the complete three-function DCIM unit is now source-selected with frozen verification.

The next independent investigated boundary is 80125B10–8012623C, three ordinary DCF filename/index functions. The first validates an eight-character directory name with three decimal digits (directory number at least 100) followed by five alphanumeric/underscore characters. The second validates four alphanumeric/underscore prefix characters followed by four decimal digits (file number at least one), permitting a following extension beginning with a dot. Both support negative explicit length by calling strlen. Holding each input character in an int local permits the same signed-char extension to serve classification and decimal accumulation. Their actual natural producer reproduces all 1,020 instruction bytes and every applied relocation target. A bounded additive isalnum body follows the existing locale-table ctype contract, outside only.

The full index-selection producer at 80125F20 reaches 800 bytes against native 796 with the exact 2,272-byte stack. Its current residual is a hoisted pointer to the directory filename suffix that uses an extra saved register; native recomputes that stack-relative address at its comparison sites. Wii/1.3 leaves this unchanged. Naming loop-local filename pointers increases the discrepancy and is not adopted. No flags, volatile storage or fabricated cache controls are used.

The real caller allocates two directory-search records 1,092 bytes apart and accesses attributes at offset 556 and short names at offset 557. This disproves allocating the current complete private PF_DTA (which includes a Unicode tail) as the game's public record. The public THPConv FA types at pinned e7f2a17eeff43bc6b822a01af66ce670822ab77b independently declare a distinct FADta ending with the OEM long filename; its natural size is 1,092. Older CC0 IPL corroborates that extent. The private PF_DTA Unicode fields are legitimate library state and must remain intact. The earlier public opaque alias to PF_DTA was sufficient for pointer-only async consumers, but cannot establish this allocation contract. Only factual type/field/API observations are used from the restrictive public header; no restricted implementation is imported.

The resulting complete public FADta correction and native FAChdir/FAErrnum declarations remain outside in 222-reference/dcim/include/revolution/fa.h. They require a separate bounded shared-interface proposal with actual async consumers and native symbol-binding controls before publication. The full three-function unit has not passed source-selected acceptance and receives no source credit.

The species combined-type getter at 801250D0 calls the small type-table lookup helper at 80125340, after the strong StageDesc functions at 80125180–80125334. This is a warning that the first-owner species/data source boundary may extend beyond the earlier 80125180 estimate. No competing StageDesc split is inferred solely from addresses.
