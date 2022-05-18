/*! Generate segmentation fault */

#include <stdio.h>
#include <api/prog_info.h>
#include <api/malloc.h>
#include <arch/interrupt.h>
#include <arch/processor.h>
/* detect memory faults(qemu do not detect segment violations!) */


static volatile int pon=0;

static void test2(uint irqn)
{
	printf("Interrupt handler routine5: irqn=%d\n", irqn);
}
static void test3(uint irqn)
{
	printf("Interrupt handler routine3: irqn=%d\n", irqn);
}


static void test1(uint irqn)
{
pon++;
	printf("Interrupt handler routine start1: irqn=%d\n", irqn);
	if(pon<2) {
	arch_register_interrupt_handler(SOFTWARE_INTERRUPT, test3,NULL,3);
	//arch_register_interrupt_handler(INT_MEM_FAULT, test3,NULL,4);
	//raise_interrupt(INT_MEM_FAULT);
	//raise_interrupt(INT_UNDEF_FAULT);
	raise_interrupt(SOFTWARE_INTERRUPT);
	}
	printf("Interrupt handler routine end1: irqn=%d\n", irqn);
}

int segm_fault()
{


	printf("\nInterrupt test >>>\n");

	arch_register_interrupt_handler(SOFTWARE_INTERRUPT, test1,NULL,1);
	arch_register_interrupt_handler(SOFTWARE_INTERRUPT, test2,NULL,5);
	arch_register_interrupt_handler(SOFTWARE_INTERRUPT, test2,NULL,4);

	raise_interrupt(SOFTWARE_INTERRUPT);

	printf("Interrupt test <<<\n\n");
	

	void *ptr1, *ptr2;

	ptr1 = malloc(1023);
	printf("malloc returned %x(1023)\n", ptr1);

	ptr2 = malloc(123);
	printf("malloc returned %x(123)\n", ptr2);

	if (ptr1)
		free(ptr1);
	if (ptr2)
		free(ptr2);
		
		
	unsigned int *p;
	unsigned int i, j=0;

	printf("Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		 segm_fault_PROG_HELP);

	printf("Before segmentation fault\n");

	for (i = 2; i < 32; i++)
	{
		p = (unsigned int *)(1 << i);
		printf("[%x]=%d\n", p, *p);
		j+= *p;
	}

	printf("After expected segmentation fault, j=%d\n", j);

	return 0;
}
