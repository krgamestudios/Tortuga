#for use on Windows:

#MKDIR=mkdir
#RM=del /y

CXXFLAGS+=-static-libgcc -static-libstdc++

export

OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C utils
	$(MAKE) -C src

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
