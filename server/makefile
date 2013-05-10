#config
CXXFLAGS+=-std=c++11 -DDEBUG
LIB=-lmingw32 -lSDLmain -lSDL -lwsock32 -liphlpapi

#objects
OBJDIR=obj
OBJ=$(addprefix $(OBJDIR)/,main.o config_utility.o)

#output
OUTDIR=out
OUT=$(addprefix $(OUTDIR)/,a)

#source
SRC=server.cpp

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
