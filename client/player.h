#ifndef PLAYER_H_
#define PLAYER_H_

#include "vector2.h"
#include "sprite_sheet.h"

#include "SDL/SDL.h"

enum class Direction {
	NORTH, SOUTH,
	EAST, WEST
};

class Player {
public:
	Player(SDL_Surface*, int w, int h);

	void Update(int delta);

	void WalkInDirection(Direction);

	Vector2 SetPosition(Vector2 v) { return position = v; }
	Vector2 ShiftPosition(Vector2 v) { return position += v; }
	Vector2 GetPosition() const { return position; };

	Vector2 SetMotion(Vector2 v) { return motion = v; }
	Vector2 ShiftMotion(Vector2 v) { return motion += v; }
	Vector2 GetMotion() const { return motion; }

	void DrawTo(SDL_Surface* s) { sprite.DrawTo(s, position.x, position.y); }
	void FaceDirection(Direction);

	SpriteSheet* GetSpriteSheet() { return &sprite; };
private:
	Vector2 position;
	Vector2 motion;
	SpriteSheet sprite;
};

#endif
