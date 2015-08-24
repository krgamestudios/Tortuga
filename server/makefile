#include directories
INCLUDES+=. accounts characters clients entities rooms server_utilities triggers ../common/debugging ../common/gameplay ../common/map ../common/network ../common/network/packet_types ../common/utilities

#libraries
#the order of the $(LIBS) is important, at least for MinGW
LIBS+=server.a ../common/libcommon.a -lSDL2_net
ifeq ($(OS),Windows_NT)
	LIBS+=-lwsock32 -liphlpapi -lmingw32
endif
LIBS+=-lSDL2main -lSDL2 -llua -lsqlite3
ifeq ($(shell uname), Linux)
	#I don't know what this does, but Ubuntu needs it (dynamic linking for lua)
	LIBS+=-ldl
endif

#flags
CXXFLAGS+=-std=c++11 $(addprefix -I,$(INCLUDES))

#source
CXXSRC=$(wildcard *.cpp)

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CXXSRC:.cpp=.o))

#output
OUTDIR=../out
OUT=$(addprefix $(OUTDIR)/,server)

#targets
all: $(OBJ) $(OUT)
	$(MAKE) -C accounts
	$(MAKE) -C characters
	$(MAKE) -C clients
	$(MAKE) -C entities
#	$(MAKE) -C monsters
	$(MAKE) -C rooms
	$(MAKE) -C server_utilities
	$(MAKE) -C triggers
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
