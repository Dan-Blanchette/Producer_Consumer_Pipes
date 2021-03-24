CC = gcc
Flags = -Werror -Wall

all: prodCon

clobber: clean
	rm -f *~\#*\#

clean:
	rm -f prodCon *.o



prodCon: producer_consumer.o prod_con.o
	$(CC)$(FLAGS) producer_consumer.o prod_con.o -o prodCon
producer_consumer.o: producer_consumer.c prod_con.h
	$(CC) -c producer_consumer.c
prod_con.o: prod_con.c prod_con.h
	$(CC) -c prod_con.c