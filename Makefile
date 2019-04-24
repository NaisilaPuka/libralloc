
all: libralloc.a  app experiment

libralloc.a:  ralloc.c
	gcc -Wall -c ralloc.c
	ar -cvq libralloc.a ralloc.o
	ranlib libralloc.a

app: app.c
	gcc -Wall -o app app.c -L. -lralloc -lpthread

experiment: experiment.c
	gcc -Wall -o exp experiment.c -L. -lralloc -lpthread

clean: 
	rm -fr *.o *.a *~ a.out  app ralloc.o ralloc.a libralloc.a exp
