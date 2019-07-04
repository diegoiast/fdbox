#! /bin/sh

# set -x
set -e

create_header() {
    local NAME=$1
    cat << EOF
#ifndef __${NAME}_h__
#define __${NAME}_h__

// This file is part of fdbox
// For license - read license.txt

int command_${NAME}(int arc, char* argv[]);
const char* help_${NAME}();

#endif //__${NAME}_h__
EOF
}

create_file_content() {
    local NAME=$1
    cat << EOF
#include <stdlib.h>
#include <stdio.h>

#include "fdbox.h"
#include "${NAME}.h"

// This file is part of fdbox
// For license - read license.txt

int command_${NAME}(int argc, char* argv[]) {
    printf("${NAME} - TODO: Unimplemented yet\n");
    return EXIT_FAILURE;
}

const char* help_${NAME}() {
    return "Here should be a basic help for ${NAME}";
}
EOF
}

configure_file() {
    i=$1
    
    DIR=`echo $i | cut -f 1 -d/`
    NAME=`echo $i | cut -f 2 -d/ | cut -f 1 -d.`
    INC_FILE="$DIR/${NAME}.h"
    
    OBJECTS="${OBJECTS} ${NAME}.o"
    APPLETS="${APPLETS} ${NAME}"
    INCLUDES="${INCLUDES} $INC_FILE"

    echo 
    echo "${NAME}.o: $i $INC_FILE fdbox.h"
    echo "\t\$(CC) \$(CCFLAGS) -c $< -o \$@"
}

create_applets() {
    echo "/* auto generated file */"
    echo '#include <stddef.h>'
    echo "#include \"fdbox.h\""
    echo 

    for i in $INCLUDES; do
        echo "#include \"$i\""
    done
    echo
    echo 'struct applet commands[] = {'
    for i in $APPLETS; do
        echo "    { &help_${i},  &command_${i}, \"$i\" },"
    done
    echo "    { NULL, NULL}"
    echo "};"
}

create_target() {
    local FILE_NAME=$1
    local EXTRA=$2
    echo "$FILE_NAME.o: $FILE_NAME.c fdbox.h $EXTRA"
    echo "\t\$(CC) \$(CCFLAGS) -c $< -o \$@"
    echo ""
}

MAKEFILE="Makefile"
OBJECTS=""
INCLUDES=""
APPLETS=""
LIB_SRC=`echo lib/*.c`
SOURCES=`echo dos/*.c unix/*.c ./help.c`

rm -f $MAKEFILE
echo "CCFLAGS=-Os -g -Wall -I." >> $MAKEFILE
echo ".PHONY: all clean distclean gitclean test"  >> $MAKEFILE
echo "" >> $MAKEFILE
echo "all: fdbox" >> $MAKEFILE

for i in $SOURCES $LIB_SRC; do 
    configure_file $i >> $MAKEFILE
    if [ ! -s $i ]; then 
        echo "Generating content for $i"
        create_file_content $NAME > $i
    fi
    if [ ! -s $INC_FILE ]; then
        echo "Generating header $INC_FILE"
        create_header $NAME > $INC_FILE
    fi 
done
echo "" >> $MAKEFILE


OBJECTS="${OBJECTS} main.o applets.o"

create_target "tests" "lib/args.h" >> $MAKEFILE
create_target "applets" $INCLUDES >> $MAKEFILE
create_target "main" $INCLUDES >> $MAKEFILE

echo "OBJECTS =${OBJECTS}"  >> $MAKEFILE

echo "" >> $MAKEFILE
echo 'fdbox: $(OBJECTS)' >> $MAKEFILE 
echo '\t$(CC) $(OBJECTS) -o fdbox'   >> $MAKEFILE

echo "" >> $MAKEFILE
echo 'unittest: tests.o' >> $MAKEFILE 
echo '\t$(CC) tests.o args.o -o unittest'   >> $MAKEFILE

echo "" >> $MAKEFILE
echo "test: unittest" >>  $MAKEFILE
echo "\t@./unittest" >>  $MAKEFILE

echo "" >> $MAKEFILE
echo "clean:" >>  $MAKEFILE
echo '\trm -f $(OBJECTS) tests.o unittest fdbox' >>  $MAKEFILE

echo "" >> $MAKEFILE
echo "distclean: clean" >>  $MAKEFILE
echo "\trm -f ${INCLUDES} main.c Makefile" >>  $MAKEFILE

echo "" >> $MAKEFILE
echo "gitclean: clean" >>  $MAKEFILE
echo "\tgit checkout dos/*.c unix/*.c" >>  $MAKEFILE
echo "\trm -f  dos/*.h unix/*.h" >>  $MAKEFILE

create_applets $APPLETS  > applets.c
