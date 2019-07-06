#ifndef FDBOX_APPLETS
#define FDBOX_APPLETS


typedef int (*function_handler)(int, char*[]);
typedef const char* (*function_help)();

struct applet { 
  function_help help_text;
  function_handler handler;
  const char* name;
};


struct applet* find_applet(const char* applet_name, struct applet commands[]);

#endif // FDBOX_APPLETS
