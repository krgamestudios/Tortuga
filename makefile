#Windows 7:
#RM=del /y

#Windows 8.1:
#RM=del /S

OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C common
	$(MAKE) -C server
	$(MAKE) -C client

debug: export CXXFLAGS+=-g
debug: clean all

release: export CXXFLAGS+=-static-libgcc -static-libstdc++
release: clean all package

#For use on my machine ONLY
package:
	rar a -r -ep Tortuga.rar $(OUTDIR)/*.exe  $(OUTDIR)/*.dll
	rar a -r Tortuga.rar rsc/* copyright.txt README.txt

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
