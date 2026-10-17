# F-760-1: Extension text table dimensions

The message parser at `0x801862B0` stores quote-delimited text pointers at `HomeButton + 0x2DC + j * 4 + k * 4`. It increments `j` for each opening quote, resets it after ten entries, and increments `k` at that point. This is the same two-index loop as `set_text`, with one language slot per entry instead of six.

Declare `mpTextEx` as `wchar_t* [10][1]`. The forty-byte extent and every following member offset remain identical. This corrects the imported flat declaration; no existing source currently accesses this member. The reconstruction of issue #760 needs the native two-dimensional access to preserve instruction selection. A trial using the flat index `j + k` scored 77.85%; the two-dimensional declaration and access reproduce all 132 parser instruction bytes at 100% with Wii/1.0a and `cflags_hbm`.

Reference: Kirby's Epic Yarn, `Swiftshine/key` commit `7f2a9ab9`, CC0, `src/hbm/homebutton/HBMBase.cpp`, NAND `set_text_ex`. The native stores and class extent establish the dimensions for this target independently of reference field names.

This prerequisite changes only the shared declaration. The three extension method bodies remain for the separate issue #760 pull request. No reconstruction progress is claimed by this header change.

Validation: `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0 on the prerequisite commit. The existing unit remains `NonMatching` and links its original fallback.

## Traps

A flat pointer array with index `j + k` preserves the mathematical address but changes the generated parser registers and arithmetic. Preserve the evidenced row and language dimensions. Evidence: F-760-1.
