# F-486-3: DSP task priority is volatile

This prerequisite for issue #486 corrects only the qualifier of `DSPTaskInfo.priority` in the imported `include/revolution/dsp.h`. Its unsigned 32-bit type, name, offset 4 and structure layout remain unchanged.

The pinned CC0-1.0 [ogws DSP task header](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/include/revolution/DSP/dsp_task.h) declares the corresponding member `volatile u32 prio` at offset 4. Target `__AXOutInitDSP` at 0x802DC110 stores zero to this member at 0x802DC168 (`stw r12, 4(r31)`), before the later task setup stores. The #486 AXOut reconstruction with Wii/1.0 and existing SDK flags scored 93.55932% for this function with the unqualified member. Restoring the reference qualifier produces 100%; all twelve AXOut functions then compare at 100%. This is evidence for the member qualifier, not a change to the SDK task layout or a claim that AXOut is source linked in this prerequisite.

The header change is isolated because it corrects a shared declaration used by other units. Its clean full build uses `python configure.py`, `.venv/Scripts/ninja.exe`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`. All three commands exited 0 on the clean build; the DOL passes the frozen digest and direct byte comparison. No source coverage is added by this header prerequisite; final AXOut linkage is recorded by the source pull request for #486.

## Traps

When the AXOut task initializer differed only in store scheduling, retaining the reference volatile qualifier restored the target instructions. Do not reorder assignments to compensate for a missing shared-type qualifier. The evidence here is specific to `DSPTaskInfo.priority` and this Wii/1.0 reconstruction.
