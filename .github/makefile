CC = gcc
FLAGS = -Werror -Wall

all: prodCon

clobber: clean
	rm -f *~\#*\#

clean:
	rm -f prodCon *.o



prodCon: main.o prod_con.o
	$(CC) $(FLAGS) main.o prod_con.o -o prodCon
main.o: main.c prod_con.h
	$(CC) -c main.c
prod_con.o: prod_con.c prod_con.h
	$(CC) -c prod_con.c