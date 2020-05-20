CCFLAGS=-Os -g -Wall -I.
.PHONY: all clean

all: fdbox



beep.o: dos/beep.c dos/beep.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

call.o: dos/call.c dos/call.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

cd.o: dos/cd.c dos/cd.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

cls.o: dos/cls.c dos/cls.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

copy.o: dos/copy.c dos/copy.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

date.o: dos/date.c dos/date.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

dir.o: dos/dir.c dos/dir.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

echo.o: dos/echo.c dos/echo.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

exit.o: dos/exit.c dos/exit.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

for.o: dos/for.c dos/for.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

goto.o: dos/goto.c dos/goto.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

if.o: dos/if.c dos/if.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

mv.o: dos/mv.c dos/mv.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

path.o: dos/path.c dos/path.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

prompt.o: dos/prompt.c dos/prompt.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

pwd.o: dos/pwd.c dos/pwd.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

reboot.o: dos/reboot.c dos/reboot.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

rem.o: dos/rem.c dos/rem.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

set.o: dos/set.c dos/set.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

shift.o: dos/shift.c dos/shift.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

time.o: dos/time.c dos/time.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

type.o: dos/type.c dos/type.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

cal.o: unix/cal.c unix/cal.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

head.o: unix/head.c unix/head.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

hexdump.o: unix/hexdump.c unix/hexdump.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

tail.o: unix/tail.c unix/tail.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@

help.o: ./help.c ./help.h fdbox.h 
\t $(CC) $(CCFLAGS) -c $< -o $@
main.o: main.c fdbox.h  dos/beep.h dos/call.h dos/cd.h dos/cls.h dos/copy.h dos/date.h dos/dir.h dos/echo.h dos/exit.h dos/for.h dos/goto.h dos/if.h dos/mv.h dos/path.h dos/prompt.h dos/pwd.h dos/reboot.h dos/rem.h dos/set.h dos/shift.h dos/time.h dos/type.h unix/cal.h unix/head.h unix/hexdump.h unix/tail.h ./help.h
\t $(CC) $(CCFLAGS) -c $< -o $@
OBJECTS = beep.o call.o cd.o cls.o copy.o date.o dir.o echo.o exit.o for.o goto.o if.o mv.o path.o prompt.o pwd.o reboot.o rem.o set.o shift.o time.o type.o cal.o head.o hexdump.o tail.o help.o main.o

fdbox: $(OBJECTS)
\t$(CC) $(OBJECTS) -o fdbox

clean:
\trm -f $(OBJECTS) fdbox
distclean: clean
\trm -f  dos/beep.h dos/call.h dos/cd.h dos/cls.h dos/copy.h dos/date.h dos/dir.h dos/echo.h dos/exit.h dos/for.h dos/goto.h dos/if.h dos/mv.h dos/path.h dos/prompt.h dos/pwd.h dos/reboot.h dos/rem.h dos/set.h dos/shift.h dos/time.h dos/type.h unix/cal.h unix/head.h unix/hexdump.h unix/tail.h ./help.h main.c Makefile
gitclean: clean
\tgit checkout dos/*.c unix/*.c 
\trm -f  dos/*.h unix/*.h main.c
