#ifndef SCENELIST_HPP_
#define SCENELIST_HPP_

enum class SceneList {
	//these are reserved
	QUIT, //doubles as "up one scene" in nested scenes
	CONTINUE,
	FIRST,

	//custom indexes
	TESTSYSTEMS,
	SPLASH,
	MAINMENU,
	INGAME,

	//subscenes
	LOBBY,
	COMBAT,
};

#endif
