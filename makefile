LIBDIR=lib
OUTDIR=out

all: $(LIBDIR) $(OUTDIR)
	$(MAKE) -C server

$(LIBDIR):
	mkdir $(LIBDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
