CXXFLAGS+=-std=c++11
DFLAGS=-DDEBUG
LIB=-lmingw32 -lSDL_net -lSDLmain -lSDL -lwsock32 -liphlpapi
OBJ=
SRC=server.cpp

all: debug

release: $(OBJ)
	$(CXX) $(CXXFLAGS) $(SRC) $(OBJ) $(LIB)

debug: $(OBJ)
	$(CXX) $(CXXFLAGS) $(DFLAGS) $(SRC) $(OBJ) $(LIB)

clean:
	-$(RM) *.o *.a *.exe

unit:
	$(CXX) $(CXXFLAGS) unit.cpp
