#include "sprite_sheet.hpp"

SpriteSheet::SpriteSheet(SDL_Surface* s, Uint16 w, Uint16 h)
	: Image(s, {0, 0, w, h})
{
	currentFrame = 0; maxFrames = GetSurface()->w / GetClipW();
	currentStrip = 0; maxStrips = GetSurface()->h / GetClipH();
	interval = ticks = 0;
}

void SpriteSheet::Update(int delta) {
	if (interval && (ticks += delta) > interval) {
		if (++currentFrame >= maxFrames) {
			currentFrame = 0;
		}
		ticks = 0;
	}
	SetClipX(currentFrame * GetClipW());
	SetClipY(currentStrip * GetClipH());
}

void SpriteSheet::SetSurface(SDL_Surface* s, Uint16 w, Uint16 h) {
	Image::SetSurface(s, {0, 0, w, h});
	currentFrame = 0; maxFrames = GetSurface()->w / GetClipW();
	currentStrip = 0; maxStrips = GetSurface()->h / GetClipH();
	interval = ticks = 0;
}