OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C libs
	$(MAKE) -C client

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
