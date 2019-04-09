
all: libralloc.a  app

libralloc.a:  ralloc.c
	gcc -Wall -c ralloc.c
	ar -cvq libralloc.a ralloc.o
	ranlib libralloc.a

app: app.c
	gcc -Wall -o app app.c -L. -lralloc -lpthread

clean: 
	rm -fr *.o *.a *~ a.out  app ralloc.o ralloc.a libralloc.a
