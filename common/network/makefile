#config
INCLUDES+=. packet_types ../global_defines ../utilities ../../TurtleGUI ../../TurtleMap
LIBS+=
CXXFLAGS+=-std=c++11 $(addprefix -I,$(INCLUDES))

#source
CXXSRC=$(wildcard *.cpp)

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CXXSRC:.cpp=.o))

#targets
all: $(OBJ) $(OUT)
	ar -crs $(OUT) $(OBJ)
	$(MAKE) -C packet_types

$(OBJ): | $(OBJDIR)

$(OUT): | $(OUTDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
