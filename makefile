OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C server
	$(MAKE) -C client
	$(MAKE) -C editor

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
