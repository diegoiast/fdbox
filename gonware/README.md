# fdbox on GonWare

The `gonware` branch ports fdbox to GDOS, the start-up environment of
[GonWare](https://gitea.disconnected-by-peer.at/GonSoft/GonWare), where it is
the embedded console `GCOM.EXE` (and later, the same source dynamically
linked, `COMMAND.EXE`).

- Platform `__GONWARE__`: the POSIX code paths of fdbox on GonWare's C
  library (mlibc), but DOS conventions: `\` separates directories, switches
  start with `/`, `*.*` matches all files.
- `src/lib/gonware/glob.c`: glob() with DOS semantics (no case, `*.*` also
  matches names without an extension, `\` is no escape character).
- Programs start through the GDOS system call `GDOS_SPAWN` (there is no
  fork), found in the current directory and in `PATH`, `.EXE` added.
- `D:` changes the drive, `DIR D:` and `DIR NAME` list as in DOS.
- Batch files (`src/dos/batch.c`, for every platform): `.BTM` and `.BAT` with
  `%0`-`%9`, `%VAR%`, labels, `@`, `ECHO OFF`, `GOTO`, `SHIFT`, `CALL`, `IF
  ERRORLEVEL` (at least, as in DOS) and the error level of programs.
- `LOAD` loads a kernel module (GLM), as on NetWare.
- `GCOM /C command`, `GCOM FILE.BTM`, and at the prompt after
  `\GONWARE\AUTOEXEC.BTM` of the system volume.

Build: `gonware/build.sh <GonWare>/tools/gdos-cc.sh <arch> <sysroot> GCOM.EXE`
(GonWare's build does this itself).
