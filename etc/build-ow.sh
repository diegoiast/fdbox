#! /bin/sh

set -e
set -x

wget --quiet -nc https://github.com/open-watcom/open-watcom-v2/releases/download/Current-build/ow-snapshot.tar.gz

mkdir -p ow
#tar -C ow -xf ow-snapshot.tar.gz

export WATCOM=$(pwd)/ow
export PATH=$WATCOM/binl:$PATH
export EDPATH=$WATCOM/eddat
export EDPATH=$WATCOM/eddat
export INCLUDE=$WATCOM/h

cd ..
make -f Makefile.ow clean
make -f Makefile.ow -k -j 32