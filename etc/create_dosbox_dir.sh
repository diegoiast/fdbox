#! /bin/sh

set -x
set -e
MAIN=dosbox
DOWNLOAD_DIR=zip

# TurboC can be downloaded from here, but from argive.org its easier
# https://edn.embarcadero.com/jp/article/20841

# PacificC is also archives at two dirrect times:
# https://web.archive.org/web/19980524231436/http://www1.htsoft.com/files/pacific/pacific.exe
# https://web.archive.org/web/20000819032652/http://www1.htsoft.com:80/files/pacific/pacific.exe


ONLINE_ZIPS='
http://ndn.muxe.com/download/file/ndn_3_00_0005_d32.zip
https://github.com/downloads/AnimatorPro/Animator-Pro/AnimatorAKA.zip
https://archive.org/download/msdos_borland_turbo_c_2.01/BorlandTurboC201-megapack.zip
https://ia902905.us.archive.org/34/items/4dos8/4dos800.zip
https://web.archive.org/web/19980524231436if_/http://www.htsoft.com:80/files/pacific/pacific.exe
'

# Choose the zip picker, been doing this since 1998. Crap.
DJGPP_FILES='
http://www.delorie.com/pub/djgpp/current/v2/copying.dj
http://www.delorie.com/pub/djgpp/current/v2/djdev205.zip
http://www.delorie.com/pub/djgpp/current/v2/faq230b.zip
http://www.delorie.com/pub/djgpp/current/v2/pakk023b.zip
http://www.delorie.com/pub/djgpp/current/v2apps/rhid15ab.zip
http://www.delorie.com/pub/djgpp/current/v2gnu/bnu234b.zip
http://www.delorie.com/pub/djgpp/current/v2gnu/gcc930b.zip
http://www.delorie.com/pub/djgpp/current/v2gnu/gdb801b.zip
http://www.delorie.com/pub/djgpp/current/v2gnu/gpp930b.zip
http://www.delorie.com/pub/djgpp/current/v2gnu/mak43b.zip
http://www.delorie.com/pub/djgpp/current/v2misc/csdpmi7b.zip
http://www.delorie.com/pub/djgpp/current/v2tk/allegro/all422ar2.zip
http://www.delorie.com/pub/djgpp/current/v2tk/allegro/all422br2.zip
http://www.delorie.com/pub/djgpp/current/v2tk/grx249s.zip
http://www.delorie.com/pub/djgpp/current/v2tk/pdcur39a.zip
'

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
# https://winworldpc.com/download/c28d2431-c28d-5737-11c3-a7c29d255254
TC1_FILE='http://julian.winworldpc.com/Abandonware Applications/PC/Borland Turbo C++ 1.01 (3.5).7z'

# https://winworldpc.com/download/c28d2431-c28d-5737-11c3-a7c29d255254
TC30_FILE='http://julian.winworldpc.com/Borland Turbo CPP 3.0 (5.25-1.2mb).7z'


function download_files()
{
    local FILES=$1
    local DOWNLOAD_DIR=$2
    local EXTRACT_DIR=$3

    mkdir -p $DOWNLOAD_DIR
    mkdir -p $EXTRACT_DIR

    for i in $FILES; do
        wget -q -nc -P $DOWNLOAD_DIR $i
    done

    for i in $DOWNLOAD_DIR/*.zip; do
        # TODO - copy files, instead of extracting
        # gcc ia16 are not dos compatible, are compressed using 7z the following
        # command does not work for them:
        # unzip -qxu $i -d $EXTRACT_DIR
        7z x -y -O$EXTRACT_DIR $i > /dev/null
    done
}

rm -fr $MAIN

download_files "$ONLINE_ZIPS"    $DOWNLOAD_DIR       $MAIN
download_files "$GCC_IA16_FILES" $DOWNLOAD_DIR/GCC   $MAIN/GCC
download_files "$DJGPP_FILES"    $DOWNLOAD_DIR/DJGPP $MAIN/DJGPP
#download_files "$TC1_FILE"       $DOWNLOAD_DIR/TC1   $MAIN/TC1
#download_files "$TC30_FILE"      $DOWNLOAD_DIR/TC1   $MAIN/TC3

# special unzip - this is an EXE, not handled by the function
7z x -y $DOWNLOAD_DIR/pacific.exe -o$MAIN/pacific/ > /dev/null

# minor cleanups
rm -fr $MAIN/__MACOSX
rm -f  $MAIN/autoexec.bat
rm -f  $MAIN/RUNHACK.BAT   $MAIN/RUNHACK.EXE
mv $MAIN/ndn_3_00_0005_d32 $MAIN/NDN
mv $MAIN/SAMPLES/          $MAIN/TC/SAMPLES
mv $MAIN/AnimatorAKA       $MAIN/ATA
cp autoexec.bat            $MAIN/AUTOEXEC.BAT