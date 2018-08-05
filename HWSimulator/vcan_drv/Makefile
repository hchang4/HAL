# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y


# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)

CFLAG += -msoft-float

ifneq ($(KERNELRELEASE),)
# call from kernel build system

# We build the code TWICE as TWO DIFFERENT modules. The kernel
# will not allow us to insert the same module twice (and changing
# the filename does not help- they have to be build with different
# names here).
# module params configure the same driver to operate on the 1st or
# 2nd can chip.

virtcan-objs := vcan_drv.o 

obj-m	:= virtcan.o

else

#KERNELDIR ?= /usr/local/ppc/eldk-4.0/ppc_6xx/usr/src/linux
#KERNELDIR ?= /usr/src/linux
#KERNELDIR ?=  /usr/src/kernels/2.6.11-1.1369_FC4smp-i686
KERNELDIR ?= /lib/modules/$(shell uname -r)/build 
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) 

endif



clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions


depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif

