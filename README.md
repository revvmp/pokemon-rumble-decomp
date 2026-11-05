# Pokémon Rumble decompilation

A source reconstruction of Pokémon Rumble (WiiWare, USA, `WPSE`) that rebuilds the game's `main.dol` byte for byte.

![Code progress](images/progress-code.svg)

All 13,462 CPU functions match, all 897 units link from source, and the embedded DSP/controller programs build from symbolic source. Embedded message assets are extracted locally from the user-supplied original executable.

# Build Instructions

Original game files are not included and therefore you must own your own copy of the game. Supply your own **USA WiiWare release (`WPSE`), title version 1** executable.

1. Install [Git](https://git-scm.com/download/win).
2. Install [Python](https://www.python.org/downloads/) 3.10 or newer. On the first installer screen, tick **Add python.exe to PATH**.
3. Open a terminal and clone the repository:

   ```
   git clone https://github.com/revvmp/pokemon-rumble-decomp.git
   cd pokemon-rumble-decomp
   ```

4. Place your extracted, decompressed game executable at this exact path (do not rename the WAD itself to `main.dol`):

   ```text
   orig/
     .gitkeep
     wpse-v1/
       .gitkeep
       main.dol    <-- supply this file yourself
   ```

   The required file is **4,094,880 bytes**, with SHA-256:

   ```text
   ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da
   ```

   If starting from your own WAD, first use a Wii WAD extraction tool to extract and decrypt its contents into a directory outside this repository. The main executable is the NLZSS-compressed content `00000001.app`. After installing Python, download the pinned build tools and decompress that content with:

   ```powershell
   python tools/setup/acquire_tools.py
   .\tools\local\dtk.exe nlzss decompress -o orig/wpse-v1/main.dol "C:\path\to\decrypted\00000001.app"
   ```

   You can check the supplied executable before building:

   ```powershell
   python tools/setup/check_reference.py
   ```

   Only `main.dol` is required. Do not copy the WAD, boot executable, tickets, certificates, or other game contents into the repository. The build extracts its embedded message archive into `build/` automatically; no separate textures or JSON assets are needed. Files you place under `orig/` are ignored by Git except for `.gitkeep` placeholders.

5. Run the build in terminal (or double click the `build.bat` file):

   ```
   .\build.bat
   ```

6. The last line should read `Build succeeded: build\wpse-v1\main.dol matches the original.`.

The build verifies `build/wpse-v1/main.dol` against your supplied original. It does not build a complete WAD or include the external game assets needed to run the game.
