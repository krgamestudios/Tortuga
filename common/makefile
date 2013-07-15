#config
LOCALLIBS=
LIB=
INCLUDES=
CXXFLAGS+=-std=c++11 -DDEBUG $(addprefix -I,$(INCLUDES))

#source
SRC=$(wildcard *.cpp)

#objects
OBJDIR=obj
OBJ=$(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))

#output
OUTDIR=../lib
OUT=$(addprefix $(OUTDIR)/,libCommon.a)

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

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
