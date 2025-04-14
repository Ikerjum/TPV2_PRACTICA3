#include "ImageWithFrames.h"

#include <cassert>

#include "../ecs/Manager.h"
#include "../sdlutils/macros.h"
#include "../sdlutils/Texture.h"
#include "Transform.h"
#include "../game/Game.h"
#include "../sdlutils/SDLUtils.h"

ImageWithFrames::ImageWithFrames() :
	_tr(), _tex(),_width(0), _height(0), _texHeigthMax(0), _texWidthMax(0), _timer(0), _initialFil(0), _initialCol(0), _numFrames(0), _actualFrame(0) {
}

ImageWithFrames::ImageWithFrames(Texture* tex, float width, float height, int fil, int col, int numFrames) :
	_tr(), _tex(tex){
	_texWidthMax = _tex->width();
	_texHeigthMax = _tex->height();
	_width = (_tex->width() / 8.0f);
	_height = (_tex->height() / 8.0f);
	_timer = 0;
	_numFrames = numFrames;
	_actualFrame = 0;
	_initialFil = fil;
	_initialCol = col;
	_reverseAnimation = false;
	_actFrame = Vector2D(_initialFil*(_texWidthMax/8.0f),_initialCol*(_texHeigthMax/8.0f));
}

ImageWithFrames::~ImageWithFrames() {
}

void ImageWithFrames::initComponent() {

	auto mngr = Game::Instance()->getManager();
	_tr = mngr->getComponent<Transform>(_ent);
	assert(_tr != nullptr);
}


void ImageWithFrames::updateImage() {


	if (_actualFrame < _numFrames) {
		if (_actFrame.getX() + _width <= _texWidthMax) {
			if (!_reverseAnimation) _actFrame.setX(_actFrame.getX() + _width);
			else _actFrame.setX(_actFrame.getX() - _width);
		}
		//else if (_actFrame.getY() + _height <= _texHeigthMax) {
		//	_actFrame.setX(0);
		//	_actFrame.setY(_actFrame.getY() + _height);
		//}
		_actualFrame++;
	}
	else {
		_actualFrame = 0;
		_reverseAnimation = !_reverseAnimation;
	}


}

void ImageWithFrames::resetFramePosition()
{
	_actFrame.set(_initialFil * _width, _initialCol * _height);
	_reverseAnimation = false;
	_actualFrame = 0;
}

void ImageWithFrames::setCol(int newCol) {
	_initialCol = newCol;
}

void ImageWithFrames::setFil(int newFil) {
	_initialFil = newFil;
}
