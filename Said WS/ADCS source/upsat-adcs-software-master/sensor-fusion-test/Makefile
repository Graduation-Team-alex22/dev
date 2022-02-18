# setup
#47424	


#ARMGCC_PATH= /media/ProgramData/ARM/GNU/gcc-arm-none-eabi-4_9-2014q4/bin/
#ARMGCCLIB_PATH = /media/ProgramData/ARM/GNU/gcc-arm-none-eabi-4_9-2014q4/arm-none-eabi/lib/armv7e-m/fpu
#ARMLIBGCC_PATH = /media/ProgramData/ARM/GNU/gcc-arm-none-eabi-4_7-2013q3/lib/gcc/arm-none-eabi/4.7.4/armv7e-m/fpu
#ARMLIBGCC_PATH = /media/ProgramData/ARM/GNU/gcc-arm-none-eabi-4_9-2014q4/arm-none-eabi/lib/armv7e-m/fpu

STlinkPath =/media/ProgramData/testGCC/stlink-master
export ARMGCC_PATH 
#
COMPILE_OPTS= -std=gnu99 -g -mcpu=cortex-m4 -mthumb  -O3  -fsingle-precision-constant   -mfloat-abi=hard -mfpu=fpv4-sp-d16
COMPILE_OPTS+=-mlittle-endian 
COMPILE_OPTS+=-ffreestanding -nostdlib -nostartfiles -nodefaultlibs
# ST specific definitions..
COMPILE_OPTS+=-DUSE_USB_OTG_FS
COMPILE_OPTS+=-DUSE_STDPERIPH_DRIVER # stm32f4xx.h uses stm32f4xx_conf.h (must be Std../inc/devices..)
COMPILE_OPTS+=-DHSE_VALUE=12000000
COMPILE_OPTS+=-DARM_MATH_CM4=1
#COMPILE_OPTS+=-D__FPU_USED=1
COMPILE_OPTS+=-D__FPU_PRESENT=1

export COMPILE_OPTS


OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c)) 
INCLUDE_DIRS = -I ./ 

# Linker File (Device specific)
LinkerFile = stm32_flash.ld
#LinkerFile = stm32_flashld.ld
OBJECTS +=startup_stm32f40xx.o



# Include ST stdPeriph Library

INCLUDE_DIRS += -I Libraries/STM32F4xx_StdPeriph_Driver/inc
#INCLUDE_DIRS += -I Libraries/STM32F4xx_StdPeriph_Driver/inc/device_support
#INCLUDE_DIRS += -I Libraries/STM32F4xx_StdPeriph_Driver/inc/core_support
#INCLUDE_DIRS += -I Libraries/CMSIS/core_support

INCLUDE_DIRS += -I Libraries/CMSIS/Include
INCLUDE_DIRS += -I Libraries/CMSIS/Device

#INCLUDE_DIRS += -I Libraries/STM32F4xx_StdPeriph_Driver/inc/Device
#INCLUDE_DIRS += -I Libraries/STM32F4xx_StdPeriph_Driver/inc/Include


LIBS=  -L Libraries/STM32F4xx_StdPeriph_Driver   -lSTM32F4xx_StdPeriph_Driver
LIBS+= -L Libraries/CMSIS   -lCMSIS_DSP
LIBS+= -lm -lc    
LIBS+= -lgcc -lg -lnosys


