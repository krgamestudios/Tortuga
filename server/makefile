#include directories
INCLUDES+=. accounts barriers battles characters clients creatures entities inventory rooms triggers ../common/debugging ../common/global_defines ../common/network ../common/network/packet_types ../common/utilities ../TurtleGUI ../TurtleMap

#libraries
#the order of the $(LIBS) is important, at least for MinGW
LIBS+=server.a ../libcommon.a ../libturtlegui.a ../libturtlemap.a -lSDL2_net
ifeq ($(OS),Windows_NT)
	LIBS+=-lwsock32 -liphlpapi -lmingw32
endif
LIBS+=-lSDL2main -lSDL2 -lsqlite3
ifeq ($(shell uname), Linux)
	#I don't know what this does, but Ubuntu needs it (dynamic linking for lua)
	LIBS+=/usr/local/lib/liblua.a -ldl
else
	LIBS+=-llua
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
	$(MAKE) -C barriers
	$(MAKE) -C battles
	$(MAKE) -C characters
	$(MAKE) -C clients
	$(MAKE) -C creatures
	$(MAKE) -C entities
	$(MAKE) -C inventory
	$(MAKE) -C rooms
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
