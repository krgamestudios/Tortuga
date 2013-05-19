#config
INCLUDES=../common
CXXFLAGS+=-std=c++11 -DDEBUG $(addprefix -I,$(INCLUDES))
LIB=../common/out/common.a -lmingw32 -lSDL_net -lSDLmain -lSDL -lwsock32 -liphlpapi

#source
SRC=$(filter-out unit.cpp, $(wildcard *.cpp))

#objects
OBJDIR=obj
OBJ=$(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))

#output
OUTDIR=out
OUT=$(addprefix $(OUTDIR)/,client)

#targets
all: $(OBJ) $(OUT)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(OBJ) $(LIB)

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
