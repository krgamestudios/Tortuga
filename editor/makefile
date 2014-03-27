#config
INCLUDES+=../common ../common/graphics ../common/map ../common/ui
LIBS+=../libcommon.a -lmingw32 -lSDLmain -lSDL -llua
CXXFLAGS+=-std=c++11 -DDEBUG $(addprefix -I,$(INCLUDES))
CFLAGS+=-DDEBUG $(addprefix -I,$(INCLUDES))

#source
CXXSRC=$(wildcard *.cpp)
CSRC=$(wildcard *.c)

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CXXSRC:.cpp=.o))
OBJ+=$(addprefix $(OBJDIR)/,$(CSRC:.c=.o))

#output
OUTDIR=../out
OUT=$(addprefix $(OUTDIR)/,editor)

#targets
all: $(OBJ) $(OUT)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(OBJ) $(LIBS)

$(OBJ): | $(OBJDIR)

$(OUT): | $(OUTDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
