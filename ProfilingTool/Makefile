

.PHONY: clean

RM = rm -f

profile: profile.o strace.o pidstat.o
	gcc -o profile profile.o strace.o pidstat.o

profile.o: profile.c strace.h pidstat.h
	gcc -Wall -g -c profile.c

strace.o: strace.c strace.h
	gcc -Wall -g -c strace.c

pidstat.o: pidstat.c pidstat.h
	gcc -Wall -g -c pidstat.c

clean:
	$(RM) *.o
