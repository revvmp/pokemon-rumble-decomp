# BTE source provenance for issue 510

The reconstruction in `src/RVL_SDK/bte/bte_hcisu.c`, `bte_init.c`, `bte_logmsg.c`, `bte_main.c`, and `include/RVL_SDK/bte/transport.h` adapts reference code and names from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`. Changes simplify dependencies, remove source comments, use explicit primitive types, and reconstruct the target variadic ABI. The source files are modified from the reference.

Petari's root license is CC0 1.0; the BTE reference carries more specific notices. The following copyright and license notices apply to the referenced material:

- `bte_init.c`: Copyright (C) 2000-2012 Broadcom Corporation.
- `bte_logmsg.c`: Copyright (C) 2001-2012 Broadcom Corporation.
- `bte_main.c`: Copyright (C) 2009-2012 Broadcom Corporation.
- `hci.h`: Copyright (C) 2009-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE follows.

---

This NOTICE is for informational purposes only. All files are still bound by any
licenses they contain, and anything in this file does not change that.

---

This decompilation of the BTE library contains code from bluedroid
<https://android.googlesource.com/platform/external/bluetooth/bluedroid>,
commits 5738f83 and ead3cde, and references code from esp-idf
<https://github.com/espressif/esp-idf> (no specific commit here. whatever main
is at, I guess. I had already decompiled the relevant code by then; I just used
it for naming things).

---

This is a decompilation of a library, *not* a port of one. It is drastically
different from the original code, both in style and organization. The main focus
here is trying to recreate old code, document bugs, and note curiosities, *not*
to retrofit a highly configurable library onto its distant ancestor's shadow
with minimal changes.

One of the major changes I made was hardcoding settings of and removing most
other aspects of the configuration system. Many things previously in uncompiled
blocks are now simply absent; they might have been there in the original, but
they didn't get compiled here and so don't need to be here. Other changes
include completely replacing the logging macro interface, moving many things
around between headers, in between headers and source files sometimes, adding
new things, deleting old things (or just not including them), changing macro
enums to language enums, adding more typedefs so it's more easily recognizable
what things mean, and also the general style and formatting of everything.

tl;dr this code does not look like bluedroid i know that it does not look like
bluedroid do not complain to me that it does not look like bluedroid

- muff1nOS

## Issue 511 additions

`src/RVL_SDK/bte/bta_sys_conn.c` and `include/RVL_SDK/bte/sys.h` adapt Petari `bta_sys_conn.c`, `bta_sys_int.h`, `bta_sys.h` and `ptim.h` at the same commit. These reference files carry Copyright (C) 2003-2012 Broadcom Corporation, Apache-2.0, and changes by muff1n1634. Changes remove comments, simplify dependencies and primitive type aliases, and retain only target-linked functions. The complete Apache-2.0 text linked above applies to these modifications.
