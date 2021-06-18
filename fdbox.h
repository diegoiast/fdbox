#ifndef __fdbox_h__
#define __fdbox_h__

/* https://stackoverflow.com/a/5459929/78712 */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/* https://stackoverflow.com/questions/1486904/how-do-i-best-silence-a-warning-about-unused-variables
 */
#define UNUSED(expr)                                                                               \
        do {                                                                                       \
                (void)(expr);                                                                      \
        } while (0)

#define FDBOX_MAJOR 0
#define FDBOX_MINOR 0
#define FDBOX_PATCH 1
#define FDBOX_RELEASE 1

#define FDBOX_MKVER(major, minor, patch, release)                                                  \
        ((release) | (patch << 4) | (minor << 8) | (major << 12))
#define FDBOX_VERSION FDBOX_MKVER(FDBOX_MAJOR, FDBOX_MINOR, FDBOX_PATCH, FDBOX_RELEASE)
#define FDBOX_VERSION_STR                                                                          \
        STR(FDBOX_MAJOR) "." STR(FDBOX_MINOR) "." STR(FDBOX_PATCH) "-" STR(FDBOX_RELEASE)

#define DEBUG_LINE printf("%s:%d %s() - PING\n", __FILE__, __LINE__, __FUNCTION__)

#ifdef _POSIX_C_SOURCE
#define DIRECTORY_DELIMITER "/"
#define ARGUMENT_DELIMIER '-'
#define ALL_FILES_GLOB "*"
#endif

#if defined(__WIN32__) || defined(__MSDOS__)
#define DIRECTORY_DELIMITER "\\"
#define ARGUMENT_DELIMIER '/'
#define ALL_FILES_GLOB "*.*"
#endif

#endif // __fdbox_h__
