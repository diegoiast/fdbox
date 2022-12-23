#! /bin/bash

set -x
set -e
MAIN=dosbox
DOWNLOAD_DIR=zip
UNZIP=7z

# TurboC can be downloaded from here, but from argive.org its easier
# https://edn.embarcadero.com/jp/article/20841

# PacificC is also archives at two dirrect times:
# https://web.archive.org/web/19980524231436/http://www1.htsoft.com/files/pacific/pacific.exe
# https://web.archive.org/web/20000819032652/http://www1.htsoft.com:80/files/pacific/pacific.exe

# also worth checking:
# http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/devel/c/cc386/420/
# http://www.desmet-c.com/
# http://ladsoft.tripod.com/orange_c_compiler.html
# http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/devel/c/cc386/420/

# nice docs:
# http://bitsavers.informatik.uni-stuttgart.de/pdf/borland/turbo_c/Turbo_C_Users_Guide_1987.pdf

ONLINE_ZIPS='
http://ndn.muxe.com/download/file/ndn_3_00_0005_d32.zip
https://github.com/downloads/AnimatorPro/Animator-Pro/AnimatorAKA.zip
https://archive.org/download/msdos_borland_turbo_c_2.01/BorlandTurboC201-megapack.zip
https://ia902905.us.archive.org/34/items/4dos8/4dos800.zip
https://web.archive.org/web/19980524231436if_/http://www.htsoft.com:80/files/pacific/pacific.exe
https://github.com/open-watcom/open-watcom-v2/releases/download/Current-build/open-watcom-2_0-f77-dos.exe
'

# Choose the zip picker, been doing this since 1998. Crap.
DJGPP_BASE="http://www.delorie.com/pub/djgpp/current"
# DJGPP_BASE="ftp://ftp.fu-berlin.de/pc/languages/djgpp/current"
# DJGPP_BASE="http://djgpp.mirror.garr.it/current"

DJGPP_FILES="
$DJGPP_BASE/v2/copying.dj
$DJGPP_BASE/v2/djdev205.zip
$DJGPP_BASE/v2/faq230b.zip
$DJGPP_BASE/v2/pakk023b.zip
$DJGPP_BASE/v2apps/rhid15ab.zip
$DJGPP_BASE/v2gnu/bnu2351b.zip
$DJGPP_BASE/v2gnu/gcc1030b.zip
$DJGPP_BASE/v2gnu/gdb801b.zip
$DJGPP_BASE/v2gnu/gpp1030b.zip
$DJGPP_BASE/v2gnu/mak43br2.zip
$DJGPP_BASE/v2misc/csdpmi7b.zip
$DJGPP_BASE/v2tk/allegro/all422ar2.zip
$DJGPP_BASE/v2tk/allegro/all422br2.zip
$DJGPP_BASE/v2tk/grx249s.zip
$DJGPP_BASE/v2tk/pdcur39a.zip
"

# still have no idea how to use this :)
GCC_IA16_FILES='
https://github.com/tkchia/build-ia16/releases/download/20200321/i16butil.zip
https://github.com/tkchia/build-ia16/releases/download/20200321/i16gcc.zip
https://github.com/tkchia/build-ia16/releases/download/20200321/i16newli-fixed-20200503.zip
https://github.com/tkchia/build-ia16/releases/download/20200321/i16budoc.zip
https://github.com/tkchia/build-ia16/releases/download/20200321/i16gcdoc.zip
https://github.com/tkchia/build-ia16/releases/download/20191010/i16elklc.zip
'

# this is nice - but I am having bash problems, and unsure how safe it is to use
# https://winworldpc.com/product/borland-turbo-c/1x
TC1_FILE='http://julian.winworldpc.com/Abandonware Applications/PC/Borland Turbo C++ 1.01 (3.5).7z'
TC1_FILE='https://dl-alt1.winworldpc.com/Abandonware%20Applications/PC/Borland%20Turbo%20C%202.01%20(3.5).7z'

# https://winworldpc.com/product/borland-turbo-c/2x
TC2_FILE='https://dl.winworldpc.com/Borland%20Turbo%20C%202.01%20(1989)%20(3.5-720k).7z'

# https://winworldpc.com/product/turbo-c/3x
TC30_FILE='http://julian.winworldpc.com/Borland Turbo CPP 3.0 (5.25-1.2mb).7z'
TC30_FILE='https://dl.winworldpc.com/Borland%20Turbo%20CPP%203.0%20(5.25-1.2mb).7z'
TC30_FILE='https://dl-alt1.winworldpc.com/Borland%20Turbo%20CPP%203.0%20(5.25-1.2mb).7z'

