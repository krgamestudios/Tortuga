#config
COMMONDIR+=../common ../common/graphics ../common/map ../common/network ../common/ui
COMMON+=../libcommon.a
LIB+=$(COMMON) -lSDL_net -lwsock32 -liphlpapi -lmingw32 -lSDLmain -lSDL
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

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
