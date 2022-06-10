/*! Hello world program */

#include "time.h"
#include "../../kernel/device.h"
#include "../../kernel/memory.h"
#include "../../kernel/fs.h"
#include <kernel/errno.h>
#include <kernel/features.h>
#include <arch/interrupt.h>
#include <arch/processor.h>
#include <api/stdio.h>
#include <api/prog_info.h>
#include <lib/string.h>
#include <stdio.h>
#include <api/prog_info.h>

int hello_world()
{
	printf("Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		 hello_world_PROG_HELP);

	printf("Hello World!\n");

#if 0	/* test escape sequence */
	printf("\x1b[20;40H" "Hello World at 40, 20!\n");
#endif

	stdio_init(); /* initialize standard input & output devices */

	k_fs_init("DISK", 512, 4096);

	int fd = open("file:test", O_CREAT | O_WRONLY, 0);
	kprintf("fd=%d\n", fd);
	int retval = write(fd, "lucy", 4);
	kprintf("retval=%d\n", retval);
	retval = close(fd);
	kprintf("retval=%d\n", retval);


	
	
	 fd = open("file:test2", O_CREAT | O_WRONLY, 0);
	kprintf("fd=%d\n", fd);
	retval = write(fd, "b", 1);
	kprintf("retval=%d\n", retval);
	retval = close(fd);
	kprintf("retval=%d\n", retval);
	
	fd = open("file:test", O_RDONLY, 0);
	kprintf("fd=%d\n", fd);
	char buff[4];
	retval = read(fd, buff, 4);
	kprintf("retval=%d\n", retval);
	kprintf("buff=%s\n", buff);
	retval = close(fd);

	fd = open("file:test2", O_RDONLY, 0);
	kprintf("fd=%d\n", fd);
	char buff2[1];
	retval = read(fd, buff2, 1);
	kprintf("retval=%d\n", retval);
	kprintf("buff2=%s\n", buff2);
	retval = close(fd);
	
	printf("\n --- file2 wipe: ---\n\n");
	retval = wipe("file:test2");
	printf("wipe retval=%d\n", retval);
	
	
	/*	
	fd = open("file:test", O_RDONLY, 0);
	kprintf("fd=%d\n", fd);

	retval = read(fd, buff, 4);
	kprintf("retval=%d\n", retval);
	kprintf("buff=%s\n", buff);
	retval = close(fd);

	fd = open("file:test2", O_RDONLY, 0);
	kprintf("fd=%d\n", fd);*/

	retval = read(fd, buff2, 1);
	kprintf("retval=%d\n", retval);
	kprintf("buff2=%s\n", buff2);
	retval = close(fd);

	return 0;
}
