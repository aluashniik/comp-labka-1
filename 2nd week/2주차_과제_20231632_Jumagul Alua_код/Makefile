CC	=	gcc
CFLAGS	=	-g	-c
TARGET	=	program

$(TARGET):	main.o	war.o
				$(CC)	-o	$(TARGET)	main.o	war.o

main.o:	main.c	war.h
				$(CC)	$(CFLAGS)	main.c

war.o:	war.c	war.h
				$(CC)	$(CFLAGS)	war.c

clean:
				rm	-f	*.o	$(TARGET)

