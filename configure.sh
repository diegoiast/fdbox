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

int command_${NAME}(int arc, char* argv[]) {
    printf("${NAME} - TODO: Unimplemented yet\n");
    return EXIT_FAILURE;
}

const char* help_${NAME}() {
    return "Here should be a basic help for ${NAME}";
}
EOF
}

create_main() {
    echo "#include <stdlib.h>"
    echo "#include <stdio.h>"
    echo "#include <string.h>"
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
    echo "   { NULL, NULL}"
    echo "};"

    cat << EOF

struct applet* find_applet(const char* applet_name);

int main(int argc, char *argv[])
{    
    struct applet *cmd = find_applet(argv[1]);
    if (cmd != NULL) {
        return cmd->handler(argc, argv);
    }
    command_help(argc, argv);
    return EXIT_FAILURE;
}

struct applet* find_applet(const char* applet_name) {
   struct applet *app = commands;
   if (applet_name == NULL) {
       return NULL;
   }   
   while (app != NULL && app->name != NULL) {
      if (strcmp(app->name, applet_name) == 0) {
         return app;
      }
      app ++;
   }
   return NULL;
}
EOF
}

MAKEFILE="Makefile"
OBJECTS=""
INCLUDES=""
APPLETS=""
SOURCES=`echo dos/*.c unix/*.c ./help.c `

rm -f $MAKEFILE
echo "CCFLAGS=-Os -g -Wall -I." >> $MAKEFILE
echo ".PHONY: all clean"  >> $MAKEFILE
echo "" >> $MAKEFILE
echo "all: fdbox" >> $MAKEFILE
echo "" >> $MAKEFILE
echo "" >> $MAKEFILE

for i in $SOURCES; do 
    DIR=`echo $i | cut -f 1 -d/`
    NAME=`echo $i | cut -f 2 -d/ | cut -f 1 -d.`

    INC_FILE="$DIR/${NAME}.h"
    OBJECTS="${OBJECTS} ${NAME}.o"
    APPLETS="${APPLETS} ${NAME}"
    INCLUDES="${INCLUDES} $INC_FILE"

    echo >> $MAKEFILE
    echo "${NAME}.o: $i $INC_FILE fdbox.h " >> $MAKEFILE
    echo  "\t" '$(CC) $(CCFLAGS) -c $< -o $@' >> $MAKEFILE

    if [ ! -s $i ]; then 
        echo "Generating content for $i"
        create_file_content $NAME > $i
    fi
    if [ ! -s $INC_FILE ]; then
        echo "Generating header $INC_FILE"
        create_header $NAME > $INC_FILE
    fi 
done;


echo "main.o: main.c fdbox.h $INCLUDES" >> $MAKEFILE
echo  "\t" '$(CC) $(CCFLAGS) -c $< -o $@' >> $MAKEFILE
OBJECTS="${OBJECTS} main.o"

echo "OBJECTS =${OBJECTS}"  >> $MAKEFILE
echo "" >> $MAKEFILE

echo 'fdbox: $(OBJECTS)' >> $MAKEFILE 
echo '\t$(CC) $(OBJECTS) -o fdbox'   >> $MAKEFILE
echo "" >> $MAKEFILE
echo "clean:" >>  $MAKEFILE
echo '\trm -f $(OBJECTS) fdbox' >>  $MAKEFILE

echo "distclean: clean" >>  $MAKEFILE
echo "\trm -f ${INCLUDES} main.c Makefile" >>  $MAKEFILE

echo "gitclean: clean" >>  $MAKEFILE
echo "\tgit checkout dos/*.c unix/*.c " >>  $MAKEFILE
echo "\trm -f  dos/*.h unix/*.h main.c" >>  $MAKEFILE

create_main $APPLETS  > main.c
