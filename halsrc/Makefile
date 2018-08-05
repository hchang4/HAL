# Path processing task makefile
# Based upon Listing 7-1 of the Linux Programming Bible
# by John Goerzen.

# CC is the variable that will contain your compiler
CC = g++

CROSS_COMPILE=

#C = ppc_6xx-g++
 
#LIBDIR=/usr/lib
LIBDIR=../../../lib
#IBDIR=/usr/local/ppc/eldk-4.0/ppc_6xx/usr/lib/
STATIC_OBJS_DIR =../../../common_static_modules

#Specify include file search directory.
INCLUDES_GC700XP = ../../../include
INCLUDES = ../include
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
#CFLAGS = -Wall $(OPT_FLAGS)
CFLAGS = -Wall $(OPT_FLAGS) -O0
CPPFLAGS = -I$(INCLUDES) -I$(INCLUDES_GC700XP) -D$(COMPILE_FOR) -D$(GC_MODEL) -D$(DEBUG_STATUS)
COMPILE = $(CROSS_COMPILE)$(CC) $(CPPFLAGS) $(LIB) -c $(CFLAGS)

# Each source file will have an associated dependency
# file with the .d extension.
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
DEPS = $(patsubst %.cpp,%.d,$(SRCS))

EXTRA_OBJS = $(STATIC_OBJS_DIR)/debug.o $(STATIC_OBJS_DIR)/UDPClient.o

# all is the default target.
all: libgc700xphal.so.1.0.1


libgc700xphal.so.1.0.1: $(DEPS) $(OBJS) $(EXTRA_OBJS) Makefile
	$(CROSS_COMPILE)$(CC) $(LIB) -fPIC -lipc IMBComm.o SerialModeCtrl.o Pressure.o IRKeyPad.o CPU_ADC_AD7908.o FID_DAC_AD5570ARSZ.o FID_ADC_AD7811YRU.o FIDOperations.o FIDControl.o FPD_ADC_7705.o FPDControl.o Diagnostic.o FFBComm.o AnalogIn.o AnalogOut.o BaseDev.o CANComm.o DigitalIn.o DigitalOut.o EPC.o Fragment.o DataFragment.o HeaterCtrl.o PreampStream.o PreampStreamSim.o PreampStreamWrapper.o PreampConfig.o Reliability.o ResolveDevName.o RTD.o Serial.o SolenoidCtrl.o LtLoi.o crc16.o Fifo.o BoardSlotInfo.o CycleClockSync.o FpdG2control.o HwInhibitCtrl.o $(EXTRA_OBJS) -o $@ -shared -Wl,-soname,libgc700xphal.so.1 -lc
	cp -af $@ $(LIBDIR)
	cd $(LIBDIR); ln -sf libgc700xphal.so.1.0.1 libgc700xphal.so.1
	cd $(LIBDIR); ln -sf libgc700xphal.so.1 libgc700xphal.so


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
	rm -rf $(OBJS)
	rm -rf $(DEPS)
	rm -rf libgc700xphal.so.1.0.1
	rm -rf core
	rm -rf $(LIBDIR)/*gc700xphal.so.*

explain:
	@echo The following information represents the program
	@echo Source files      $(SRCS)
	@echo Object files      $(OBJS)
	@echo Dependency files  $(DEPS)

depend: $(DEPS)
	@echo Dependencies are now up-to-date.

-include $(DEPS)
