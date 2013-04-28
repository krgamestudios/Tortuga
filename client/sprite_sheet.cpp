#include "sprite_sheet.h"

SpriteSheet::SpriteSheet(SDL_Surface* s, int w, int h)
	: Image(s, {0, 0, (Uint16)w, (Uint16)h})
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
