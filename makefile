OUTDIR=out

all: $(OUTDIR)
	$(MAKE) -C src

debug: export CXXFLAGS+=-g
debug: clean all

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
