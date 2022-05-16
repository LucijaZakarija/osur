/*! Interrupt handling - 'arch' layer(only basic operations) */

#define _ARCH_INTERRUPTS_C_
#include "interrupt.h"

#include <arch/processor.h>
#include <kernel/errno.h>
#include <lib/list.h>
#include <kernel/memory.h>

/*! Interrupt controller device */
extern arch_ic_t IC_DEV;
static arch_ic_t *icdev = &IC_DEV;

/*! interrupt handlers */
static list_t ihandlers[INTERRUPTS];

struct ihndlr
{
        int prio; //DODANO
	void *device;
	int (*ihandler)(unsigned int, void *device);

	list_h list;
};

struct zahtjev //DODAN
{
	struct ihndlr *handler;
	bool obrada_u_tijeku;

	//list_h list;
};


/*! Initialize interrupt subsystem(in 'arch' layer) */
void arch_init_interrupts()
{
	int i;

	icdev->init();

	for (i = 0; i < INTERRUPTS; i++)
		list_init(&ihandlers[i]);
}

/*!
 * enable and disable interrupts generated outside processor, controller by
 * interrupt controller(PIC or APIC or ...)
 */
void arch_irq_enable(unsigned int irq)
{
	icdev->enable_irq(irq);
}
void arch_irq_disable(unsigned int irq)
{
	icdev->disable_irq(irq);
}

/*! Register handler function for particular interrupt number */ MAKNUTO
/*void arch_register_interrupt_handler(unsigned int inum, void *handler,
				       void *device)
{
	struct ihndlr *ih;

	if (inum < INTERRUPTS)
	{
		ih = kmalloc(sizeof(struct ihndlr));
		ASSERT(ih);

		ih->device = device;
		ih->ihandler = handler;

		list_append(&ihandlers[inum], ih, &ih->list);
	}
	else {
		LOG(ERROR, "Interrupt %d can't be used!\n", inum);
		halt();
	}
}*/

void arch_register_interrupt_handler(unsigned int inum, void *handler, //DODANO
				       void *device, int prio)
{
	struct ihndlr *ih;

	if (inum < INTERRUPTS)
	{
		ih = kmalloc(sizeof(struct ihndlr));
		ASSERT(ih);
                ih->prio=prio;
		ih->device = device;
		ih->ihandler = handler;

		list_append(&ihandlers[inum], ih, &ih->list);
	}
	else {
		LOG(ERROR, "Interrupt %d can't be used!\n", inum);
		halt();
	}
}


//void arch_register_interrupt_handler(unsigned int inum, void *handler, void *device, int prio) TODO

/*! Unregister handler function for particular interrupt number */
void arch_unregister_interrupt_handler(unsigned int irq_num, void *handler,
					 void *device)
{
	struct ihndlr *ih, *next;

	ASSERT(irq_num >= 0 && irq_num < INTERRUPTS);

	ih = list_get(&ihandlers[irq_num], FIRST);

	while (ih)
	{
		next = list_get_next(&ih->list);

		if (ih->ihandler == handler && ih->device == device)
			list_remove(&ihandlers[irq_num], FIRST, &ih->list);

		ih = next;
	}
}

/*!
 * "Forward" interrupt handling to registered handler
 * (called from interrupts.S)
 */
void arch_interrupt_handler(int irq_num)
{
	struct ihndlr *ih;

	if (irq_num < INTERRUPTS && (ih = list_get(&ihandlers[irq_num], FIRST)))
	{
		/* enable interrupts on PIC immediately since program may not
		 * return here immediately */
		if (icdev->at_exit)
			icdev->at_exit(irq_num);

		/* Call registered handlers */
		while (ih)
		{
			ih->ihandler(irq_num, ih->device);

			ih = list_get_next(&ih->list);
		}
	}

	else if (irq_num < INTERRUPTS)
	{
		LOG(ERROR, "Unregistered interrupt: %d - %s!\n",
		      irq_num, icdev->int_descr(irq_num));
		halt();
	}
	else {
		LOG(ERROR, "Unregistered interrupt: %d !\n", irq_num);
		halt();
	}
}



void arch_interrupt_handler(int irq_num)
{
	struct ihndlr *ih;

	if (irq_num < INTERRUPTS && (ih = list_get (&ihandlers[irq_num], FIRST)))
	{
	             if (icdev->at_exit)
			icdev->at_exit(irq_num);
		while (ih)
		{
			//ih->ihandler(irq_num, ih->device); 
			/* umjesto poziva stvoriti objekt zahtjev i dodati ga u listu */
			struct zahtjev *zah;
			zah = kmalloc(sizeof(struct zahtjev));
			zah->handler=ih; //ok?? ili pokazivac??
			zah->obrada_u_tijeku=FALSE;
			//sto je njima lista?
			list_t *list;
			list_init(list); //ili pokazivac
			list_append(list,zah); //je li ovako??
					
		        //ASSERT(zah); //ali gdje?????

			ih = list_get_next(&ih->list);
		}

		/* dodati obradu prema prioritetima (skica ispod) */
		prvi = dohvati_prvi_iz_liste(zahtjevi) //samo dohvati ne i makni,
                while( prvi != NULL && prvi.obrada_u_tijeku == FALSE ) {
		prvi.obrada_u_tijeku = TRUE
		arch_irq_enable(irq_num); //nz
		obradi(prvi)
		arch_irq_disable(irq_num); 
		makni prvi opisnik iz liste
		kfree(prvi)
		prvi = dohvati_prvi_iz_liste(zahtjevi)
		}

/* izlaskom iz petlje su ili sve obrade gotove ili se povratkom
 * iz prekida vraćamo na nastavak obrade nekog prekida (koji je prvi u listi) */
	}
	
       else if (irq_num < INTERRUPTS)
	{
		LOG(ERROR, "Unregistered interrupt: %d - %s!\n",
		      irq_num, icdev->int_descr(irq_num));
		halt();
	}
	else {
		LOG(ERROR, "Unregistered interrupt: %d !\n", irq_num);
		halt();
	}
}
