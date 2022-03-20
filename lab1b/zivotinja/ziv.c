#include <stdio.h>
#include <postavke.h>


int funkcija_ziv ( const char *prefiks )
{
	 printf ( "%s -- funkcija_zivotinja %d %d\n", prefiks, VAR1, VAR2 );
	 	funkcija_vau ( "main" );
	funkcija_mjau ( prefiks );
	funkcija_vau ( prefiks );
	funkcija_pudl ( prefiks );
		return 0;
}
