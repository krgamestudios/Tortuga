#include "delta.h"
#include "sprite_sheet.h"
#include "image.h"
#include "surface_manager.h"

#include "SDL/SDL.h"

#include <exception>
#include <iostream>
#include <ctime>
#include <vector>

using namespace std;

int go(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Surface* screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);

	SurfaceManager sMgr;

	//load all resources
	sMgr.Load("player", "rsc/graphics/sprites/elliot2.bmp");
	sMgr.Load("flower", "rsc/graphics/sprites/aniflower.bmp");
	sMgr.Load("tileset", "rsc/graphics/tilesets/MishMash.bmp");

	SpriteSheet player(sMgr.Get("player"), 32, 48);
	Image tiles(sMgr.Get("tileset"));

	player.SetInterval(200);

	vector<SpriteSheet> flowerVector;
	for (int i = 0; i < 100; i++) {
		SpriteSheet ss(sMgr.Get("flower"), 32, 32);
		ss.SetInterval(i%5 + 95);
		flowerVector.push_back(ss);
	}

	Delta delta;

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
						case SDLK_1:
							player.SetCurrentStrip(0);
						break;
						case SDLK_2:
							player.SetCurrentStrip(1);
						break;
						case SDLK_3:
							player.SetCurrentStrip(2);
						break;
						case SDLK_4:
							player.SetCurrentStrip(3);
						break;
					}
			}
		}

		delta.Calculate();

		player.Update(delta.GetDelta());

		for (int i = 0; i < 100; i++) {
			flowerVector[i].Update(delta.GetDelta());
		}

		SDL_FillRect(screen, 0, 0);

		tiles.DrawTo(screen, 0, 0);
		player.DrawTo(screen, 0, 0);

		for (int i = 0; i < 100; i++) {
			flowerVector[i].DrawTo(screen, 20+i*4, i*4);
		}

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