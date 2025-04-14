// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include "../ecs/Component.h"
#include "../utils/Vector2D.h"
#include <cassert>

struct Transform: public ecs::Component {

	Transform() :
			_pos(), _vel(), _width(), _height(), _rot() {
	}

	Transform(Vector2D pos, Vector2D vel, float w, float h, float r) :
			_pos(pos), _vel(vel), _width(w), _height(h), _rot(r) {
	}

	virtual ~Transform() {
	}

	void init(Vector2D pos, Vector2D vel, float w, float h, float r) {
		_pos = pos;
		_vel = vel;
		_width = w;
		_height = h;
		_rot = r;
	}

	Vector2D _pos;
	Vector2D _vel;
	float _width;
	float _height;
	float _rot;
};

