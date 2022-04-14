ENTRY(arch_startup)
ROM_START = 0x100000;
RAM_START = 0x200000;
STOG_START = 0x600000;

SECTIONS {
	.code ROM_START : AT(ROM_START)
	{
		kernel_code_addr = .;

		/* instructions */
		*?/boot/startup.asm.o ( .text* )
		*?/boot/premjesti.o ( .text* )

	}
	
	size_c = SIZEOF(.code);
	nakon_koda = ROM_START + SIZEOF(.code);
	
	
	.instrukcije RAM_START : AT(nakon_koda)
	{
 		*( .text* )
 	}
 	
 	
	size_i = SIZEOF(.instrukcije);
	nakon_instrukcija = nakon_koda + size_i;
	data_start = RAM_START + size_i;
	xxxx = 0x10000;

	.data data_start : AT(nakon_instrukcija)
	{
		kernel_data_addr = .;

		/* read only data (constants), initialized global variables */
		* ( .rodata* .data* )
	}
	
	size_data = SIZEOF(.data);
	nakon_data = nakon_instrukcija + size_data;
	bss_start = data_start + size_data;
	
	
	.bss bss_start : AT(nakon_data)
	{
		*( .bss* COMMON* )

		. = ALIGN (4096);
	}
	
	
	size_bss = SIZEOF(.bss);
	nakon_svega = nakon_koda + size_data + size_bss;
	
	
	.stog_u_slici STOG_START : AT (STOG_START)
	{
		*( .stog )
		. = ALIGN (4096);
	}

	/*
	 * what with other sections generated with gcc (various versions)?
	 * if optimizing for size discard them with: /DISCARD/ : { *(*) }
	 * if debugging with qemu/gdb they must be included
	 * - they can be included implicitly, without declaring them here
	 * - but they will be put behind "kernel_end_addr" so its best to use
	 *   multiboot loader (QEMU is that)
	 */

/DISCARD/ : { *(.eh_frame) }
#ifndef DEBUG
	/DISCARD/ : { *(*) }
#endif
	kernel_end_addr = .;
}

