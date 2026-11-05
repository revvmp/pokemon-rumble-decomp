# BTE device manager API source provenance for issue 514

`src/RVL_SDK/bte/bta_dm_api.c` and `include/RVL_SDK/bte/dm_api.h` adapt names, algorithms and interfaces from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, specifically `src/RVL_SDK/bte/bta_dm_api.c`, `bta_dm_int.h` and `bta_api.h`. These files carry Copyright (C) 2003-2012 Broadcom Corporation and include changes by muff1n1634. They are licensed under Apache License 2.0. The complete license is [preserved here](510-apache-2.0.txt).

The files have been modified: dependencies are narrowed, comments are moved to documentation, primitive types use the existing revolution types, target-absent APIs are omitted, and the initializer remains unimplemented pending control-block layout ownership. The target has eleven functions in this unit, ten implemented here.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The original BTE NOTICE follows.

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

## Completion additions

The completed API and new `src/RVL_SDK/bte/bta_dm_main.c` and `src/RVL_SDK/bte/bta_dm_pm.c` adapt the same pinned Petari commit and corresponding source paths, each Copyright (C) 2003-2012 Broadcom Corporation with changes by muff1n1634, under the full Apache-2.0 license linked above. The prior partial-implementation description is historical: the initializer is now implemented, canonical headers use the SDK aliases, and five unlinked public APIs remain in source. Main and power source retain the reference state tables and helpers; logging and dependencies use project interfaces and comments are recorded in documentation.
