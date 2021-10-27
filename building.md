# Building

The assumption is that this project will be developed on
Linux (or any Posix compliant OS), and will maintain compatibilty
with DOS at all times. The reason, is for better tooling (git,
valgrind, strace, text-editors, clag-format ...etc).

Code is strictly C (asm where needed, nothing added yet). Supported environments:

 * FreeDOS/MS-DOS/dosbox
    * TurboC 2.01 -  http://edn.embarcadero.com/article/20841
    * DJGPP - http://www.delorie.com/djgpp/ (WIP, interactive shell not working)
    * PacificC - http://www.grifo.com/SOFT/Pacific/uk_pacific.html (soon)
    * OpenWatcoom  - http://www.openwatcom.com/ (soon)
    * GCC ia16 - https://github.com/tkchia/gcc-ia16 (soon)
    * OrangcC - https://github.com/LADSoft/OrangeC (soon)
 * Linux
    * GCC / CLang + CMake.
 * Windows
    * GCC / CLang + CMake
    * https://github.com/LADSoft/OrangeC (soon)
    * VisualStudio (?)
    * OpenWatcoom  - http://www.openwatcom.com/ (soon)
 * BSD ? (should work, untested)
 * OSX ? (should work, untested)
 * SerenityOS (soon)


Check out this git repository:

    git clone https://github.com/elcuco/fdbox/
    cmake -S . -B build -GNinja
    cmake --build build

On Windows, the best build environment I found is QtCreator. No need for Qt-dev
but you will need Ninja and CMake to be installed from the installer. Before the
cmake commmand type on your cmd:

    set PATH=%PATH%;c:\Qt\Tools\CMake_64\bin;c:\Qt\Tools\Ninja;c:\Qt\Tools\mingw810_64\bin

By default CMake on windows tries to generate NMake (VC) files. I opted
for ninja, but MinGW Makefiles should work as well. You might want to update
the paths to Qt, CMake, Ninja and MinGW as its installed on your system. The `-B build`
is the build directory and can be changed as you please.

If you are using QtCreator - build will fail. Because QtCreator
by default will pass QMake variables (which you may not have), and
since again it will default to NMake. In the build tab, in the "Initial CMake parameters" type:

    -DCMAKE_BUILD_TYPE:String=Debug
    -DCMAKE_C_COMPILER:STRING=%{Compiler:Executable:C}
    -DCMAKE_CXX_COMPILER:STRING=%{Compiler:Executable:Cxx}
    -GNinja

In `etc/fdshell-qtcreator-indent.xml` you will find the indentaion
configuration I use for. It should be equivalent to the `clang-format`
one.

I also managed to compile this project using VSCode. You willl
need to install the CMake extension (https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
and C++ support (https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools).

If you plan on building this on DOS - there is a TurboC 2.02 project. Code
does compile under TC - its tested all the time. If you want to test DJGPP/RHIDE
there is a batch file which will run the correct Makefile (under the generated dos drive, 
see DosBox setup bellow).
