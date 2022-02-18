LIB = libCMSIS_DSP.a

CC = $(ARMGCC_PATH)arm-none-eabi-gcc
AR = $(ARMGCC_PATH)arm-none-eabi-ar
RANLIB = $(ARMGCC_PATH)arm-none-eabi-ranlib



ifndef COMPILE_OPTS
COMPILE_OPTS= ERROR -Wall -O2 -mlittle-endian -mthumb -mcpu=cortex-m4 -ffreestanding -nostdlib
endif

CFLAGS=$(COMPILE_OPTS)
#CFLAGS += -Iinc #-Iinc/device_support #-Iinc/core_support
CFLAGS += -I Include
CFLAGS += -I Device

#CFLAGS += -I ./
#CFLAGS += $(INCLUDE_DIRS)


OBJS = $(patsubst %.c,%.o,$(wildcard DSP_Lib/Source/*/*.c))

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) -r $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

clean:
	-rm -f $(OBJS)
	-rm -f $(LIB)

.PHONY: all clean
