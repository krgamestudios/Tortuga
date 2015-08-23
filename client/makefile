#include directories
INCLUDES+=. client_utilities entities gameplay_scenes menu_scenes ../common/debugging ../common/gameplay ../common/graphics ../common/map ../common/network ../common/network/packet_types ../common/ui ../common/utilities

#libraries
#the order of the $(LIBS) is important, at least for MinGW
LIBS+=client.a ../common/libcommon.a -lSDL2_net
ifeq ($(OS),Windows_NT)
	LIBS+=-lwsock32 -liphlpapi -lmingw32
endif
LIBS+=-lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

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
OUT=$(addprefix $(OUTDIR)/,client)

#targets
all: $(OBJ) $(OUT)
	$(MAKE) -C client_utilities
	$(MAKE) -C entities
	$(MAKE) -C gameplay_scenes
	$(MAKE) -C menu_scenes
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
	find . -type f -name *.o -exec rm -f -r -v {} \;
	find . -type f -name *.a -exec rm -f -r -v {} \;
	rm -f -v out/client out/server
endif

rebuild: clean all
