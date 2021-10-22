# fdbox

A new command interpreter for FreeDOS. Code will contain usable utilities
borrowed from Unix and maintain (basic) compatibility with 
MSDOS 6.22 `command.com`, while (sometimes) improving. 

<p align="center">
<img src="fdbox-dosbox.png" />
</p>

The program compiles also as a native Linux executable
<p align="center">
<img src="fdbox-linux.png" />
</p>

.. and Windows as well 
<p align="center">
<img src="fdbox-win10.png" />
</p>

.. and Apple silicon as well 
<p align="center">
<img src="fdbox-osx-apple-silicon.png" />
</p>

## Status

 * Only DOS commands are supported. Unix commands might follow.
 * Code compiles only under GCC (Windows+Linux). TCC (DOS). 
 * Github actions compiles all the code for Linux, Windows and OSX, 
   the worlflow also runs the test on all platforms. (DOS automatic builds
   are not available yet).
 * Currently I am using LibC's functions - so I am limited to
   standard C code support for localization.
 * `TurboC` does need a proper makefile.
 * All commands support multiple arguments (like Unix shells), unlike DOS
   which you cannot do `del /r file1.txt *.bat /f`, and arguments can come
   even after file names (not only at the begining of the command).
 * Some commands share the same code (`copy`+`move`, `date`+`time`)
 * Interactive shell is beeing worked on. I want to have 4DOS command  
   completion, or bash/zsh. I like what the SerenityOS guys are doing -
   this can be another implementation detail.
 * Contains a bash shell script to generate the development enviroment
   for DOS+DosBOX

## Implementation status of commands
| command      | status | remark        |
|------------- | ------ | -----------   |
| `beep`       |  *done*  | needs to be tested on DOS/Windows, on unix it just beeps              |
| `cd`         |  *done*  |  |
| `cls`        |  *done*  |  |
| `copy`       |  WIP     | All copies are binaries. By design. <br> Recursive copy not implemented yet <br> Copy globs, and several files - not implemented yet <br> Append files (copy file1+file2 file3) not implemented yet |
| `date`/`time`|  WIP     | Missing AM/PM support <br> Not supported on windows (only DOS/Linux) |
| `del`        |  *done*  | Prompt might need more testing |
| `dir`        |  WIP     |  `/o?` order is funky. <br> `/p` - pausing is not implemented <br> Missing disk usage|
| `echo`       |  *done*  |  |
| `if`         |  *done*  |  |
| `md`/`mkdir` |  *done*  |  |
| `move`/`rename`/`ren` |  *done*  | All thes command are aliases, hardcoded |
| `type`       |  *done*  | Bonus: support also line numbers printing |
| `command`    | WIP | See bellow |
| `prompt`     |  *done*  | |
| `set`        |  *done*  | Does not handle quoting properly due to internal library issues |
| `rem`        |  *done*  | |
| `type`       |  *done*  | As a bonus, also prints line numbers |
| `ver`        |  *done*  | |

Major project worked on is interactive shell. The main task right now is the 
internal readline code (history, editor etc).

## Command line editor status:
 | Key                 | status    | comment |
 | ------------------- | --------- | ------- |
 | Left/right/home/end | *done*    |         |
 | Up/Down: history    | WIP       | fails randomly |
 | Backspace           | *done*    | |
 | Del                 | no        | |
 | control+a : HOME    | *done*    |
 | control+b : word left| no       |  |
 | control+c : cancel  | WIP       | de facto - clear line, fails on Posix |
 | control+d : EOF     | *done*    |
 | control+e : END     | *done*    |
 | control+f : word right| no       |  |
 | control+l : cls     | *done*    |         |
 | TAB                 | no        | |
 | insert/override     | WIP       | I have API, not implemented on edit|
 | esc                 | ????      | Fix breaks on Linux/OSX ? |          
 | insert              | *done*    | Toggle insert/override mode   |
 | control+arrors      | no        | move to next/prev word    |

## Compiler support
 1. Linux: Clang/GCC - this is the main development environment. Supported.
 2. Windows10: MinGW/TCM (9,10) - this is the main development environment. Supported.
 3. DOS: TC 2.02 - this is the main development environment. Supported.
    > __BUG__:
    >
    > `glob()` functionlaity tends to allocate too much >memory, and the code is faulty,
          not reporting this to the app. Results in garbage in the screen when
	  > `dir ` on a directory with lots of files (~80 on my tests, but it depends
	  > on memory available).
 4. DOS/OpenWatcoom (WIP): we have a CI building it. The generated binary crashes on startup.
 5. OSX: Works, not main target but actively developed (similar code path as Linux)
 6. DOS: DJGPP - I am haing problems in runtime. Seems like `tolower()` is breaking
    my code. Might be a compiler bug.
 7. DOS: PacificC - Its not a high priority, but we have a working branch called `pacific-c-support`
    which contains the work needed to make this project compile with that compiler.

## Building

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

Linux requirements:
```
apt install 7z wget dosbox
```

OSX requirements:
```
brew install p7zip wget dosbox
```

TODO: If you save a file under DOS, DosBox will rename it to capital letters. I am
unsure how to automatically do this.

## License
GPL V3. See file license.txt

## TODO - future development
 - I had this `configure` system which would create the sources for commands
   and auto generate the `applets.c` file. Not against the idea.
 - I need to make a TC makefile
 - Add support for Github actions  - and build the binaries (windows, linux and MSDOS
   on a tag).
 - There are commands that are available on Unix and DOS, but differ in syntax
   unsure how to handle.
 - MSDOS: Using XMS swapping wold be good, using SPWNO would be epic (http://www.cs.cmu.edu/%7Eralf/files.html)
 
