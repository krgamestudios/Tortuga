#config
CXXFLAGS+=-std=c++11 -DDEBUG
LIB=-lmingw32 -lSDLmain -lSDL -lwsock32 -lWS2_32

#source
SRC=base_scene.cpp \
    scene_manager.cpp \
    test_systems.cpp \
    splash.cpp \
    main_menu.cpp \
    in_game.cpp \
    lobby.cpp \
    combat.cpp \
    surface_manager.cpp \
    image.cpp \
    sprite_sheet.cpp \
    player.cpp \
    player_manager.cpp \
    config_utility.cpp \
    network.cpp \
    network_tcp.cpp \
    main.cpp

#objects
OBJDIR=obj
OBJ=$(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))

#output
OUTDIR=out
OUT=$(addprefix $(OUTDIR)/,a)

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
	$(CXX) $(CXXFLAGS) -c -o $(@) $<

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all

unit:
