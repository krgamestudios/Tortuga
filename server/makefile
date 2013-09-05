#config
COMMONDIR+=../common
COMMON+=../libcommon.a
LIB+=$(COMMON) -lmingw32 -lSDLmain -lSDL -llua -lsqlite3
CXXFLAGS+=-std=c++11 -DDEBUG $(addprefix -I,$(COMMONDIR))
CFLAGS+=-DDEBUG $(addprefix -I,$(COMMONDIR))

#source
CXXSRC=$(wildcard *.cpp)
CSRC=$(wildcard *.c)

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CXXSRC:.cpp=.o))
OBJ+=$(addprefix $(OBJDIR)/,$(CSRC:.c=.o))

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

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
