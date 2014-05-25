#config
INCLUDES+=. .. ../../common ../../common/gameplay ../../common/graphics ../../common/map ../../common/network ../../common/ui ../../common/utilities
LIBS+=libclient.a ../libcommon.a -lSDL_net -lwsock32 -liphlpapi -lmingw32 -lSDLmain -lSDL -llua
CXXFLAGS+=-std=c++11 -DDEBUG $(addprefix -I,$(INCLUDES))

#source
CXXSRC=$(wildcard *.cpp)

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CXXSRC:.cpp=.o))

#output
OUTDIR=..
OUT=$(addprefix $(OUTDIR)/,libclient.a)

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
