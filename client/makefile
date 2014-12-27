#include directories
INCLUDES+=. client_utilities entities scenes ../common/debugging ../common/gameplay ../common/graphics ../common/map ../common/network ../common/network/packet_types ../common/ui ../common/utilities

#libraries
#the order of the $(LIBS) is important, at least for MinGW
LIBS+=client.a ../libcommon.a -lSDL_net
ifeq ($(OS),Windows_NT)
	LIBS+=-lwsock32 -liphlpapi -lmingw32
endif
LIBS+=-lSDLmain -lSDL

#flags
CXXFLAGS+=-std=c++11 $(addprefix -I,$(INCLUDES))

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
	$(MAKE) -C scenes
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
	$(RM) *.o *.a *.exe

rebuild: clean all
