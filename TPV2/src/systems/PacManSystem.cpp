// This file is part of the course TPV2@UCM - Samir Genaim

#include "PacManSystem.h"

#include "../components/Image.h"
#include "../components/Transform.h"
#include "../ecs/Manager.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../sdlutils/Texture.h"
#include "../components/ImageWithFrames.h"
#include "../game/Game.h"
#include "../components/Health.h"

PacManSystem::PacManSystem() :
		_pmTR(nullptr), _pmIWF(nullptr), _tex(nullptr), _timer(0), _score() {
}

PacManSystem::~PacManSystem() {
}

void PacManSystem::initSystem() {
	// create the PacMan entity
	//
	auto pacman = _mngr->addEntity();
	_mngr->setHandler(ecs::hdlr::PACMAN, pacman);


	_pmTR = _mngr->addComponent<Transform>(pacman);
	auto s = 50.0f;
	auto x = (sdlutils().width() - s) / 2.0f;
	auto y = (sdlutils().height() - s) / 2.0f;
	_pmTR->init(Vector2D(x, y), Vector2D(), s, s, 0.0f);

	_timer = 0;
	_pmIWF = _mngr->addComponent<ImageWithFrames>(pacman, &sdlutils().images().at("sprites"),s,s,0,0,3);

	_mngr->addComponent<Health>(pacman);

}

void PacManSystem::update() {

	auto &ihdlr = ih();

	if (ihdlr.keyDownEvent()) {
		if (ihdlr.isKeyDown(SDL_SCANCODE_UP)) {
			_pmTR->_vel.set(Vector2D(0.0f, -3.0f).rotate(_pmTR->_rot));
		}
		else if (ihdlr.isKeyDown(SDL_SCANCODE_DOWN)) {
			_pmTR->_vel.set(Vector2D(0.0f, 0.0f));
		}
		else if (ihdlr.isKeyDown(SDL_SCANCODE_LEFT)) {
			_pmTR->_rot -= 90.f;
			_pmTR->_vel.set(Vector2D(_pmTR->_vel.getX(), _pmTR->_vel.getY()).rotate(-90.f));
		}
		else if (ihdlr.isKeyDown(SDL_SCANCODE_RIGHT)) {
			_pmTR->_rot += 90.f;
			_pmTR->_vel.set(Vector2D(_pmTR->_vel.getX(),_pmTR->_vel.getY()).rotate(90.f));
		}
	}

	_pmTR->_pos = _pmTR->_pos + _pmTR->_vel;

	// check left/right borders
	if (_pmTR->_pos.getX() < 0) {
		_pmTR->_pos.setX(0.0f);
		_pmTR->_vel.set(0.0f, 0.0f);
	} else if (_pmTR->_pos.getX() + _pmTR->_width > sdlutils().width()) {
		_pmTR->_pos.setX(sdlutils().width() - _pmTR->_width);
		_pmTR->_vel.set(0.0f, 0.0f);
	}
	
	// check upper/lower borders
	if (_pmTR->_pos.getY() < 0) {
		_pmTR->_pos.setY(0.0f);
		_pmTR->_vel.set(0.0f, 0.0f);
	} else if (_pmTR->_pos.getY() + _pmTR->_height > sdlutils().height()) {
		_pmTR->_pos.setY(sdlutils().height() - _pmTR->_height);
		_pmTR->_vel.set(0.0f, 0.0f);
	}


	
	if (_timer + 100 <= sdlutils().virtualTimer().currTime()) {
		_timer = sdlutils().virtualTimer().currTime();
		_pmIWF->updateImage();
	}


}

void PacManSystem::resetPacmanPosition()
{
	assert(_pmTR != nullptr);

	float tam = _pmTR->_width;
	auto y = (sdlutils().height() - tam) / 2.0f;
	auto x = (sdlutils().width() - tam) / 2.0f;
	_pmTR->_pos.set(x,y); //Reseteamos posicion
	_pmTR->_vel.set(0.0f, 0.0f); //Reseteamos velocidad
	_pmTR->_rot = 0.0f; //Reseteamos rotacion

	_timer = 0;
}

void PacManSystem::loseRound()
{
	auto pac = _mngr->getHandler(ecs::hdlr::PACMAN);
	auto h = _mngr->getComponent<Health>(pac);
	//Perdemos una vida
	h->loseLife();
}

void PacManSystem::resetLives()
{
	auto pac = _mngr->getHandler(ecs::hdlr::PACMAN);
	auto h = _mngr->getComponent<Health>(pac);
	//Reseteamos las vidas del pacman
	h->reset();
}

void PacManSystem::immunityEnd()
{
	auto pac = _mngr->getHandler(ecs::hdlr::PACMAN);
	auto h = _mngr->getComponent<Health>(pac);
	//Desactivamos inmunidad del pacman para no mantenerla entre partidas
	h->deactivateImmunity();
}

void PacManSystem::recieve(const Message& m)
{
	switch (m.id) {
	case _m_ROUND_START:
		resetPacmanPosition();
		break;
	case _m_ROUND_OVER:
		loseRound();
		Game::Instance()->setState(Game::NEWROUND);
		break;
	case _m_NEW_GAME:
		resetLives();
		setScore(0);
		break;
	case _m_GAME_OVER:
		immunityEnd();
		Game::Instance()->setVictory(m.game_over_data.win);
		Game::Instance()->setState(Game::GAMEOVER);
		break;
	case _m_PACMAN_FOOD_COLLISION:
		//_score += _mngr->getComponent<Points>(m.food_eaten_data.e)->_points;
		incrScore(5);
		break;
	default:
		break;
	}
}
