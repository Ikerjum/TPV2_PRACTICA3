// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include "../ecs/Component.h"

class Texture;

struct Image: public ecs::Component {

	Image() :
			_tex() {
	}

	Image(Texture *tex) :
			_tex(tex) {
	}

	virtual ~Image() {
	}

	Texture *_tex;
};

