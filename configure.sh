#! /bin/sh

# set -x
set -e

function create_header() {
    local NAME=$1
    cat << EOF
#ifndef __${NAME}_h__
#define __${NAME}_h__

int command_${NAME}(int arc, char* argv[]);

#endif //__${NAME}_h__
EOF
}

function create_file_content() {
    local NAME=$1
    cat << EOF
#include <stdlib.h>
#include <stdio.h>
#include "${NAME}.h"

int command_${NAME}(int arc, char* argv[]) {
    return EXIT_FAILURE;
}
EOF
}

function create_main() {
    echo "#include <stdlib.h>"
    echo "#include <stdio.h>"
    echo "#include <strings.h>"
    
    echo 

    for i in $INCLUDES; do
        echo "#include \"$i\""
    done

    echo 
    echo "typedef int (*functionHandler)(int, char*[]);"
    echo 
    echo "struct applet {"
    echo "  const char* name;"
    echo "  functionHandler handler;"
    echo "};"
    echo
    echo 'struct applet commands[] = {'
    for i in $APPLETS; do
        echo "    { \"$i\", &command_${i} },"
    done
    echo "   { NULL, NULL}"
    echo "};"

    CAT << EOF

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
SOURCES=`echo ./help.c dos/*.c unix/*.c `

rm -f $MAKEFILE
echo ".PHONY: all clean"  >> $MAKEFILE
echo "" >> $MAKEFILE
echo "all: fdbox" >> $MAKEFILE
echo "" >> $MAKEFILE
echo "" >> $MAKEFILE

for i in $SOURCES; do 
    DIR=`echo $i | cut -f 1 -d/`
    NAME=`echo $i | cut -f 2 -d/ | cut -f 1 -d.`
    echo "${NAME}.o: $i" >> $MAKEFILE
    echo  "\t" '$(CC) $(CCFLAGS) -c $< -o $@' >> $MAKEFILE

    echo >> $MAKEFILE
    INC_FILE="$DIR/${NAME}.h"
    OBJECTS="${OBJECTS} ${NAME}.o"
    APPLETS="${APPLETS} ${NAME}"
    INCLUDES="${INCLUDES} $INC_FILE"

    if [ ! -s $INC_FILE ]; then 
        create_header $NAME > $INC_FILE
    fi 

    if [ ! -s $i ]; then 
        create_file_content $NAME > $i
    fi
done;


echo "main.o: main.c" >> $MAKEFILE
echo  "\t" '$(CC) $(CCFLAGS) -c $< -o $@' >> $MAKEFILE
OBJECTS="${OBJECTS} main.o"

echo "OBJECTS =${OBJECTS}"  >> $MAKEFILE
echo "" >> $MAKEFILE

echo 'fdbox: $(OBJECTS)' >> $MAKEFILE 
echo '\t$(CC) $(OBJECTS) -o fdbox'   >> $MAKEFILE
echo "" >> $MAKEFILE
echo "clean:" >>  $MAKEFILE
echo '\trm -f $(OBJECTS) fdbox' >>  $MAKEFILE

echo "dist-clean: clean" >>  $MAKEFILE
echo "\trm -f ${INCLUDES} main.c Makefile" >>  $MAKEFILE

create_main $APPLETS  > main.c
