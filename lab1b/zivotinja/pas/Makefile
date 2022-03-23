include $(POCETNI_DIREKTORIJ)/config.ini

VAR1 = 1
VAR2 = 1
CFLAGS += -D VAR1=$(VAR1) -D VAR2=$(VAR2)

OBJEKTI = vau.o grr.o
MODULI = pudlica/mod-pudlica.o 
MODUL = mod-pas.o

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
	$(MAKE) -C pudlica brisi
	rm -f $(OBJEKTI) $(MODUL)
