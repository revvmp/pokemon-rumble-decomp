# F-361-1: Native FA file unit and original names

## Scope and ownership

The original #361 inventory has25 functions and9212 code bytes from8020F880 through80211D04. They are the middle of pf_file.c. The first native retained function in that file is PFFILE_Cursor_AdvanceToRead at8020D930, belonging to unclaimed #360. No split, object registration or canonical file-header takeover is made by this checkpoint. The final three file functions at80211D10/11E10/11F10 belong to #362 and are respectively cinsert, cdelete and flock.

## Evidence and references

The pinned CC0 IPL map/splits at f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c places the FA file unit at815D8378–815DC914. Thirteen consecutive retained function sizes starting with Cursor_AdvanceToRead identify the native unit beginning8020D930. The original sequence places p_fappend at8020F880, p_finfo atF960, p_combine atFA80, p_divide atFD00, p_cinsert at210040, p_cdelete at2101F0 and p_flock at210390. Native11F10 independently takes a file pointer and mode and calls10390, confirming flock rather than the earlier tentative combine label for that final function.

A related primary source is the CC0 TFP2 revision b62dfe78759133a85bf5ce451bcf496b87439b0c. Its src/vf/fatfs/pf_file.c has SHA2560ce79a3e5537b4732e0b9e7d831af2510b320f9b3fd6b13c530b4e15da5e68b3. Its file header has SHA256ba14d549f93d0259f50c6f680e5067c097cf5dad45eba701230fe5e8db709f83. Root LICENSE has SHA25636ffd9dc085d529a7e60e1276d73ae5a030b020313e6c5408593a6ae2af39673 and no file-specific source notice overrides it. This is a related VF revision; FA-specific append, combine, divide and lock behavior still requires native evidence. No whole-source match is claimed.

## Validation state

No split or source-selected build has been run for this checkpoint. Established matching neighboring FA units use Wii/1.0 with cflags_sdk; any file-source hypothesis must pass complete native code, independently paired targets, allocated data, existing-consumer preservation and full frozen-artifact validation before acceptance.

## Traps

- A block issue can begin deep inside a source unit. The file unit starts in #360; claiming #361 does not authorize taking its split or canonical header.
- A related VF source omits FA-only operations. Preserve original FA map order and independently inspect native callees rather than shifting names through missing entries.
- The final public file function is flock, not combine. Its two arguments and call to the private lock operation independently confirm the pinned original identity.
