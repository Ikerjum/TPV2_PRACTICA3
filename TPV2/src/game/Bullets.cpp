// This file is part of the course TPV2@UCM - Samir Genaim

#include "Bullets.h"

#include "../sdlutils/SDLUtils.h"
Bullets::Bullets() :
		_bullets(), //
		_img(sdlutils().images().at("fire")), //
		_lastUsed(-1) {
}

Bullets::~Bullets() {
}

void Bullets::shoot(Vector2D p, Vector2D v, int width, int height, float r) {

	int idx = (_lastUsed + 1) % max_bullets;
	while (idx != _lastUsed && _bullets[idx].used) {
		idx = (idx + 1) % max_bullets;
	}

	if (idx != _lastUsed) {
		_lastUsed = idx;
		Bullet &b = _bullets[idx];
		b.used = true;
		b.pos = p;
		b.vel = v;
		b.width = width;
		b.height = height;
		b.rot = r;
	}
}

void Bullets::update() {
	int width = sdlutils().width();
	int height = sdlutils().height();

	for (Bullet &b : _bullets) {
		if (b.used) {
			b.pos = b.pos + b.vel;
			if (!b.pos.inRange(-b.width, width, -b.height, height)) {
				b.used = false;
			}
		}
	}
}

void Bullets::render() {
	for (Bullet &b : _bullets) {
		if (b.used) {
			SDL_Rect dest = build_sdlrect(b.pos, b.width, b.height);
			_img.render(dest, b.rot);
		}
	}
}
