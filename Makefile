#
#
#
SRCS = main.c score.c
OBJS = main.o score.o

XLIB         = -lX11
MOTIFLIBS    = -lXm -lXt
EXTENSIONLIB = -lXext
SYSLIBS      = -lm
LIBS         = -L/opt/local/lib $(MOTIFLIBS) $(EXTENSIONLIB) $(XLIB) $(SYSLIBS)

LOCALINCS = -I.
MOTIFINCS = -I/opt/local/include -I/opt/local/include/X11
INCS      = $(LOCALINCS) $(MOTIFINCS)

CDEBUGFLAGS = -g
CFLAGS      = $(DEFINES) $(INCS) $(CDEBUGFLAGS)

DESTINATION = $(HOME)/bin

PROG  = mtetris
DEBUG = debug

DEPEND = makedepend

.SUFFIXES: .o .c

.c.o:
	$(CC) -c $(INCS) $(CFLAGS) $*.c

all: $(PROG)

$(PROG): $(SRCS) Makefile
	$(CC) -o $(PROG) $(CFLAGS) $(SRCS) $(LIBS)
	strip $(PROG)

$(DEBUG): $(OBJS) Makefile
	$(CC) -o $(DEBUG) $(OBJS) $(LIBS)

install: $(PROG)
	install -c -s $(PROG) $(DESTINATION)

clean: 
	rm -f *~ *.bak core *.o \#*\# $(PROG) $(DEBUG) lint_errs

depend:
	$(DEPEND) $(INCS) $(SRCS)


# DO NOT DELETE THIS LINE -- make depend depends on it.
