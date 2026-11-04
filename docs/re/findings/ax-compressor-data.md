# AX compressor lookup table

The existing __AXCompressorTable symbol at0x803D8620 occupies4032bytes before the separate axDspSlave microcode. Its established public type is const u16[], and AXCL assigns it to sCompressorTable when enabling compressor commands. The reconstructed C definition contains2016 unsigned16-bit lookup values, decoded from the sealed reference and retained as ordinary numeric initializers. These are compressor data; the adjacent DSP instructions remain an explicit original fallback and are not copied into this source.

The native compiler defaults const data to .rodata. The target stores this table in .data, so the translation unit selects that section before including the header containing its first extern declaration. Placing the pragma after that declaration leaves the table in .rodata and shifts subsequent sections by0xFC0; the full SHA rejects that trial. The correct first-declaration placement produces the exact original section, with no relocations or functions.

The normal build exits0 and the frozen main.dol verifier passes after selecting this typed table from source. No original hashes changed and no executable bytes, object patches or fake function were introduced.
