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
  src/dos/batch.c
  src/dos/load.c
  src/dos/md.c
  src/dos/path.c
  src/dos/prompt.c
  src/dos/chcp.c
  src/dos/keyb.c
  src/dos/reboot.c
  src/dos/rem.c
  src/dos/set.c
  src/dos/shift.c
  src/dos/type.c
  src/dos/ver.c
  src/unix/cal.c
  src/unix/head.c
  src/unix/hexdump.c
  src/unix/tail.c)

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

# Translations (src/lib/nls.h, po/): -DFDBOX_NLS=ON builds fdbox with
# gettext and the catalogues of po/ as <prefix>/share/locale/<lang>/
# LC_MESSAGES/fdbox.mo
option(FDBOX_NLS "Translated messages (gettext)" OFF)
if(FDBOX_NLS)
  find_package(Intl REQUIRED)
  find_program(MSGFMT msgfmt REQUIRED)
  include(GNUInstallDirs)
  set(FDBOX_LOCALEDIR "${CMAKE_INSTALL_FULL_LOCALEDIR}" CACHE PATH "Where the catalogues are installed")
  target_compile_definitions(fdbox PRIVATE FDBOX_NLS "FDBOX_LOCALEDIR=\"${FDBOX_LOCALEDIR}\"")
  target_include_directories(fdbox PRIVATE ${Intl_INCLUDE_DIRS})
  target_link_libraries(fdbox PRIVATE ${Intl_LIBRARIES})
  file(GLOB _pos CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/po/*.po")
  foreach(_po IN LISTS _pos)
    get_filename_component(_lang "${_po}" NAME_WE)
    set(_mo "${PROJECT_BINARY_DIR}/locale/${_lang}/LC_MESSAGES/fdbox.mo")
    add_custom_command(OUTPUT "${_mo}"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/locale/${_lang}/LC_MESSAGES"
      COMMAND "${MSGFMT}" -o "${_mo}" "${_po}"
      DEPENDS "${_po}")
    list(APPEND _mos "${_mo}")
    install(FILES "${_mo}" DESTINATION "${FDBOX_LOCALEDIR}/${_lang}/LC_MESSAGES")
  endforeach()
  add_custom_target(fdbox-catalogues ALL DEPENDS ${_mos})
endif()
