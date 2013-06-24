OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C libs
	$(MAKE) -C server
	$(MAKE) -C client
	$(MAKE) -C editor
	$(MAKE) -C test

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
