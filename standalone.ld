/******************************************************************************
 *
 * watchdog.ld - Linker configuration file for watchdog.
 *
 * Copyright (c) 2013-2014 Texas Instruments Incorporated.  All rights reserved.
 * Software License Agreement
 * 
 * Texas Instruments (TI) is supplying this software for use solely and
 * exclusively on TI's microcontroller products. The software is owned by
 * TI and/or its suppliers, and is protected under applicable copyright
 * laws. You may not combine this software with "viral" open-source
 * software in order to form a larger program.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 * NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
 * NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 * 
 * This is part of revision 2.1.0.12573 of the EK-TM4C1294XL Firmware Package.
 *
 *****************************************************************************/

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00100000
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000
}

SECTIONS
{
    /DISCARD/ :
    {
	*(.ARM.exidx)
	*(.ARM.exidx.*)
    }
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        _devid_start = ALIGN(4);
        __init_array_start = .;
        KEEP(*(.init_array))
        __init_array_end = .;
        sys_threads = .;
        KEEP(*(.threads*))
        sys_threads_end = .;
        *(.rodata*)
        __sh_commands_start = ALIGN(4);
        KEEP(*(SORT(.commands)))
        KEEP(*(.command..a.*))
        KEEP(*(.commands_end))
        KEEP(*(SORT_BY_NAME(.command.*.*)))
        _etext = .;
    } > FLASH

    .data  : AT(ADDR(.text) + SIZEOF(.text))
    {
        _data = .;
        *(vtable)
        *(.data*)
        _lock_list = .;
        KEEP(*(.locks*))
        _lock_list_end = .;
        _recursive_lock_list = .;
        KEEP(*(.recursive_locks*))
        _recursive_lock_list_end = .;
        _edata = .;
    } > SRAM

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM
}
