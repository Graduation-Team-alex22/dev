/*******************************************************************************
# Copyright (c) 2012 Christopher James Huff
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#******************************************************************************/

MEMORY
{
  /*RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 128K*/
  
  
  
    /* 1 MB flash */     
    flash (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
    /* 64 kB core-coupled SRAM */
    ccm (xrw) : ORIGIN = 0x10000000, LENGTH = 64K
    /* 112 + 16 kB SRAM */
    sram (w!rx) : ORIGIN = 0x20000000, LENGTH = 128K
    bkpsram (w!rx) : ORIGIN = 0x40024000, LENGTH = 4K
}
sram_top = ORIGIN(sram) + LENGTH(sram);
/*ccm_top = ORIGIN(ccm) + LENGTH(ccm);*/
/* _estack = 0x2001c000;*/ /* end of 112 kB SRAM (leaving sram2 free) */
 _estack = 0x20020000; /* end of 128 kB SRAM */
/*_estack = 0x10010000; /* end of 64 kB CCM SRAM */
  

ENTRY(Reset_Handler)

SECTIONS
{
    .isr_vector : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > flash
    
    .text : {
        __ctors_start__ = .;
        KEEP(SORT(*)(.init_array))
        __ctors_end__ = .;
        
        __dtors_start__ = .;
        __dtors_end__ = .;
        
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)
        
        *(.glue_7)
        *(.glue_7t)
        . = ALIGN(4);
    } > flash
    
    __exidx_start = .;
    .ARM.exidx : {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > flash
    __exidx_end = .;
    
    .text.align : {
        . = ALIGN(8);
        _etext = .;
        _sidata = .;
    } > flash
    
    .data : AT (_sidata) {
        . = ALIGN(4);
        _sdata = .;
        KEEP(*(.data))
        KEEP(*(.data*))
        . = ALIGN(4);
        _edata = .;
    } > sram
    
    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > sram
    
    . = ALIGN(4);
    end = .;
    _stack_top = sram_top;

    /* discard debugging information from the standard libraries */
    DISCARD : {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
    
} /* SECTIONS */
