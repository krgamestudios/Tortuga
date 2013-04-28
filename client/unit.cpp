#include "delta.h"
#include "player.h"
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

	Player player(sMgr.Get("player"), 32, 48);
	Image tiles(sMgr.Get("tileset"));
	SpriteSheet rose(sMgr.Get("flower"), 32, 32);

	player.GetSpriteSheet()->SetInterval(200);
	rose.SetInterval(200);

//	vector<SpriteSheet> flowerVector;
//	for (int i = 0; i < 100; i++) {
//		SpriteSheet ss(sMgr.Get("flower"), 32, 32);
//		ss.SetInterval(i%5 + 95);
//		flowerVector.push_back(ss);
//	}

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
						case SDLK_w:
						case SDLK_UP:
							player.WalkInDirection(Direction::NORTH);
						break;
						case SDLK_s:
						case SDLK_DOWN:
							player.WalkInDirection(Direction::SOUTH);
						break;
						case SDLK_a:
						case SDLK_LEFT:
							player.WalkInDirection(Direction::WEST);
						break;
						case SDLK_d:
						case SDLK_RIGHT:
							player.WalkInDirection(Direction::EAST);
						break;
					}
				break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym) {
						case SDLK_w:
							player.WalkInDirection(Direction::SOUTH);
						break;
						case SDLK_s:
							player.WalkInDirection(Direction::NORTH);
						break;
						case SDLK_a:
							player.WalkInDirection(Direction::EAST);
						break;
						case SDLK_d:
							player.WalkInDirection(Direction::WEST);
						break;
					}
			}
		}

		delta.Calculate();

		player.Update(delta.GetDelta());
		rose.Update(0);

//		for (int i = 0; i < 100; i++) {
//			flowerVector[i].Update(delta.GetDelta());
//		}

		SDL_FillRect(screen, 0, 0);

		tiles.DrawTo(screen, 0, 0);
		player.DrawTo(screen);
		rose.DrawTo(screen, 50, 100);

//		for (int i = 0; i < 100; i++) {
//			flowerVector[i].DrawTo(screen, 20+i*4, i*4);
//		}

		SDL_Flip(screen);

		//debugging
//		cout << player.GetSpriteSheet()->GetCurrentFrame() << endl;
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