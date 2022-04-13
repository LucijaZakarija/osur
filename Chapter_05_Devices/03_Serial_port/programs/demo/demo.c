#include <stdio.h>
#include <api/prog_info.h>
#include <api/malloc.h>

int demo()
{

	int *polje;
	

	
	
	for(int i=0;i<=10000000;i+=1000) {
	  polje=kmalloc(sizeof(polje)+i+sizeof(int));
	  if(i%100==0) {
	  kfree(polje);
	  }
	}

	return 0;
	}

