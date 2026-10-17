# F-804-1: VoiceManager and Util header prerequisites

Target `wpse-v1`. This prerequisite corrects the canonical Voice layout and VoiceManager memory-size signature, and declares the existing Util low-pass constants/table needed by the owned #804 sources. It changes no source selections, split boundaries, function extents, base classes or virtual methods. Voice source ownership remains with its first-function issue; only the shared layout needed by VoiceManager is corrected here.

## Target layout evidence

`VoiceManager::GetRequiredMemSize(int)` at `0x80290510` begins with `mulli r3,r4,0x104`. The explicit count argument is in r4 after the implicit this pointer; the canonical declaration lacked that argument. The instruction and Setup's allocation stride independently establish Voice size `0x104`. VoiceManager list operations use node offset `0xFC` and priority offset `0xA8`. UpdateAllVoicesSync loads/stores a halfword at `0xA2`, rather than the canonical header's byte.

| Field | Target offset/type | Direct evidence |
| --- | --- | --- |
| mVoiceOutParamPitchDisableFlag | `0xA1`, bool | InitParam clears byte at `0x8028DD60`; pinned reference names the flag |
| mSyncFlag | `0xA2`, u16 | InitParam halfword store; SetBiquadFilter halfword OR/store at `0x8028EBB8`-`0x8028EBC0`; manager sync accesses |
| mRemoteFilter / mBiquadType | `0xA4` / `0xA5`, u8 | InitParam and filter setters |
| mPriority | `0xA8`, int | AllocVoice and ChangeVoicePriority |
| mBiquadValue | `0xB8`, f32 | InitParam store and SetBiquadFilter loads/stores |
| mRemoteOutVolume | `0xD4`, four f32 values | InitParam and SetRemoteOutVolume indexed access |
| pitch / volume / VE values | `0xE4` / `0xE8` / `0xEC` / `0xF0` | InitParam stores |
| pan mode / pan curve / list node | `0xF4` / `0xF8` / `0xFC` | InitParam and manager list operations |

The former remote-send and remote-FX-send arrays are absent from this release's layout. Removing them while adding the missing flag, halfword width and biquad value gives the evidenced size and offsets without anonymous padding. Existing public declarations are preserved. The class's inherited DisposeCallback layout and four AxVoice/VoiceOutParam output slots remain unchanged.

## Source provenance and isolated probes

CC0 zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, the Voice, VoiceManager and Util headers and corresponding source files, supplies member/function names and the lookup tables. The files identify kiwi515/ogws as their original source. The reference's field-offset comments are not treated as target evidence; the instructions above establish the target revision.

With the corrected scratch declarations and otherwise unchanged Wii/1.1 NW4R flags, all eleven retained VoiceManager functions reproduce every non-relocated target instruction word at the original extents. The former header produced only immediate/offset differences in those bodies. All eight retained Util functions also reproduce every non-relocated instruction word; the complete named pitch, volume, pan, remote-filter and low-pass tables have unique byte-identical occurrences in the frozen DOL. The additional Util declarations are CALC_LPF_FREQ_INTERCEPT, CALC_LPF_FREQ_TABLE_SIZE = 24 and CalcLpfFreqTable. These are diagnostic source probes; final source selection, relocation/data and full-link acceptance belongs to the later source change, and no reconstructed byte credit is claimed here.

## Native caller and full-artifact gate

The unchanged baseline is main `177199a89ea445c85e192c939ae91c0ad7a7d78e`. Its fresh default full build, explicit frozen verification and all_source target pass before saving all 441 registered native object outputs. After only these three header changes, configure, full default Ninja, all_source and explicit frozen verification again exit 0. Of 441 complete object files, 439 are byte-identical. snd_AxManager.o and snd_Channel.o differ only in numeric compiler-local names in .strtab; every allocated section, raw RELA entry, symbol value/size/type and symbol order is identical.

The actual rebuilt DOL directly equals the frozen reference: 4,094,880 bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The isolated caller comparison remains scoped to that baseline; the exact committed head must pass configure, full Ninja and explicit frozen verification again after its last latest-main rebase. Scope excludes the boot loader, other package contents and WAD packaging.

## Traps

- A canonical imported class can belong to another revision. VoiceManager retained the correct function sizes but differed only in field offsets and allocation-stride immediates until Voice's target layout was corrected. A reference header's own offset comments are insufficient; the size-producing instruction, typed field accesses and complete native caller comparison establish this correction.
- As in F-444-2, header parsing can change only compiler-local numeric names. The two changed whole-object hashes are disclosed separately from the exact allocated sections and final artifact.
