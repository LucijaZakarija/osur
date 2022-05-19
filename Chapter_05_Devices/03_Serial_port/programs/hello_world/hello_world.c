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
	int retval = write(fd, "neki tekst ", 12);
	kprintf("retval=%d\n", retval);
	retval = close(fd);
	kprintf("retval=%d\n", retval);


	
	
	int fd2 = open("file:test2", O_CREAT | O_WRONLY, 0);
	kprintf("fd=%d\n", fd2);
	int retval2 = write(fd2, "lala", 4);
	kprintf("retval=%d\n", retval2);
	retval2 = close(fd2);
	kprintf("retval=%d\n", retval2);
	
	fd = open("file:test", O_RDONLY, 0);
	kprintf("fd=%d\n", fd);
	char buff[12];
	retval = read(fd, buff, 12);
	kprintf("retval=%d\n", retval);
	kprintf("buff=%s\n", buff);
	retval = close(fd);

	fd2 = open("file:test2", O_RDONLY, 0);
	kprintf("fd=%d\n", fd2);
	char buff2[4];
	retval2 = read(fd2, buff2, 4);
	kprintf("retval=%d\n", retval2);
	kprintf("buff2=%s\n", buff2);
	retval2 = close(fd2);

	return 0;
}
