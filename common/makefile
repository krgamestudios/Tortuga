#config
LIB+=
CXXFLAGS+=-std=c++11 -DDEBUG
CFLAGS+=-DDEBUG

#source
CXXSRC=$(wildcard *.cpp)
CSRC=$(wildcard *.c)

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CXXSRC:.cpp=.o))
OBJ+=$(addprefix $(OBJDIR)/,$(CSRC:.c=.o))

#output
OUTDIR=..
OUT=$(addprefix $(OUTDIR)/,libcommon.a)

#targets
all: $(OBJ) $(OUT)
	ar -crs $(OUT) $(OBJ)

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
