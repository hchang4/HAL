# Path processing task makefile
# Based upon Listing 7-1 of the Linux Programming Bible
# by John Goerzen.

# CC is the variable that will contain your compiler
CC = g++

CROSS_COMPILE=

LIBDIR=../../../lib
STATIC_OBJS_DIR = ../../../common_static_modules

#Specify include file search directory.
INCLUDES_GC700XP = ../../../include
INCLUDES = ../include/
LIB = -L$(LIBDIR)

# The following can be overridden via the top level make file
COMPILE_FOR = COMPILE_FOR_PC
GC_MODEL = MODEL_370XA
OPT_FLAGS = -O2 -g
DEBUG_STATUS=DEBUG_STATUS_ENABLED
 
# Specify compiler flags
# CFLAGS are the gcc compilation flags.
# Values in CFLAGS may be overridden.
# Values that may not be overridden are placed in ALL_CFLAGS
# prior to the CFLAG variable.
# -g    produces debugging information in the operating system's
#       native format (defaults to level 2)
# -Wall Include recommended warning options
# -Wstrict-prototypes
#       Warn if a function is declared or defined without
#       specifying arguement types.
#
# CPPFLAGS are the pre-processor flags.
CFLAGS = -Wall $(OPT_FLAGS) -O0
CPPFLAGS = -I$(INCLUDES) -I$(INCLUDES_GC700XP)  -D$(COMPILE_FOR) -D$(GC_MODEL) -D$(DEBUG_STATUS)
COMPILE = $(CROSS_COMPILE)$(CC) $(CPPFLAGS) $(LIB) -c $(CFLAGS)

# Each source file will have an associated dependency
# file with the .d extension.
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
DEPS = $(patsubst %.cpp,%.d,$(SRCS))

EXTRA_OBJS = $(STATIC_OBJS_DIR)/SysLogToText.o $(STATIC_OBJS_DIR)/debug.o $(STATIC_OBJS_DIR)/hardwareHelpers.o

# all is the default target.
all: TestHAL TestHALPre TestHALRTD

TestHAL: $(DEPS) $(OBJS) Makefile
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(LIB) -lgc700xphal -lipc -ldbapi -ltableAPI -lUnitConv -lxmlparser -lxmltok -lgetenum -ldbinterface -lmirddipc -lTableMetaDataSHM -ltablexmlparser TestHAL.o $(EXTRA_OBJS) -o $@

TestHALPre: $(DEPS) $(OBJS) Makefile
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(LIB) -lgc700xphal -lipc TestHALPre.o $(EXTRA_OBJS) -o $@

TestHALRTD: $(DEPS) $(OBJS) Makefile
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(LIB) -lgc700xphal -lipc -ldbapi -ltableAPI -lUnitConv -lxmlparser -lxmltok -lgetenum -ldbinterface -lmirddipc -lTableMetaDataSHM -ltablexmlparser TestHALRTD.o $(EXTRA_OBJS) -o $@

# Specify that the dependency files depend on the C source files.
%.d: %.cpp Makefile
	$(CROSS_COMPILE)$(CC) -M $(CPPFLAGS) $< | sed s/\\.o/.d/ > $@


# Specify that all .o files depend on .c files and the
# dependency .d files, and indicate how the .c files are converted
# (compiled) to the .o files. Note that the .o files also depend
# on this Makefile.
%.o: %.cpp %.d Makefile
	$(COMPILE) -o $@ $<

clean:
	rm -rf $(OBJS)
	rm -rf $(DEPS)
	rm -rf TestHAL TestHALPre TestHALRTD

explain:
	@echo The following information represents the program
	@echo Source files      $(SRCS)
	@echo Object files      $(OBJS)
	@echo Dependency files  $(DEPS)

depend: $(DEPS)
	@echo Dependencies are now up-to-date.

-include $(DEPS)
