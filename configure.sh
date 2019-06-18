#! /bin/sh

# set -x
set -e


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

    echo "#ifndef __${NAME}_h__" >> $INC_FILE
    echo "#define __${NAME}_h__" >> $INC_FILE
    echo "int command_${NAME}(int arc, char* argv[]);" >> $INC_FILE
    echo "#endif //__${NAME}_h__" >> $INC_FILE

    touch $INC_FILE
    echo "#include <stdlib.h> " > $i
    echo "#include <stdio.h> " >> $i
    echo "#include \"${NAME}.h\" " >> $i
    echo "" >> $i
    echo "int command_${NAME}(int arc, char* argv[]) {" >> $i
    echo "  return EXIT_FAILURE;" >> $i    
    echo "}" >> $i
done;


echo "main.o: main.c" >> $MAKEFILE
echo  "\t" '$(CC) $(CCFLAGS) -c $< -o $@' >> $MAKEFILE
OBJECTS="${OBJECTS} main.o"

echo "OBJECTS =${OBJECTS}"  >> $MAKEFILE
echo "" >> $MAKEFILE

echo 'fdbox: $(OBJECTS)' >> $MAKEFILE 
echo '\t$(CC) $(OBJECTS) -o fbbox'   >> $MAKEFILE
echo "" >> $MAKEFILE
echo "clean:" >>  $MAKEFILE
echo '\trm -f $(OBJECTS) fdbox' >>  $MAKEFILE

# create main

create_main $APPLETS  > main.c
