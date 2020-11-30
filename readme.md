# fdshell

A new shell for FreeDOS. Code will contain usable utilities 
borrowed from Unix and maintain compatibility with MSDOS 6.22
`command.com`.

## Building

The assumption is that this project will be developed on
Linux (or any Posix compliant OS), and we maintain compatibilty
with DOS at all times. The reason, is for better tooling (git, 
valgrind, strace, text-editors).

Code is strictly C (asm where needed). Supported environments

 * MS-DOS/FreeDOS/dosbox
    * TurboC 2.01 -  http://edn.embarcadero.com/article/20841
    * PacificC - http://www.grifo.com/SOFT/Pacific/uk_pacific.html (WIP)
    * OpenWatcoom  -http://www.openwatcom.com/ (WIP)
    * GCC ia16 - https://github.com/tkchia/gcc-ia16 (WIP)
    * OrangcC - https://github.com/LADSoft/OrangeC (WIP)
 * Linux
    * GCC / CLang + CMake.
 * Windows
    * GCC / Clang + CMake
    * https://github.com/LADSoft/OrangeC (WIP)
    * VisualStudio (?)
 * BSD ? (should work)
 * OSX ? (should work)
 * SerenityOS (WIP)
    

Check out this git repository:

    git clone [todo]
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

If you are using QtCrreator - build fill fail. Because QtCreator
by default will pass QMake variables (which you may not have), and
since again it will defaultto NMake. In the build tab, in the "Initial CMake parameters" type:

    -DCMAKE_BUILD_TYPE:String=Debug
    -DCMAKE_C_COMPILER:STRING=%{Compiler:Executable:C}
    -DCMAKE_CXX_COMPILER:STRING=%{Compiler:Executable:Cxx}
    -GNinja


## Why?
Because.

I started this project at 2001, and only recently re-started it from scratch. The idea came
after I saw Minibox, and though (as every programmer does...) "this is wrong, this is not the
way to do this, I will do it better". I am unsure if I am doing it better, but this is the 
best way to thank Ercan Ersoy - you give me a push to restart this project: thank you.

My old project:
http://dgi_il.tripod.com/fds/index.html

Minibox:
https://github.com/ercanersoy/Minibox

Ideas and inspiration do come from busybox (the applet concept for
example). However - this project does not share any code, and cannot
be considered a derived work (please don't copy code from Busybox here,
as I might re-license code, and want new implementations for those old ideas).  

## DosBox setup

I created a Linux shell script which will populate a "C:" drive for development.
Just execute it, and it will download all the compilers, and create an AUTOEXEC.BAT.

Then in DosBox config, edited the start up files (usually at the end) to this:


    mount c /home/diego/fdbox/etc/dosbox
    mount d /home/diego/fdbox/
    C:\AUTOEXEC.BAT
    d:
   
Then, each time I start dosbox, I have in the path all the compilers I need and 
I am redirected to the full code of the program in drive D:. The project contains 
a TurboC project file - so typing `tc` will bring up the IDE with the project ready
to hack.


TODO: If you save a file under DOS, DosBox will rename it to capital letters. I am
unsure how to automatically do this.

## License
GPL V3. See file license.txt

## TODO - future development
 - There are commands that are available on Unix and DOS, but differ in syntax
   unsure how to handle.
