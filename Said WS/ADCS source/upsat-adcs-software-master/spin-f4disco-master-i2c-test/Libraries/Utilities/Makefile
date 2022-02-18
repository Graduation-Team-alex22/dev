LIB = libSTM32F4_Discover_Util.a

CC = $(ARMGCC_PATH)arm-none-eabi-gcc
AR = $(ARMGCC_PATH)arm-none-eabi-ar
RANLIB = $(ARMGCC_PATH)arm-none-eabi-ranlib



ifndef COMPILE_OPTS
COMPILE_OPTS=-Wall -O2 -mlittle-endian -mthumb -mcpu=cortex-m4 -ffreestanding -nostdlib
endif

CFLAGS=$(COMPILE_OPTS)
CFLAGS += -ISTM32F4-Discovery 

OBJS = $(patsubst %.c,%.o,$(wildcard STM32F4-Discovery/*.c))

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) -r $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	-rm -f $(OBJS)
	-rm -f $(LIB)

.PHONY: all clean
