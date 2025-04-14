#include "PausedState.h"

#include "../sdlutils/SDLUtils.h"

#include "../game/Game.h"

#include "../sdlutils/InputHandler.h"

PausedState::PausedState() :
	_msg(sdlutils().msgs().at("PauseStatePacman"))
{
}

PausedState::~PausedState()
{
}

void PausedState::enter()
{
	sdlutils().virtualTimer().pause();
}

void PausedState::leave()
{
	sdlutils().virtualTimer().resume();
}

void PausedState::update()
{

	auto& inputHandler = ih();

	if (inputHandler.keyDownEvent()) {
		Game::Instance()->setState(Game::RUNNING);
	}

	sdlutils().clearRenderer();

	_msg.render((sdlutils().width() - _msg.width()) / 2,
		(sdlutils().height() - _msg.height()) / 2);

	sdlutils().presentRenderer();
}
