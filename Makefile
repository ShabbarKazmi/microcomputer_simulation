CC=gcc

CFLAGS = -c -Wall
LDFLAGS = -lm
DEPS = $(wildcard *.h)
SOURCES = $(wildcard *.c)
OBJ = $(SOURCES:.c=.o)
EXEC = Project1

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *o $(EXEC)