#config
CXXFLAGS+=-std=c++11 -DDEBUG
LIB=-lmingw32 -lSDLmain -lSDL -lwsock32 -liphlpapi

#objects
OBJDIR=obj
OBJ=$(addprefix $(OBJDIR)/,base_scene.o scene_manager.o main.o surface_manager.o image.o sprite_sheet.o player.o player_manager.o config_utility.o)

#output
OUTDIR=out
OUT=$(addprefix $(OUTDIR)/,a)

#source
SRC=test_systems.cpp in_game.cpp

#targets
all: $(OBJ) $(OUT)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC) $(OBJ) $(LIB)

$(OBJ): | $(OBJDIR)

$(OUT): | $(OUTDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $(@) $<

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all

unit:
