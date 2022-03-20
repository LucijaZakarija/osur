#include <stdio.h>
#include <string.h>
#include <math.h>
#include <postavke.h>


int funkcija_vau ( const char *prefiks )
{
	char p[DULJINA];

	strcpy ( p, prefiks );
	strcat ( p, " -- funkcija_vau" );

	printf ( "%s %d %g\n", p, VAR1, sqrt(VAR2) );

	funkcija_pudl ( p );
	return 0;
}
