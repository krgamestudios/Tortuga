OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C codebase
	$(MAKE) -C common

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
