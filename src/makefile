#include directories
INCLUDES+=. ../common/graphics ../common/map

#libraries
#the order of the $(LIBS) is important, at least for MinGW
LIBS+=../common/libcommon.a
ifeq ($(OS),Windows_NT)
	LIBS+=-lmingw32
endif
LIBS+=-lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -llua
ifeq ($(shell uname), Linux)
	#I don't know what this does, but Ubuntu needs it (dynamic linking for lua)
	LIBS+=-ldl
endif

#flags
CXXFLAGS+=-std=c++11 $(addprefix -I,$(INCLUDES))
ifeq ($(shell uname), Linux)
	#read data about the current install
	CXXFLAGS+=$(shell sdl-config --cflags --static-libs)
endif

#source
CXXSRC=$(wildcard *.cpp)

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CXXSRC:.cpp=.o))

#output
OUTDIR=../out
OUT=$(addprefix $(OUTDIR)/,map)

#targets
all: $(OBJ) $(OUT)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(OBJ) $(LIBS)

$(OBJ): | $(OBJDIR)

$(OUT): | $(OUTDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
ifeq ($(OS),Windows_NT)
	$(RM) *.o *.a *.exe
else ifeq ($(shell uname), Linux)
	find . -type f -name '*.o' -exec rm -f -r -v {} \;
	find . -type f -name '*.a' -exec rm -f -r -v {} \;
	rm -f -v $(OUT)
endif

rebuild: clean all
