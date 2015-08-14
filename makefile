#Windows 7:
#RM=del /y

#Windows 8.1:
#RM=del /S

OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C common
#	$(MAKE) -C server
	$(MAKE) -C client

debug: export CXXFLAGS+=-g
debug: clean all

release: export CXXFLAGS+=-static-libgcc -static-libstdc++
release: clean all package

#For use on my machine ONLY
package:
ifeq ($(OS),Windows_NT)
	rar a -r -ep Tortuga-win.rar $(OUTDIR)/*.exe  $(OUTDIR)/*.dll
	rar a -r Tortuga-win.rar rsc/* copyright.txt instructions.txt
else ifeq ($(shell uname), Linux)
	tar -C $(OUTDIR) -zcvf Tortuga-linux.tar client server ../rsc ../copyright.txt ../instructions.txt
endif

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
ifeq ($(OS),Windows_NT)
	$(RM) *.o *.a *.exe
else ifeq ($(shell uname), Linux)
	find . -type f -name '*.o' -exec rm -f -r -v {} \;
	find . -type f -name '*.a' -exec rm -f -r -v {} \;
	rm -f -v $(OUT)
	find . -empty -type d -delete
endif

rebuild: clean all