# USB Core Device Driver
INCLUDE_DIRS += -I Libraries/STM32_USB_Device_Library/Core/inc
OBJECTS+=$(patsubst %.c,%.o,$(wildcard Libraries/STM32_USB_Device_Library/Core/src/*.c))

# USB CDC Class Device driver
INCLUDE_DIRS += -I Libraries/STM32_USB_Device_Library/Class/cdc/inc
OBJECTS+=$(patsubst %.c,%.o,$(wildcard Libraries/STM32_USB_Device_Library/Class/cdc/src/*.c))

# USB OTG Driver
INCLUDE_DIRS += -I Libraries/STM32_USB_OTG_Driver/inc
OBJECTS+=$(patsubst %.c,%.o,$(wildcard Libraries/STM32_USB_OTG_Driver/src/*.c))

# STM32F4 Discovery Utilities
INCLUDE_DIRS += -I Libraries/Utilities/STM32F4-Discovery
OBJECTS+=$(patsubst %.c,%.o,$(wildcard Libraries/Utilities/STM32F4-Discovery/*.c))

# FatFs
INCLUDE_DIRS += -I FatFs/inc
OBJECTS+=$(patsubst %.c,%.o,$(wildcard FatFs/src/*.c))
OBJECTS+=$(patsubst %.c,%.o,$(wildcard FatFs/src/option/*.c))


MAIN_OUT = main
MAIN_OUT_ELF = $(MAIN_OUT).elf
MAIN_OUT_BIN = $(MAIN_OUT).bin
MAIN_OUT_LST = $(MAIN_OUT).lst 

CC = $(ARMGCC_PATH)arm-none-eabi-gcc
CFLAGS = $(COMPILE_OPTS) $(INCLUDE_DIRS) -Wl,-T,$(LinkerFile) -Wl,--gc-sections 
AS = $(ARMGCC_PATH)arm-none-eabi-gcc
ASFLAGS = -g0 -Wall -mcpu=cortex-m4 -mthumb -c 
LD = $(ARMGCC_PATH)arm-none-eabi-gcc
#LDFLAGS = --gc-sections -Map=$@.map,-cref,-u,Reset_Handler $(INCLUDE_DIRS) $(LIBRARY_DIRS) -T$(LinkerFile) -nostartfiles
#LDFLAGS =   --gc-sections -v  $(INCLUDE_DIRS) $(LIBRARY_DIRS) -T$(LinkerFile)
LDFLAGS = $(CFLAGS) #-Map=$@.map  -T$(LinkerFile) -nostartfiles
 

OBJCP = $(ARMGCC_PATH)arm-none-eabi-objcopy
OBJCPFLAGS = -O binary 
OBJDUMP = $(ARMGCC_PATH)arm-none-eabi-objdump
SIZE = $(ARMGCC_PATH)arm-none-eabi-size 

#STLINK=1
OPENOCD=1
#TELNET=1
COMMANDS = -c init -c targets -c "reset halt" -c "flash write_image erase $(MAIN_OUT_ELF)" -c "verify_image $(MAIN_OUT_ELF)" -c "reset run" -c shutdown

 

#all: $(MAIN_OUT_ELF) $(MAIN_OUT_BIN)
all: $(MAIN_OUT_BIN)
	

$(MAIN_OUT_BIN): $(MAIN_OUT_ELF)
	$(OBJCP) $(OBJCPFLAGS) $< $@
	$(OBJDUMP) -h -S -C -r $< > $(MAIN_OUT_LST)
	$(SIZE) $(MAIN_OUT_ELF)
		
$(MAIN_OUT_ELF): $(OBJECTS)
	$(MAKE) -C Libraries/CMSIS
	$(MAKE) -C Libraries/STM32F4xx_StdPeriph_Driver
	$(CC) $(CFLAGS) -Wl,-Map=$@.map $^ -o $@   $(LIBS)
	#$(LD) $(LDFLAGS)  $^ -o $@   $(LIBS)
	
# flash

flash: $(MAIN_OUT_BIN)

ifdef STLINK
	$(STlinkPath)/st-flash write $(MAIN_OUT_BIN) 0x8000000
endif
ifdef OPENOCD
	#openocd -f board/stm32f4discovery.cfg  $(COMMANDS)
	openocd -f stm32f4discoveryRESETNONE.cfg  $(COMMANDS)
endif

ifdef TELNET 
	(echo "reset halt; flash write_image erase $(MAIN_OUT_ELF); verify_image $(MAIN_OUT_ELF); reset run; exit")|nc localhost 4444
endif


clean:
	-rm -f $(OBJECTS)
	-rm -f $(MAIN_OUT_ELF) $(MAIN_OUT_BIN)

cleanall:
	$(MAKE) -C Libraries/STM32F4xx_StdPeriph_Driver clean
	$(MAKE) -C Libraries/CMSIS clean
	-rm -f $(OBJECTS)
	-rm -f $(MAIN_OUT_ELF) $(MAIN_OUT_BIN)

