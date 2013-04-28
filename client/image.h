#ifndef IMAGE_H_
#define IMAGE_H_

#include "SDL/SDL.h"

class Image {
public:
	Image(SDL_Surface*);
	Image(SDL_Surface*, SDL_Rect);
	virtual ~Image() {}

	SDL_Surface* SetSurface(SDL_Surface*);
	SDL_Surface* SetSurface(SDL_Surface*, SDL_Rect);
	SDL_Surface* GetSurface() const;

	void DrawTo(SDL_Surface*, Sint16 x, Sint16 y);

	//Clip handlers
	SDL_Rect SetClip(SDL_Rect r) { return clip = r; }
	SDL_Rect GetClip() const { return clip; }

	Sint16 SetClipX(Sint16 x) { return clip.x = x; }
	Sint16 SetClipY(Sint16 y) { return clip.y = y; }
	Uint16 SetClipW(Uint16 w) { return clip.w = w; }
	Uint16 SetClipH(Uint16 h) { return clip.h = h; }

	Sint16 GetClipX() const { return clip.x; }
	Sint16 GetClipY() const { return clip.y; }
	Uint16 GetClipW() const { return clip.w; }
	Uint16 GetClipH() const { return clip.h; }
private:
	SDL_Surface* surface;
	SDL_Rect clip;
};

#endif
