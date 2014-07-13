# common variables and rules to make stm32 binaries
TARGET:=arm-none-eabi
CC:=$(TARGET)-gcc
CXX:=$(TARGET)-g++
CPP:=$(TARGET)-cpp
OBJCOPY:=$(TARGET)-objcopy
OBJDUMP:=$(TARGET)-objdump

STMDIR=/usr/share/stm32lib
STMINC=$(STMDIR)/inc
STMSRC=$(STMDIR)/src
STMLD=$(STMDIR)/ld

# OXDIR := ox // from Makefile
OXINC = $(OXDIR)/inc
OXSRC = $(OXDIR)/src

MICRORL_DIR=/usr/share/microrl/src

DEPSDIR=.deps
OBJDIR=.objs


# FreeRTOS: rtos/Source -> /usr/share/FreeRTOS/Source
RTDIR=rtos
RTSRC=$(RTDIR)/Source
RTINC=$(RTSRC)/include
# do it in Makefile
#SRCS += croutine.c
#SRCS += list.c
#SRCS += queue.c
#SRCS += tasks.c
#SRCS += timers.c
# symlink to Source/portable/GCC/ARM_CM3/port.c <<< change
#SRCS += heap_2.c
# symlink to Source/portable/GCC/ARM_CM4F/port.c <<< change
#SRCS += port.c
# Beware: portmacro.h is symlink to
#   rtos/Source/portable/GCC/ARM_CM3/portmacro.h or CM4F

###################################################

ALLFLAGS := -g -O2 -Wall
ALLFLAGS += -DUSE_STDPERIPH_DRIVER=1
ALLFLAGS += -ffreestanding
ALLFLAGS += -mlittle-endian
ifeq "$(NO_STDLIB)" "y"
  ALLFLAGS += -nostdlib
endif

$(info MCTYPE is $(MCTYPE) )
MCBASE := $(shell echo "$(MCTYPE)" | head -c 7  )
$(info MCBASE is $(MCBASE) )

ALLFLAGS  += -D$(MCTYPE) -D$(MCBASE)

ifeq "$(MCBASE)" "STM32F0"
  ALLFLAGS += -mthumb -mcpu=cortex-m0
endif
ifeq "$(MCBASE)" "STM32F1"
  ALLFLAGS += -mthumb -mcpu=cortex-m3
endif
ifeq "$(MCBASE)" "STM32F2"
  ALLFLAGS += -mthumb -mcpu=cortex-m3
endif
ifeq "$(MCBASE)" "STM32F3"
  ALLFLAGS += -mthumb -mcpu=cortex-m4f
  ALLFLAGS += -mfloat-abi=softfp -mfpu=fpv4-sp-d16
endif
ifeq "$(MCBASE)" "STM32F4"
  ALLFLAGS += -mthumb -mcpu=cortex-m4f
  ALLFLAGS += -mfloat-abi=softfp -mfpu=fpv4-sp-d16
endif


ALLFLAGS += -T$(LDSCRIPT)
ALLFLAGS += $(CFLAGS_ADD)


###################################################

ALLFLAGS += -I. -I$(STMINC)

SRCPATHS =  $(STMSRC) $(STMSRC)/templates

ifeq "$(USE_OX)" "y"
  SRCPATHS += $(OXSRC)
  ALLFLAGS += -I$(OXINC)
endif

ifeq "$(USE_FREERTOS)" "y"
  SRCPATHS += $(RTSRC) $(RTDIR)
  ALLFLAGS += -I$(RTINC)
endif

ifeq "$(USE_MICRORL)" "y"
  SRCPATHS += $(MICRORL_DIR)
  ALLFLAGS += -I$(MICRORL_DIR)
endif

ifeq "$(USE_USB_OTG)" "y"
  SRCPATHS += $(STMSRC)/usb_otg usb
  ALLFLAGS += -I$(STMINC)/usb_otg -Iusb
  ALLFLAGS += -DUSE_USB_OTG_FS=1 -DUSE_EMBEDDED_PHY=1
endif

vpath %.c   $(SRCPATHS)
vpath %.cpp $(SRCPATHS)
vpath %.s   $(OXSRC)/startup $(STMSRC)
vpath %.o   $(OBJDIR)
vpath %.d   $(DEPSDIR)


OBJS0a = $(SRCS:.cpp=.o)
OBJS0 = $(OBJS0a:.c=.o)
OBJS  = $(OBJS0:.s=.o)
OBJS1 = $(addprefix $(OBJDIR)/,$(OBJS))

CFLAGS   = $(ALLFLAGS)  -std=c11
CXXFLAGS = $(ALLFLAGS)  -std=c++11 -fno-rtti -fno-exceptions

###################################################

.PHONY: proj

all: proj dirs

dirs:
	mkdir -p $(DEPSDIR) $(OBJDIR)

proj:  dirs $(PROJ_NAME).elf

$(OBJDIR)/*.o:  Makefile $(OXDIR)/mk/common_cortex.mk


$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -MMD -o $@ $<
	mv $(OBJDIR)/$*.d $(DEPSDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<
	mv $(OBJDIR)/$*.d $(DEPSDIR)

$(OBJDIR)/%.o: %.s
	$(CC) $(CFLAGS) -I$(OXSRC)/startup -c -o $@ $<


$(PROJ_NAME).elf: $(OBJS1)
	$(LINK) $(CFLAGS) -Wl,-Map=$(PROJ_NAME).map $^ -o $@
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	$(OBJDUMP) -h -f -d -S $(PROJ_NAME).elf > $(PROJ_NAME).lst


clean:
	rm -f *.o *.d $(OBJDIR)/*.o $(DEPSDIR)/*.d
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).lst
	rm -f $(PROJ_NAME).bin

#

