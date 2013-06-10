#config
LIBDIR=../libs
LOCALLIBS=$(LIBDIR)/out/libCodebase.a $(LIBDIR)/out/libSDL_net.a
LIB=$(LOCALLIBS) -lwsock32 -liphlpapi -lmingw32 -lSDLmain -lSDL
INCLUDES=$(LIBDIR)/Codebase $(LIBDIR)/SDL_net ../common
CXXFLAGS+=-std=c++11 -DDEBUG $(addprefix -I,$(INCLUDES))

#source
SRC=$(wildcard *.cpp)

#objects
OBJDIR=obj
OBJ=$(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))

#output
OUTDIR=../out
OUT=$(addprefix $(OUTDIR)/,server)

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
