CXXFLAGS+=-std=c++11 -DDEBUG
LIB=-lmingw32 -lSDL_net -lSDLmain -lSDL -lwsock32 -liphlpapi
OBJ=base_scene.o scene_manager.o main.o surface_manager.o image.o sprite_sheet.o player.o
SRC=test_systems.cpp in_game.cpp

all: debug

release: $(OBJ)
	$(CXX) $(CXXFLAGS) $(SRC) $(OBJ) $(LIB)

debug: $(OBJ)
	$(CXX) $(CXXFLAGS) $(SRC) $(OBJ) $(LIB)

clean:
	-$(RM) *.o *.a *.exe

rebuild: clean all

unit:
	$(CXX) $(CXXFLAGS) unit.cpp surface_manager.cpp image.cpp sprite_sheet.cpp player.cpp -lmingw32 -lSDLmain -lSDL
