#include "Blindr.h"

bool Blindr::Explosion::draw() {
	int frame = int(time * ExplosionFPS);
	time += Time::deltaSeconds();
	if (frame >= assets->explosion->nframes) {
		return true;
	}
	SpriteBatch::draw(assets->explosion, pos, frame);
	return 0;
}