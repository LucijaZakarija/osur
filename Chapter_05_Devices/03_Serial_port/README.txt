1. pozvala sam funkciju iz glavnog programa
2.u stdio.c sam nadodala funkciju wipe
3.u stdio.h sam nadodala definiciju funkcije
4. u fs.c sam napisala implementaciju
5. u fs.h dodala opis

fs.c:

uzela sam dio koda iz open u funkciji k_fs_wipe

dosla sam do tfd i mogu procitati pravo ime filea, njegovu velicinu...
napravila sam direktan write u buffer u zadnjem bloku filea, tj da tamo pise x
nisam uspjela koristiti svoju funkciju read_write jer u njoj ne mogu "prepisati" blok nego samo nadopisati kako iteriram po feee[] polju

