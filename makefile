#output
export OUTDIR=..
export OUT=$(addprefix $(OUTDIR)/,libcommon.a)

all: $(OUTDIR)
	$(MAKE) -C debugging
	$(MAKE) -C gameplay
	$(MAKE) -C graphics
	$(MAKE) -C map
	$(MAKE) -C network
	$(MAKE) -C ui
	$(MAKE) -C utilities

debug: export CXXFLAGS+=-g
debug: clean all

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
ifeq ($(OS),Windows_NT)
	$(RM) *.o *.a *.exe
else ifeq ($(shell uname), Linux)
	find . -type f -name *.o -exec rm -f -r -v {} \;
	find . -type f -name *.a -exec rm -f -r -v {} \;
	rm -f -v out/client out/server
endif

rebuild: clean all
