# Pokémon Rumble decompilation

A source reconstruction of Pokémon Rumble (WiiWare, USA, `WPSE`) that rebuilds the game's `main.dol` byte for byte.

![Code progress](images/progress-code.svg)

All 13,462 CPU functions match, all 897 units link from source, and the embedded DSP/controller programs build from symbolic source. Two clean builds reproduce the original main.dol exactly.

## Setup

You need Windows 10 or 11 and access to this private repository. The original game files are already in `orig/`, so you do not need to provide them.

1. Install [Git](https://git-scm.com/download/win).
2. Install [Python](https://www.python.org/downloads/) 3.10 or newer. On the first installer screen, tick **Add python.exe to PATH**.
3. Open a terminal and clone the repository:

   ```
   git clone https://github.com/revvmp/pokemon-rumble-decomp.git
   cd pokemon-rumble-decomp
   ```

4. Run the build in terminal (or double click the `build.bat` file):

   ```
   .\build.bat
   ```

   The first run downloads the pinned compilers and tools into `tools/local/` and checks each one's digest. It then builds the game and compares the result with the original.
5. The last line should read `Build succeeded: build\wpse-v1\main.dol matches the original.`.
