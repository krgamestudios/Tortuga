#config
CXXFLAGS+=-std=c++11 -DDEBUG
LIB=-lmingw32 -lSDL_net -lSDLmain -lSDL -lwsock32 -liphlpapi

#source
SRC=$(filter-out unit.cpp main.cpp, $(wildcard *.cpp))

#objects
OBJDIR=obj
OBJ=$(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))

#output
OUTDIR=out

#targets
all: $(OBJ) $(OUT)
	$(CXX) $(CXXFLAGS) -o $(OUTDIR)/server main.cpp $(OBJ) $(LIB)

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

rebuild: clean all unit

unit: $(OBJ) $(OUT)
	$(CXX) $(CXXFLAGS) -o $(OUTDIR)\unit unit.cpp $(OBJ) $(LIB)