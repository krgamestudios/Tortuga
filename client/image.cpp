#include "image.h"

Image::Image(SDL_Surface* p) {
	SetSurface(p);
}

Image::Image(SDL_Surface* p, SDL_Rect r) {
	SetSurface(p, r);
}

SDL_Surface* Image::SetSurface(SDL_Surface* p) {
	surface = p;
	clip = {0, 0, (Uint16)surface->w, (Uint16)surface->h};
	return surface;
}

SDL_Surface* Image::SetSurface(SDL_Surface* p, SDL_Rect r) {
	surface = p;
	clip = r;
	return surface;
}

SDL_Surface* Image::GetSurface() const {
	return surface;
}

void Image::DrawTo(SDL_Surface* dest, Sint16 x, Sint16 y) {
	SDL_Rect sclip = clip, dclip = {x,y};

	SDL_BlitSurface(surface, &sclip, dest, &dclip);
}
