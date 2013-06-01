#include "test_systems.hpp"

#include <iostream>

using namespace std;

#include "splash.hpp"

//-------------------------
//Public access members
//-------------------------

TestSystems::TestSystems(ConfigUtility* cUtil, SurfaceManager* sMgr, UDPNetworkUtility* nUtil) {
#ifdef DEBUG
	cout << "entering TestSystems" << endl;
#endif
	configUtil = cUtil;
	surfaceMgr = sMgr;
	netUtil = nUtil;

	//subscene; load the resources
	Splash* splash = new Splash(configUtil, surfaceMgr);

	while(splash->GetNextScene() == SceneList::CONTINUE) {
		//wipe the screen
		SDL_FillRect(splash->GetScreen(), 0, 0);
		//call each user defined function
		((BaseScene*)(splash))->RunFrame();
		//give the computer a break
		SDL_Delay(10);
	}
	delete splash;
	SetNextScene(SceneList::CONTINUE);

	playerCounter = currentPlayer = 0;

	playerMgr.New(playerCounter++, surfaceMgr->Get("elliot"));
	playerMgr.New(playerCounter++, surfaceMgr->Get("elliot"));
	playerMgr.New(playerCounter++, surfaceMgr->Get("coa"));
	playerMgr.New(playerCounter++, surfaceMgr->Get("coa"));

	font.SetSurface(surfaceMgr->Get("font"));
}

TestSystems::~TestSystems() {
	playerMgr.DeleteAll();
#ifdef DEBUG
	cout << "leaving TestSystems" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void TestSystems::FrameStart() {
	frameRate.Calculate();
}

void TestSystems::FrameEnd() {
	//
}

void TestSystems::Update() {
//	Delta::Calculate();
//	playerMgr.UpdateAll(Delta::GetTime());
}

string IToS(int i) {
	char buffer[20];
	memset(buffer, 0, 20);
	sprintf(buffer, "%d", i);
	return string(buffer);
}

void TestSystems::Render(SDL_Surface* const screen) {
	playerMgr.DrawAllTo(screen);
	font.DrawStringTo("FPS: " + IToS(frameRate.GetFrameRate()), screen, 16, 16);
}

//-------------------------
//Event handlers
//-------------------------

void TestSystems::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void TestSystems::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void TestSystems::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void TestSystems::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;

		case SDLK_w:
			playerMgr[currentPlayer]->WalkInDirection(Direction::NORTH);
			SendMessage("move up");
		break;
		case SDLK_s:
			playerMgr[currentPlayer]->WalkInDirection(Direction::SOUTH);
			SendMessage("move down");
		break;
		case SDLK_a:
			playerMgr[currentPlayer]->WalkInDirection(Direction::WEST);
			SendMessage("move left");
		break;
		case SDLK_d:
			playerMgr[currentPlayer]->WalkInDirection(Direction::EAST);
			SendMessage("move right");
		break;

		case SDLK_1:
			SwitchToPlayer(0);
		break;
		case SDLK_2:
			SwitchToPlayer(1);
		break;
		case SDLK_3:
			SwitchToPlayer(2);
		break;
		case SDLK_4:
			SwitchToPlayer(3);
		break;
	}
}

void TestSystems::KeyUp(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_w:
			playerMgr[currentPlayer]->WalkInDirection(Direction::SOUTH);
		break;
		case SDLK_s:
			playerMgr[currentPlayer]->WalkInDirection(Direction::NORTH);
		break;
		case SDLK_a:
			playerMgr[currentPlayer]->WalkInDirection(Direction::EAST);
		break;
		case SDLK_d:
			playerMgr[currentPlayer]->WalkInDirection(Direction::WEST);
		break;
	}
}

//-------------------------
//Utilities
//-------------------------

void TestSystems::NewPlayer(int index, std::string avatarName, int x, int y) {
	Player* p = playerMgr.New(index, surfaceMgr->Get(avatarName));
	p->SetPosition(Vector2(x, y));
}

void TestSystems::SwitchToPlayer(int index) {
	//dirty hacks for smooth movement
	playerMgr[currentPlayer]->SetMotion(Vector2(0,0));
	currentPlayer = index;

	Uint8* key = SDL_GetKeyState(nullptr);

	if (key[SDLK_w]) {
		playerMgr[currentPlayer]->WalkInDirection(Direction::NORTH);
	}
	if (key[SDLK_s]) {
		playerMgr[currentPlayer]->WalkInDirection(Direction::SOUTH);
	}
	if (key[SDLK_a]) {
		playerMgr[currentPlayer]->WalkInDirection(Direction::WEST);
	}
	if (key[SDLK_d]) {
		playerMgr[currentPlayer]->WalkInDirection(Direction::EAST);
	}
}

void TestSystems::SendMessage(std::string s) {
	cout << s << endl;
}