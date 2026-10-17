# BTE source provenance for issue 509

The reconstruction in `src/RVL_SDK/bte/hcisu_h2.c` adapts reference code, names and the receive state machine from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (`src/RVL_SDK/bte/hcisu_h2.c`). Changes remove source comments, give the control block descriptive field names and enumerated states, replace the stream macro with an explicit expression, and rename include paths. The file is modified from the reference.

The referenced file carries these notices:

- `hcisu_h2.c` (bluedroid `hci/src/hci_h4.c`): Copyright (C) 2009-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE is reproduced in [the issue 510 notice](510-bte-NOTICE.md).

`src/RVL_SDK/bte/uusb_ppc.c` and `include/revolution/bte/uusb.h` follow Petari's `uusb_ppc.c` and `uusb.h` at the same commit. Those files carry no file-level notice, so Petari's root CC0 dedication applies.
