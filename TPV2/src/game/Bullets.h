// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <array>

#include "BulletsManager.h"

class Texture;

class Bullets: public BulletManager {
public:
	Bullets();
	virtual ~Bullets();

	struct Bullet {
		bool used = false;
		Vector2D pos;
		Vector2D vel;
		int width;
		int height;
		float rot;
	};

	constexpr static auto max_bullets = 20;
	typedef std::array<Bullet, max_bullets> bullets_array;
	typedef bullets_array::iterator iterator;


	iterator begin() {
		return _bullets.begin();
	}

	iterator end() {
		return _bullets.end();
	}

	void shoot(Vector2D p, Vector2D v, int width, int height, float r) override;

	void update();
	void render();

private:
	bullets_array _bullets;
	Texture &_img;
	int _lastUsed;

};

