#config
INCLUDES=
CFLAGS+=$(addprefix -I,$(INCLUDES))
LIB=

#source
SRC=$(wildcard *.c)

#objects
OBJDIR=obj
OBJ=$(addprefix $(OBJDIR)/,$(SRC:.c=.o))

#output
OUTDIR=../out
OUT=$(addprefix $(OUTDIR)/,libSDL_net.a)

#targets
all: $(OBJ) $(OUT)
	ar -crs $(OUT) $(OBJ)

$(OBJ): | $(OBJDIR)

$(OUT): | $(OUTDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all