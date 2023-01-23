compiler := gcc;

ipc: inf151736_inf151756_s.o inf151736_inf151756_k.o
	gcc inf151736_inf151756_s.o -o serwer
	gcc inf151736_inf151756_k.o -o klient

inf151736_inf151756_s.o: inf151736_inf151756_s.c inf151736_inf151756.h
	gcc -Wall -c inf151736_inf151756_s.c

inf151736_inf151756_k.o: inf151736_inf151756_k.c inf151736_inf151756.h
	gcc -Wall -c inf151736_inf151756_k.c

clean:
	rm *.o serwer klient