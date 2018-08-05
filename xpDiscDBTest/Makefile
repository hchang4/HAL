
CC = g++

CROSS_COMPILE=

INCLUDES_GC700XP = ../../../include
INCLUDES = ../include
LIB = -L../../../lib
#LIBS = -ldbapi -ldbinterface -lgetenum -lxmlparser -lxmltok
LIBS = 
STATIC_OBJS_DIR = ../../../common_static_modules

COMPILE_FOR = COMPILE_FOR_PC
OPT_FLAGS = -O2 -g

CFLAGS = -Wall -Wstrict-prototypes $(OPT_FLAGS)
CPPFLAGS = -I$(INCLUDES_GC700XP) -I$(INCLUDES) -D$(COMPILE_FOR) -DLINUX
COMPILE = $(CROSS_COMPILE)$(CC) $(CPPFLAGS) $(LIB) -c $(CFLAGS)

# Each source file will have an associated dependency
# file with the ".d" extension.
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
DEPS := $(patsubst %.cpp,%.d,$(SRCS))
DEPS_HPP := $(wildcard *.hpp,*.h)

# "all" is the default target.
all:xpdevdisc

EXTRA_OBJS = $(STATIC_OBJS_DIR)/loglib.o $(STATIC_OBJS_DIR)/wrappers.o \
$(STATIC_OBJS_DIR)/debug.o $(STATIC_OBJS_DIR)/customerror.o \
$(STATIC_OBJS_DIR)/runAsRTTask.o $(STATIC_OBJS_DIR)/dbList.o


xpdevdisc: configHWSetup.o  $(OBJS) Makefile
	$(CROSS_COMPILE)$(CC) $(CPPFLAGS) *.o  -lgc700xphal -ltableAPI $(EXTRA_OBJS) -o $@ \
        -lc $(LIB) $(LIBS)

# Specify that the dependency files depend on the C source files.
%.d: %.cpp Makefile
	#$(CC) -M $(CPPFLAGS) $< > $@
	$(CROSS_COMPILE)$(CC) -M $(CPPFLAGS) $< | sed s/\\.o/.d/ > $@


# Specify that all .o files depend on .c files and the
# dependency .d files, and indicate how the .c files are converted
# (compiled) to the .o files. Note that the .o files also depend
# on this Makefile.
%.o: %.cpp %.d Makefile
	$(COMPILE) -o $@ $<

clean:
	rm -rf *~
	rm -rf $(OBJS)
	rm -rf $(DEPS)
	rm -rf *.so
	rm -rf core
	rm -rf *.o
	rm -rf *.d
	rm -rf xpdevdisc

# Call "make lint" to lint check (using splint (from splint.org)) to
# check your C programs.
lint:
	splint -f ../../../.splintflags  -weak +posixlib $(INCLUDES_GC700XP) $(INCLUDES) *.c

explain:
	@echo "The following information represents the program:"
	@echo "Source files:      $(SRCS)"
	@echo "Object files:      $(OBJS)"
	@echo "Dependency files:  $(DEPS)"
depend: $(DEPS)
	@echo "Dependencies are now up-to-date."


