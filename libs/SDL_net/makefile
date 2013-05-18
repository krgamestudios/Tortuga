SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
OUT = libSDL_net.a

all: $(OBJ)
	ar -crs $(OUT) $(OBJ)

$(%.o): %.c
	$(CC) -c -o $@ $<

clean:
	$(RM) *.o *.a
