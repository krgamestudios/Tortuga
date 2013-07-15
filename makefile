LIBDIR=lib
OUTDIR=out

all: $(LIBDIR) $(OUTDIR)
	$(MAKE) -C common
	$(MAKE) -C server
	$(MAKE) -C client
	$(MAKE) -C test

$(LIBDIR):
	mkdir $(LIBDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
