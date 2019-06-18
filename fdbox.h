#ifndef __fdbox_h__
#define __fdbox_h__

// https://stackoverflow.com/a/5459929/78712
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


typedef int (*function_handler)(int, char*[]);
typedef const char* (*function_help)();

struct applet { 
  function_help help_text;
  function_handler handler;
  const char* name;
};

#define FDBOX_MAJOR 1
#define FDBOX_MINOR 2 
#define FDBOX_PATCH 1
#define FDBOX_RELEASE 1

#define FDBOX_MKVER(major, minor, patch, release) ( (release) | (patch << 4) | (minor << 8) | (major << 12) )
#define FDBOX_VERSION  FDBOX_MKVER( FDBOX_MAJOR, FDBOX_MINOR, FDBOX_PATCH, FDBOX_RELEASE ) 
#define FDBOX_VERSION_STR STR(FDBOX_MAJOR)"."STR(FDBOX_MINOR)"."STR(FDBOX_PATCH)"-"STR(FDBOX_RELEASE)

#ifndef __MAIN__
extern struct applet commands[];
#endif

#endif // __fdbox_h__
