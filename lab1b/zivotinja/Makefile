include $(POCETNI_DIREKTORIJ)/config.ini


CFLAGS += -D VAR1=$(VAR1) -D VAR2=$(VAR2)

OBJEKTI = ziv.o
MODULI = macka/mod-macka.o pas/mod-pas.o 
MODUL = mod-zivotinja.o

$(MODUL): $(OBJEKTI) $(MODULI)
	$(LD) -r $^ $(LDFLAGS) -o $@ 

vau.o: vau.c $(POCETNI_DIREKTORIJ)/include/postavke.h
	$(CC) $(CFLAGS) -o $@ -c $<


.PHONY: $(MODULI)
$(MODULI):
	$(MAKE) -C $(dir $@)

# brisanje
.PHONY: brisi
brisi:
	$(MAKE) -C macka brisi
	$(MAKE) -C pas brisi
	rm -f $(OBJEKTI) $(MODUL)