FREEDOS="
https://github.com/FDOS/freecom/releases/download/com084pre7/com084b7-xmsswap.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/append/appe506x.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/xcopy/xwcpy081.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/attrib/attrib121.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/deltree/deltree102g.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/mem/1.11/mem1_11.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/more/4.3/more43.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/type/type20.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/tree/3.7/tree372x.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/edit/0.9/edit09ax.zip
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/dos/label/1.4/label14.zip
"

download_file_file()
{
    local DOWNLOAD_DIR=$1
    local FILE_URL=$2
#    wget -q -nc -P $FILE
#    curl -L --silent --remote-name --output-dir $DOWNLOAD_DIR $FILE_URL
    curl -L -C - --remote-name --output-dir $DOWNLOAD_DIR $FILE_URL
}

function download_files()
{
    local FILES=$2
    local DOWNLOAD_DIR=$3
    local EXTRACT_DIR=$4
    local UNZIP="$1"
    
    mkdir -p $DOWNLOAD_DIR
    mkdir -p $EXTRACT_DIR

    for i in $FILES; do
        download_file_file $DOWNLOAD_DIR $i
    done

    for i in `find $DOWNLOAD_DIR/*.7z $DOWNLOAD_DIR/*.zip`; do
        # TODO - copy files, instead of extracting
        # gcc ia16 are not dos compatible, are compressed using 7z the following
        # command does not work for them:
        # unzip -qxu $i -d $EXTRACT_DIR
        "$UNZIP" x -y -O$EXTRACT_DIR "$i" > /dev/null
    done
}


if [ "$OSTYPE" = "msys" ]; then
    UNZIP='/c/Program Files/7-Zip/7z.exe'
fi

rm -fr $MAIN

download_files "$UNZIP" "$ONLINE_ZIPS"    $DOWNLOAD_DIR         $MAIN 
download_files "$UNZIP" "$GCC_IA16_FILES" $DOWNLOAD_DIR/GCC     $MAIN/GCC 
download_files "$UNZIP" "$DJGPP_FILES"    $DOWNLOAD_DIR/DJGPP   $MAIN/DJGPP 
download_files "$UNZIP" "$FREEDOS"        $DOWNLOAD_DIR/FREEDOS $MAIN/FREEDOS 
# download_files "$UNZIP"  "$TC1_FILE"      $DOWNLOAD_DIR/TC1     $MAIN/TC1 
download_files "$UNZIP" "$TC2_FILE"      $DOWNLOAD_DIR/TC2     $MAIN/TC2 
download_files "$UNZIP" "$TC30_FILE"     $DOWNLOAD_DIR/TC3     $MAIN/TC3 

# special unzip - this is an EXE, not handled by the function
"$UNZIP" x -y $DOWNLOAD_DIR/pacific.exe -o$MAIN/pacific/ > /dev/null
"$UNZIP" x -y $DOWNLOAD_DIR/open-watcom-2_0-f77-dos.exe  -o$MAIN/ow/ > /dev/null

# minor cleanups
rm -fr $MAIN/__MACOSX
rm -f  $MAIN/autoexec.bat
rm -f  $MAIN/RUNHACK.BAT   $MAIN/RUNHACK.EXE
mv $MAIN/ndn_3_00_0005_d32 $MAIN/NDN
mv $MAIN/SAMPLES/          $MAIN/TC/SAMPLES
mv $MAIN/AnimatorAKA       $MAIN/AAT

set +e
mkdir -p $MAIN/FREEDOS/BIN/
mv $MAIN/FREEDOS/bin/*     $MAIN/FREEDOS/BIN/  || true
rm -fr $MAIN/FREEDOS/bin/
mv $MAIN/FREEDOS/doc/*     $MAIN/FREEDOS/DOC/
rm -fr $MAIN/FREEDOS/doc/
mv $MAIN/FREEDOS/help/*     $MAIN/FREEDOS/HELP/
rm -fr $MAIN/FREEDOS/help/
mv $MAIN/FREEDOS/nls/*     $MAIN/FREEDOS/NLS/
rm -fr $MAIN/FREEDOS/nls/
mv $MAIN/FREEDOS/*.exe     $MAIN/FREEDOS/BIN/ || true
mv $MAIN/FREEDOS/*.com     $MAIN/FREEDOS/BIN/ || true
mv $MAIN/FREEDOS/*.EXE     $MAIN/FREEDOS/BIN/ || true
mv $MAIN/FREEDOS/*.COM     $MAIN/FREEDOS/BIN/ || true

cp autoexec.bat            $MAIN/AUTOEXEC.BAT
