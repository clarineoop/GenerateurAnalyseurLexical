CC = gcc
CFLAGS  = -g -Wall

default: gal

gal:  utils.o ast.o fa.o gal.o 
	$(CC) $(CFLAGS) -o gal utils.o ast.o fa.o gal.o

utils.o:  utils.c
	$(CC) $(CFLAGS) -c utils.c

ast.o:  ast.c ast.h 
	$(CC) $(CFLAGS) -c ast.c

fa.o:  fa.c fa.h 
	$(CC) $(CFLAGS) -c fa.c

gal.o:  gal.c utils.h ast.h fa.h
	$(CC) $(CFLAGS) -c gal.c

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) gal *.o *~