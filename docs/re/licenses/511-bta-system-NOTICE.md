# BTE system source provenance for issue 511

The implementations in `src/RVL_SDK/bte/btu_task1.c`, `bd.c`, `bta_sys_main.c` and `ptim.c`, plus additions to `include/revolution/bte/{bd.h,bt_types.h,btu.h,gki.h}`, adapt Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/bte/`. Changes remove comments, use project include paths, simplify trace macro expansions and dependencies, retain target-linked address helpers, and express the timer conversion with its reference constant. The files are modified from the reference.

File-specific notices take precedence over the reference repository's root CC0 license:

- `bd.c`, `bta_sys_main.c`, `ptim.c`, `bd.h`, `bta_sys.h`, `bta_sys_int.h` and `ptim.h`: Copyright (C) 2003-2012 Broadcom Corporation.
- `bt_types.h`, `btu.h`, `gki.h` and `gki_target.h`: Copyright (C) 1999-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt), and the original BTE NOTICE is reproduced in [the issue 510 notice](510-bte-NOTICE.md). The reference `btu_task1.c` has no file-level license notice; Petari's root CC0 applies to that file. Its dependency declarations retain the file-specific notices listed above.
