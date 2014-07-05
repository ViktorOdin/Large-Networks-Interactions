##
##	Victor Oudin
##	05/03/2014
##
##	Makefile TP4
##

CC=gcc
CFLAGS=-W -Wall -g
EXEC=er mc
OBJ1=er.o
OBJ2=mc.o


all: $(EXEC)

er.o: er.c
	$(CC) $(CFLAGS) -o $@ -c $<

mc.o: mc.c
	$(CC) $(CFLAGS) -o $@ -c $<

degre.o: utils/degre.c
	$(CC) $(CFLAGS) -o $@ -c $<

nbr_noeuds.o: utils/nbr_noeuds.c
	$(CC) $(CFLAGS) -o $@ -c $<

cc-uf.o: utils/cc-uf.c utils/utils.h
	$(CC) $(CFLAGS) -o $@ -c $<

ph.o: utils/ph.c utils/ensemble.h utils/utils.h
	$(CC) $(CFLAGS) -o $@ -c $<

mod.o: utils/mod.c utils/utils.h
	$(CC) $(CFLAGS) -o $@ -c $<

utils.o: utils/utils.c utils/utils.h
	$(CC) $(CFLAGS) -o $@ -c $<

er: $(OBJ1)
	$(CC) -o $@ $^

mc: $(OBJ2)
	$(CC) -o $@ $^

degre: utils/degre.o
	$(CC) -o $@ $^

nbr_noeuds: utils/nbr_noeuds.o
	$(CC) -o $@ $^

cc-uf: utils/cc-uf.o utils/utils.o utils/utils.h
	$(CC) -o $@ $^

ph: utils/ph.o utils/ensemble.o utils/utils.o utils/ensemble.h utils/utils.h
	$(CC) -o $@ $^

mod: utils/mod.o utils/utils.o utils/utils.h
	$(CC) -o $@ $^ $(LDFLAGS)

clean:	
	rm -f *.o
	rm -f *.a
	rm -f *~
	rm -f utils/*.o
	rm -f utils/*.a
	rm -f utils/*~

mrproper: clean
	rm -f cc-uf ph mod degre nbr_noeuds
	rm -f $(EXEC)

pkg: mrproper
	tar -czvf ../Victor_Oudin_Archive.tar.gz ../*

