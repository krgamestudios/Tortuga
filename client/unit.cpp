#include "image.h"
#include "surface_manager.h"

#include "SDL/SDL.h"

#include <exception>
#include <iostream>

using namespace std;

int go(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Surface* screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);

	SurfaceManager sMgr;

	//load all resources
	sMgr.Load("player", "rsc/graphics/sprites/elliot2.bmp");
	sMgr.Load("tileset", "rsc/graphics/tilesets/MishMash.bmp");

	Image player(sMgr.Get("player"));
	Image tiles(sMgr.Get("tileset"));

	bool running = true;
	while (running) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					running = false;
				break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = false;
						break;
					}
			}
		}

		SDL_FillRect(screen, 0, 0);

		tiles.DrawTo(screen, 0, 0);
		player.DrawTo(screen, 0, 0);

		SDL_Flip(screen);
	}

	SDL_Quit();
	return 0;
}

int SDL_main(int argc, char* argv[]) {
	try {
		go(argc, argv);
	}
	catch(exception& e) {
		cerr << "Error: " << e.what() << endl;
		return 1;
	}
	return 0;
}