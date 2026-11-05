# BTE source provenance for issue 508

The reconstructions in `src/RVL_SDK/bte/gki_buffer.c`, `src/RVL_SDK/bte/gki_time.c` and `src/RVL_SDK/bte/gki_ppc.c`, and the declarations in `include/revolution/bte/gki_int.h`, adapt reference code from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (`src/RVL_SDK/bte/gki_buffer.c`, `gki_time.c`, `gki_ppc.c`, `gki_int.h`, `gki_common.h` and `gki_target.h`). The changes:

- Source comments are removed.
- Functions absent from this target are dropped, and the asserts are removed from `GKI_enable` and `GKI_disable`.
- The target configuration is folded into one internal header.
- Include paths are renamed.

The files are modified from the reference.

The referenced files carry these notices:

- `gki_buffer.c`, `gki_time.c` and `gki_common.h` (bluedroid `gki/common`), `gki_int.h` (bluedroid `gki/ulinux`) and `gki_target.h` (bluedroid `include`): Copyright (C) 1999-2012 Broadcom Corporation.
- `gki_ppc.c` (bluedroid `gki/ulinux/gki_ulinux.c`): Copyright (C) 2009-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE is reproduced in [the issue 510 notice](510-bte-NOTICE.md).
