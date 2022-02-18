LIB = libSTM32_USB_OTG_Drive.a

CC = $(ARMGCC_PATH)arm-none-eabi-gcc
AR = $(ARMGCC_PATH)arm-none-eabi-ar
RANLIB = $(ARMGCC_PATH)arm-none-eabi-ranlib



ifndef COMPILE_OPTS
COMPILE_OPTS=-Wall -O2 -mlittle-endian -mthumb -mcpu=cortex-m4 -ffreestanding -nostdlib
endif

CFLAGS=$(COMPILE_OPTS)
CFLAGS += -Iinc 

OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))

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
