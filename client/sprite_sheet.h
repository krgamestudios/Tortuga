#ifndef SPRITESHEET_H_
#define SPRITESHEET_H_

#include "image.h"

#include "SDL/SDL.h"

class SpriteSheet : public Image {
public:
	SpriteSheet(SDL_Surface*, int w, int h);
	virtual ~SpriteSheet() {}

	void Update(int delta);

	int SetWidth(int i) { return SetClipW(i); };
	int SetHeight(int i) { return SetClipH(i); };
	int SetFrames(int i) { currentFrame = 0; return maxFrames = i; };
	int SetStrips(int i) { currentStrip = 0; return maxStrips = i; };

	int GetWidth() const { return GetClipW(); }
	int GetHeight() const { return GetClipH(); }
	int GetFrames() const { return maxFrames; }
	int GetStrips() const { return maxStrips; }

	int SetCurrentFrame(int i) { return currentFrame = i; };
	int SetCurrentStrip(int i) { return currentStrip = i; };
	int SetInterval(int i) { ticks = 0; return interval = i; }

	int GetCurrentFrame() const { return currentFrame; };
	int GetCurrentStrip() const { return currentStrip; };
	int GetInterval() const { return interval; };
private:
	int currentFrame, maxFrames;
	int currentStrip, maxStrips;
	int interval, ticks;
};

#endif
