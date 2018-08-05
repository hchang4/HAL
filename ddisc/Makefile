
CC = g++

CROSS_COMPILE=

INCLUDES_GC700XP = ../../../include
INCLUDES = ../include
#INCLUDES1 = ../../../../../all/tools/Drivers/IXXAT-PCI0-CAN-Driver/bci/	
INCLUDES1 = /home/vipin/GC700XP/all/tools/Drivers/IXXAT-PCI0-CAN-Driver/bci/
LIB = -L../../../lib
#LIBS = -ldbapi -ldbinterface -lgetenum -lxmlparser -lxmltok
LIBS = 
STATIC_OBJS_DIR = ../../../common_static_modules

COMPILE_FOR = COMPILE_FOR_PC
GC_MODEL = MODEL_370XA
OPT_FLAGS = -O2 -g
#DEBUG_STATUS=DEBUG_STATUS_ENABLED uncomment to enable the debug mode
 

CFLAGS = -Wall -Wno-deprecated $(OPT_FLAGS) #-DBCI_MODE -DLINUX
CPPFLAGS = -I$(INCLUDES) -I$(INCLUDES_GC700XP)  -I$(INCLUDES1) -D$(COMPILE_FOR) -D$(GC_MODEL) # -D$(DEBUG_STATUS)
COMPILE = $(CROSS_COMPILE)$(CC) $(CPPFLAGS) $(LIB) -c $(CFLAGS)

# Each source file will have an associated dependency
# file with the ".d" extension.
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
DEPS := $(patsubst %.cpp,%.d,$(SRCS))
DEPS_HPP := $(wildcard *.hpp,*.h)

# "all" is the default target.
all:ddisc ddiscfallback

EXTRA_OBJS = ../../../lib/libtableAPI.so $(STATIC_OBJS_DIR)/loglib.o $(STATIC_OBJS_DIR)/wrappers.o \
$(STATIC_OBJS_DIR)/debug.o $(STATIC_OBJS_DIR)/customerror.o \
$(STATIC_OBJS_DIR)/runAsRTTask.o $(STATIC_OBJS_DIR)/dbList.o $(STATIC_OBJS_DIR)/UDPClient.o \
$(STATIC_OBJS_DIR)/hardwareHelpers.o

ddisc: configHWSetup.o ddisc.o  $(OBJS) Makefile $(EXTRA_OBJS)  
	$(CROSS_COMPILE)$(CC) $(CPPFLAGS) *.o  -lgc700xphal -lsqlite3 -lavgArchDB -lLogApi -ldbapi -lUnitConv -lxmlgen -lstrTable -lgetenum -lipc -ldbinterface -lxmlparser -lxmltok -lmirddipc -lTableMetaDataSHM -ltablexmlparser $(EXTRA_OBJS) -o $@ \
        -lc $(LIB) $(LIBS) #-lBCI

ddiscfallback:
	$(CROSS_COMPILE)$(CC) $(STATIC_OBJS_DIR)/hardwareHelpers.o $(STATIC_OBJS_DIR)/UDPClient.o ../halsrc/CPU_ADC_AD7908.o -o ddiscfallback $(CFLAGS) $(CPPFLAGS) -DMAKE_FALLBACK $(SRCS) -lc

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
	rm -rf *~
	rm -rf $(OBJS)
	rm -rf $(DEPS)
	rm -rf *.so
	rm -rf core
	rm -rf *.o
	rm -rf *.d
	rm -f ddisc ddiscfallback

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

-include $(DEPS)
