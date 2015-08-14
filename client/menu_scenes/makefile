#config
INCLUDES+=. .. ../client_utilities ../../common/graphics ../../common/map ../../common/network ../../common/network/packet_types ../../common/ui ../../common/utilities
LIBS+=
CXXFLAGS+=-std=c++11 $(addprefix -I,$(INCLUDES))

#source
CXXSRC=$(wildcard *.cpp)

#DEBUG: override the wildcard
CXXSRC=disconnected_screen.cpp options_menu.cpp splash_screen.cpp

#objects
OBJDIR=obj
OBJ+=$(addprefix $(OBJDIR)/,$(CXXSRC:.cpp=.o))

#output
OUTDIR=..
OUT=$(addprefix $(OUTDIR)/,client.a)

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
