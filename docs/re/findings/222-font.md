# F-222-1: Font globals and weak-pointer declaration context

Issue #222 owns 32 functions and 4,936 instruction bytes. This finding concerns only its final five functions, the complete Font.cpp unit at 80126240–801263E8. The preceding controller, sound and DCIM functions remain outside this reconstruction; no whole-issue completion is claimed.

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
