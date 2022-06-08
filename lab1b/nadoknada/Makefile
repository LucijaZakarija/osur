include $(POCETNI_DIREKTORIJ)/config.ini


CFLAGS += -D VAR1=$(VAR1) -D VAR2=$(VAR2)

OBJEKTI = lab1xx.o
MODUL = mod-nadoknada.o


$(MODUL): $(OBJEKTI)
	$(LD) -r $^ $(LDFLAGS) -o $@

lab1xx.o: lab1xx.c $(POCETNI_DIREKTORIJ)/include/postavke.h
	$(CC) $(CFLAGS) -o $@ -c $<
	

# brisanje
.PHONY: brisi
brisi:
	rm -f $(OBJEKTI) $(MODUL)
