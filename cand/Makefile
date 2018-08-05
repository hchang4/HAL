# Path processing task makefile
# Based upon Listing 7-1 of the Linux Programming Bible
# by John Goerzen.

# CC is the variable that will contain your compiler
CC = g++

CROSS_COMPILE=

#CC = ppc_6xx-g++
 
#LIBDIR=/usr/lib
LIBDIR=../../../lib
#LIBDIR=/usr/local/ppc/eldk-4.0/ppc_6xx/usr/lib/
STATIC_OBJS_DIR = ../../../common_static_modules

#Specify include file search directory.
INCLUDES_GC700XP = ../../../include
INCLUDES = ../include/
INCLUDES1 = /home/vipin/GC700XP/all/tools/Drivers/IXXAT-PCI0-CAN-Driver/bci/
LIB = -L$(LIBDIR)

# The following can be overridden via the top level make file
COMPILE_FOR = COMPILE_FOR_PC
OPT_FLAGS = -O2 -g
DEBUG_STATUS=DEBUG_STATUS_ENABLED
 
# Specify compiler flags
# CFLAGS are the gcc compilation flags.
# Values in CFLAGS may be overridden.
# Values that may not be overridden are placed in ALL_CFLAGS
# prior to the CFLAG variable.
# -g    produces debugging information in the operating system's
#       native format (defaults to level 2)
:# -Wall Include recommended warning options
# -Wstrict-prototypes
#       Warn if a function is declared or defined without
#       specifying arguement types.
#
# CPPFLAGS are the pre-processor flags.
CFLAGS = -Wall $(OPT_FLAGS) -DCANDLOG_EN  #-DBCI_MODE -DLINUX
CPPFLAGS = -I$(INCLUDES_GC700XP) -I$(INCLUDES) -I$(INCLUDES1) -D$(COMPILE_FOR) -D$(DEBUG_STATUS)
COMPILE = $(CROSS_COMPILE)$(CC) $(CPPFLAGS) $(LIB) -c $(CFLAGS)

# Each source file will have an associated dependency
# file with the .d extension.
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
DEPS = $(patsubst %.cpp,%.d,$(SRCS))

EXTRA_OBJS = $(STATIC_OBJS_DIR)/debug.o $(STATIC_OBJS_DIR)/runAsRTTask.o 

# all is the default target.
all: cand

cand: $(DEPS) $(OBJS) Makefile $(EXTRA_OBJS)
	$(CROSS_COMPILE)$(CC) $(LIB) -lipc -lsqlite3 -lavgArchDB -lLogApi -ldbapi -lUnitConv -lxmlgen -lstrTable -lgetenum -ltableAPI -ldbinterface -lxmlparser -lxmltok -lmirddipc -lTableMetaDataSHM -ltablexmlparser cand.o candlog.o dfifo.o $(EXTRA_OBJS) -o $@ #-lBCI

# Specify that the dependency files depend on the C source files.
%.d: %.cpp Makefile
#	$(CROSS_COMPILE)$(CC) -M $(CPPFLAGS) $< > $@
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
	rm -rf cand

explain:
	@echo The following information represents the program
	@echo Source files      $(SRCS)
	@echo Object files      $(OBJS)
	@echo Dependency files  $(DEPS)

depend: $(DEPS)
	@echo Dependencies are now up-to-date.

-include $(DEPS)
