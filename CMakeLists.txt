cmake_minimum_required(VERSION 3.10)

# add the executable
project(fdbox VERSION 0.1)

include(cmake/CPM.cmake)

cpmaddpackage(
  NAME
  Format.cmake
  VERSION
  1.7.2
  GITHUB_REPOSITORY
  TheLartians/Format.cmake
  OPTIONS # set to yes skip cmake formatting
  "FORMAT_SKIP_CMAKE NO"
  # path to exclude (optional, supports regular expressions)
  "CMAKE_FORMAT_EXCLUDE cmake/CPM.cmake")

add_executable(
  tests
  src/tests.c
  src/lib/applet.c
  src/lib/args.c
  src/lib/environ.c
  src/lib/readline.c
  src/lib/strextra.c
)
target_include_directories(tests PUBLIC "${PROJECT_BINARY_DIR}"
                                        "${PROJECT_SOURCE_DIR}/src/")

add_executable(
  fdbox
  readme.md
  src/main.c
  src/applets.c
  src/help.c
  src/lib/applet.c
  src/lib/args.c
  src/lib/environ.c
  src/lib/strextra.c
  src/lib/readline.c
  src/lib/serenityos/serenity-glob.c
  src/dos/beep.c
  src/dos/call.c
  src/dos/cd.c
  src/dos/cls.c
  src/dos/command.c
  src/dos/copymove.c
  src/dos/datetime.c
  src/dos/del.c
  src/dos/dir.c
  src/dos/echo.c
  src/dos/exit.c
  src/dos/for.c
  src/dos/goto.c
  src/dos/history.c
  src/dos/if.c
  src/dos/md.c
  src/dos/path.c
  src/dos/prompt.c
  src/dos/reboot.c
  src/dos/rem.c
  src/dos/set.c
  src/dos/shift.c
  src/dos/type.c
  src/dos/ver.c
  src/unix/cal.c
  src/unix/head.c
  src/unix/hexdump.c
  src/unix/tail.c src/lib/serenityos/serenity-glob.c src/lib/serenityos/serenity-glob.h)

if(WIN32)
  target_sources(fdbox PRIVATE
      src/lib/win32/win32-glob.c
      src/lib/win32/win32-glob.h
      src/lib/tc202/stdextra.c
    )

  target_sources(tests PRIVATE
      src/lib/tc202/stdextra.c
    )
endif()

target_include_directories(fdbox PUBLIC "${PROJECT_BINARY_DIR}"
                                        "${PROJECT_SOURCE_DIR}/src/")
