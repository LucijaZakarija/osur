#include <types/basic.h>

void premjesti() //sredi tocne izraze i var
{
	extern char size_data;
	extern char size_bss;
	extern char size_i;
	extern char nakon_koda;
	extern char nakon_instrukcija;
	extern char nakon_data;
	extern char bss_start;
	extern char data_start;
	//extern size_t size_c;
	size_t size_d = (size_t) &size_i;
	size_t i;
	char *od = (char *) &nakon_koda;
	char *kamo = (char *) 0x200000;

	//for ( i = 0; i< size_d; i++ )
	//	kamo[i] = od[i];

	for ( i = 0; i< size_d; i++ )
		*kamo++ = *od++;

	od = (char *) &nakon_instrukcija;
	kamo = (char *) &data_start;
	size_d = (size_t) &size_data;

	for ( i = 0; i< size_d; i++ )
		*kamo++ = *od++;
		
		
     	od = (char *) &nakon_data;
	kamo = (char *) &bss_start;
	size_d = (size_t) &size_bss;

	for ( i = 0; i< size_d; i++ )
		*kamo++ = *od++;
}
