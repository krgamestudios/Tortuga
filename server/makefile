#config
INCLUDES+=. accounts characters rooms server_utilities ../common/debugging ../common/gameplay ../common/map ../common/network ../common/network/packet_types ../common/utilities

LIBS+=server.a ../libcommon.a -lSDL_net -lSDLmain -lSDL -llua -lsqlite3
ifeq ($(OS),Windows_NT)
	LIBS+=-lwsock32 -liphlpapi -lmingw32
endif

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
	$(MAKE) -C rooms
	$(MAKE) -C server_utilities
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
