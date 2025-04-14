#include "NewGameState.h"

#include "../game/Game.h"

#include "../sdlutils/Texture.h"

#include "../sdlutils/SDLUtils.h"

#include "../sdlutils/InputHandler.h"

#include "../ecs/Manager.h"

NewGameState::NewGameState() :
	_msg(sdlutils().msgs().at("NewGameStatePacman")) 
{
}

NewGameState::~NewGameState(){
}

void NewGameState::enter(){
	sdlutils().soundEffects().at("pacman_intro").setVolume(16);
	sdlutils().soundEffects().at("pacman_intro").play(0, 1);
}

void NewGameState::leave(){
	Message m;
	m.id = _m_NEW_GAME;
	Game::Instance()->getManager()->send(m);
}

void NewGameState::update(){

	auto& inputHandler = ih();

	if (inputHandler.keyDownEvent()) {
		Game::Instance()->setState(Game::NEWROUND);
	}

	sdlutils().clearRenderer();

	_msg.render((sdlutils().width() - _msg.width()) / 2,
		(sdlutils().height() - _msg.height()) / 2);

	sdlutils().presentRenderer();
}
