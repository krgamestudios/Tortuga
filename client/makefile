#config
INCLUDES+=. ../common/gameplay ../common/graphics ../common/map ../common/network ../common/ui ../common/utilities
LIBS+=../libcommon.a -lSDL_net -lwsock32 -liphlpapi -lmingw32 -lSDLmain -lSDL -llua
CXXFLAGS+=-std=c++11 $(addprefix -I,$(INCLUDES)) -DGRAPHICS

#source
CXXSRC=$(wildcard *.cpp)

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CXXSRC:.cpp=.o))

#output
OUTDIR=../out
OUT=$(addprefix $(OUTDIR)/,client)

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

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
