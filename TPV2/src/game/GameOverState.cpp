#include "GameOverState.h"

#include "../sdlutils/SDLUtils.h"

#include "../sdlutils/InputHandler.h"

#include "../game/Game.h"

GameOverState::GameOverState() :
	_msg(sdlutils().msgs().at("GameOverStatePacman")),
	_msgWin(sdlutils().msgs().at("GameOverStatePacmanWin")),
	_msgLose(sdlutils().msgs().at("GameOverStatePacmanLose")),
	_victory(false),
	_offset(100.f)
{
}

GameOverState::~GameOverState()
{
}

void GameOverState::enter()
{
	_victory = Game::Instance()->getVictory();
}

void GameOverState::leave()
{
}

void GameOverState::update()
{
	auto& inputHandler = ih();

	if (inputHandler.keyDownEvent()) {
		Game::Instance()->setState(Game::NEWGAME);
	}

	sdlutils().clearRenderer();

	_msg.render((sdlutils().width() - _msg.width()) / 2,
		(sdlutils().height() - _msg.height()) / 2);

	if (_victory) {
		_msgWin.render((sdlutils().width() - _msg.width()) / 2,
			(sdlutils().height() + _offset - _msg.height()) / 2);
	}
	else {
		_msgLose.render((sdlutils().width() - _msg.width()) / 2,
			(sdlutils().height() + _offset - _msg.height()) / 2);
	}

	sdlutils().presentRenderer();
}
