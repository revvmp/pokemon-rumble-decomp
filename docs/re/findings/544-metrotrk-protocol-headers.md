# F-544-1: MetroTRK message protocol interfaces

Issue #544 reconstructs the tail of the #543-owned message-buffer file, the message-handler file, and the console file. This prerequisite extends the canonical MetroTRK headers after their #541 delivery. It changes no existing structure, function declaration, unit selection, split, or expected artifact. The owners of #541 and #542 approved the additions; #543 retains ownership of message-buffer APIs.

## Protocol layouts and signatures

The pinned reference is Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `libs/MetroTRK/include/trk.h` and the MetroTRK portable message-handler, support, and PowerPC target sources. These files carry no file-specific exception to Petari's CC0 license. Names and algorithms derive from that reference; fields with unreadable reference names receive descriptive names below.

`CommandReply` is 64 bytes. Its length word is at zero, command word at four, reply-error word at eight, command parameter at twelve, and 48-byte payload at sixteen. Both command words expose byte and full-word views through `TRKMessageWord`. The parameter has sequence and file-position views: message handlers send the incremented sequence counter, while file requests use a position. This preserves both proven meanings without an offset-based member name. Native handlers initialize exactly 64 bytes and use these offsets; source probes reproduce their complete non-relocation instruction streams.

The protocol enums recover file handles, file I/O results, register classes, stepping modes, reply errors, memory-access options, and the memory-option bit masks. Existing message-command and core error definitions remain unchanged. Event constructor/posting and PowerPC target accessor declarations complete calls used by the new handlers.

The target stop operation uses a signed error result in its caller: `TRKDoStop` compares signed return values against the DS error constants. Declaring the reference's unsigned return changes those comparisons. The canonical interface therefore uses the established `DSError` return type. Its implementation remains owned by the neighboring target unit. The single-step count is a full-width `u32`, as in the reference implementation; callers obtain it from a byte-valued protocol field.

## Validation

Independent scratch compilations with Wii/1.1, the measured MetroTRK flags, and small-data threshold eight reproduce all retained handler and console instruction candidates. Reset and override have identical masked instruction candidates, so their identities additionally require their distinct outgoing reset/vector-copy calls. The connection getter is identified by its connection-state relocation and callers, rather than a two-instruction masked search. Full source acceptance remains a separate delivery.

The proposed message-handler and console splits were separately built from original objects before reconstruction: configure, Ninja, and explicit frozen-manifest verification each exited zero. This header prerequisite contains neither those splits nor reconstructed source and earns no source coverage.

## Traps

The pinned reference's PowerPC target-stop return declaration is unsigned, while the target caller needs signed comparisons. Import declarations only after checking their effect on the actual caller. Matching ABI width alone does not preserve comparison instructions.

The message-buffer diagnostic call omits a condition-register setup, while the message-handler option diagnostics include it. Their target compilation units use different visible reporting declarations. Do not apply a local message-buffer reporting declaration globally to other MetroTRK files.
