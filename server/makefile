#config
INCLUDES+=. accounts characters combat enemies mapgen mapgen/generators rooms ../common/debugging ../common/gameplay ../common/map ../common/network ../common/network/packet ../common/network/serial ../common/utilities
LIBS+=server.a ../libcommon.a -lSDL_net -lwsock32 -liphlpapi -lmingw32 -lSDLmain -lSDL -llua -lsqlite3
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
	$(MAKE) -C combat
	$(MAKE) -C enemies
	$(MAKE) -C rooms
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
