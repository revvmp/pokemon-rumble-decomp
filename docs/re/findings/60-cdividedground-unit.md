# F-60-1: CDividedGround.cpp unit boundaries

Target: `wpse-v1`. Issues #60 to #65 survey the CDividedGround block as six address ranges, `0x80071220`–`0x800776A8`. This finding establishes that the six ranges are one translation unit, `game/CDividedGround.cpp`, and gives its exact section ranges. The split is in `config/wpse-v1/splits.txt`; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with it.

## Section ranges

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x8000BB5C`–`0x8000CD9C` | Lowest and highest exception records named by this unit's index entries; the next index entry (`0x800776B0`) points to `0x8000CD9C` |
| `extabindex` | `0x80045264`–`0x800455AC` | 70 entries, every one for a function in `.text` below; the neighbours index `0x80070D80` and `0x800776B0` |
| `.text` | `0x80071220`–`0x800776A8` | First function is the constructor; last is the `BSoundSource` destructor thunk at `0x800776A0` |
| `.rodata` | `0x80352AC0`–`0x80352B00` | Three tables referenced only from this unit; `0x80352B00` is first used by `0x800776B0` onward |
| `.data` | `0x8039B7B8`–`0x8039BFE8` | From the `.pkc` format strings to the end of the `exception` name; `0x8039BFE8` is the `CDlgCollection` deleter vtable |
| `.sdata` | `0x804B5E68`–`0x804B5F30` | `0x804B5F30` is used by `0x80079AF0` in the next unit |
| `.sdata2` | `0x804B9DB0`–`0x804B9DC8` | The preceding unit's last constant is `0x804B9DAC`; the next unit's first is `0x804B9DC8` |

The unit has no `.ctors` entry, `.bss` or `.sbss` of its own. The one-byte flag at `0x804B9110` that it writes is shared with many units and stays outside the split.

## Why one unit

- **Start.** The function before `0x80071220` is the third virtual of `CDisplayPhaseCtrl`, whose vtable, name and base list end at `0x8039B7B8` in `.data`. `0x80071220` stores the `CDividedGround` vtables (`0x8039BB68` and its `BSoundSource` part at `+0x48`) after calling the base constructor at `0x800528E0`, so it is the constructor and the first function of a new unit. This corrects the survey's assumption that #59 might extend into the block.
- **One class.** `.data` holds one cluster of out-of-line literals (`.pkc` names, `stage.brres` paths, `vector length error`, `rep_EX%02d`), then the deleter vtables and type names for `shared_ptr` of `nw4r::g3d::ScnGroupEx`, `ScnMdlSimpleData`, `CBSPTree`, `CModel`, `CPpuEngine` and `CDividedGround`, the `CDividedGround` vtable, RTTI name and base lists, and one cluster of template literals (`tree::insert length error`, `basic_string` errors, `default_delete` names). All deleter functions named by those vtables lie in `0x800769A0`–`0x800776A0`. No `.ctors` entry or second class name interrupts the range.
- **End.** `0x800776B0` starts `CDlgCollection`, whose deleter vtable follows at `0x8039BFE8`.

## Class layout evidence

The base lists give `CDividedGround : BModelObj`, with `BModelObj : BWorldObj` at 0, `BSoundSource` at `0x48` and `BKDTreeLink` at `0x64`; `BWorldObj : ICalc` at 0 and `std::tr1::enable_shared_from_this<BWorldObj>` at 4. The `CDividedGround` vtable has 16 primary slots and a 12-slot `BSoundSource` table. The constructor initializes members from `0xF8` to `0x16C`, including two arrays of two eight-byte objects built with `__construct_array` at `0x120` and `0x130`, and four self-linked list heads at `0x10C`, `0x14C`, `0x158` and `0x164`.

## Traps

None found while establishing the split.
