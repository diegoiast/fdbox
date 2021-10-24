# Coding standard

The aim of the code standard is to make code smaller. More text you 
see on screen - more you can read. The general idea is also to brind
new methodologies to MSDOS, and try to avoid using older design parrters on modern code.

There is a clang-format configuration file. It is enforced in the CI/CD.

## Brackets
Open on current line, not following

OK:
```    
    if (foobar) {
        /* something */
    }
```

BAD:
```
    if (foobar)
    {
        /* something */
    }
```

All `for` and `if` will be followed by `{}` - never a single line
blocks.

## Variable naming
Split words by `_`. Naming is always lower case. Upper case are `#define`

OK:
``` C
    int some_variable;
    int a;
    #define MAGIC_NUMBER
```
Bad: 
``` C
    int SomeVariable;
    int otherVariable;
    #define MaximumEffort true
```

## C89 compatibility

This code needs to compile under TC 1.02. So, we have some limitations:

1. All files must end gin `\r\n`, not `\r`. 
2. Variables are defined only the the beginnig of each scope.
3. When possible - bring Posix APIs to DOS, and not DOS APIs to Posix/Windows. For example - we have a port of `glob()` for Windows and MSDOS.
4. Try to make generic code, and then most of the development can be done on modern platforms (see the readline library, or `glob` implementations)