CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra

all: life

life: life.o universe.o
	$(CC) -o life life.o universe.o -lncurses

life.o: life.c
	$(CC) $(CFLAGS) -o life.o -c life.c

universe.o: universe.c
	$(CC) $(CFLAGS) -o universe.o -c universe.c

clean:
	rm -f life *.o universe *.o

format:
	clang-format -i -style=file *.[ch]
