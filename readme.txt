fdbox
-----

A new command interpreter for FreeDOS. Code will contain usable utilities 
borrowed from Unix and maintain (basic) compatibility with MSDOS 6.22 
command.com, while (sometimes) improving. 

This is a modern attempt at writing a command.com shell in C, using  modern 
technologies. The code is cross platform, on modern platforms the code is built
the project using CMake, we use a Github CI to build all code, we enforce 
indentation using clang-format - and yet still, we can compiled using a freely
available TurboC (http://edn.embarcadero.com/article/20841), 
DJGPP (http://www.delorie.com/djgpp/) and
OpenWatcom (https://open-watcom.github.io/).

This zip contains binaries for OSX (x86-64), Linux (x86-64), Win64 and MSDOS. 

For more information visit (and full source):
https://github.com/elcuco/fdbox

(and yes, it feels weird to write HTTPS urls for a DOS project)