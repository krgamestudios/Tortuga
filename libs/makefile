OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C codebase
	$(MAKE) -C SDL_net

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
