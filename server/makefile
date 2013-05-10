#config
CXXFLAGS+=-std=c++11 -DDEBUG
LIB=-lwsock32 -lWS2_32

#objects
OBJDIR=obj
OBJ=$(addprefix $(OBJDIR)/,config_utility.o network.o network_tcp.o)

#output
OUTDIR=out
OUT=$(addprefix $(OUTDIR)/,server)

#source
SRC=server.cpp main.cpp

#targets
all: $(OBJ) $(OUT)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC) $(OBJ) $(LIB)

$(OBJ): | $(OBJDIR)

$(OUT): | $(OUTDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $(@) $<

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all

unit:
	$(CXX) $(CXXFLAGS) -o $(OUTDIR)\unit unit.cpp $(OBJ) $(LIB)