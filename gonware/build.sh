#!/bin/sh
# This file is part of fdbox
# For license - read license.txt
#
# build.sh <gdos-cc> <arch> <sysroot> <output.EXE> [source.c ...] [cflags]
#
# Builds fdbox for GonWare (GDOS) as GCOM.EXE with GonWare's tools/gdos-cc.sh
# against its C library (mlibc): the POSIX code of fdbox with DOS
# conventions (__GONWARE__), the DOS glob of src/lib/gonware.
# -DGONWARE_COMMAND gives COMMAND.EXE, the shell of an installed system: as
# the first shell (/P) it frees GDOS's boot archive before AUTOEXEC.BTM.
# The translations: GonWare passes its libintl and the catalogues (po/)
# as C sources, with -DFDBOX_NLS and its include directory.

set -eu
cc=$1 arch=$2 sysroot=$3 output=$4
shift 4
extra=""
while [ $# -gt 0 ]; do
	case $1 in
	*.c) extra="$extra $1" ;;
	*) break ;;
	esac
	shift
done
cd "$(dirname "$0")/.."

sh "$cc" "$arch" "$sysroot" "$output" \
	src/applets.c \
	src/dos/batch.c \
	src/dos/beep.c \
	src/dos/call.c \
	src/dos/cd.c \
	src/dos/chcp.c \
	src/dos/cls.c \
	src/dos/command.c \
	src/dos/copymove.c \
	src/dos/datetime.c \
	src/dos/del.c \
	src/dos/dir.c \
	src/dos/echo.c \
	src/dos/exit.c \
	src/dos/for.c \
	src/dos/goto.c \
	src/dos/history.c \
	src/dos/keyb.c \
	src/dos/if.c \
	src/dos/load.c \
	src/dos/md.c \
	src/dos/path.c \
	src/dos/prompt.c \
	src/dos/reboot.c \
	src/dos/rem.c \
	src/dos/set.c \
	src/dos/shift.c \
	src/dos/type.c \
	src/dos/ver.c \
	src/help.c \
	src/lib/applet.c \
	src/lib/args.c \
	src/lib/environ.c \
	src/lib/readline.c \
	src/lib/strextra.c \
	src/main.c \
	src/unix/cal.c \
	src/unix/head.c \
	src/unix/hexdump.c \
	src/unix/tail.c \
	src/lib/gonware/glob.c \
	$extra \
	-- -Isrc/lib/gonware -Isrc -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L -D__GONWARE__ \
	-Wno-unused-parameter -Wno-unused-but-set-variable -Wno-return-type -Wno-sign-compare \
	-Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-function "$@"
