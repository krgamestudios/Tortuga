OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C SDL_net
	$(MAKE) -C codebase

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
