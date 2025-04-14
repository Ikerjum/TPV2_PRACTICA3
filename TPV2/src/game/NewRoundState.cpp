#include "NewRoundState.h"

#include "../sdlutils/Texture.h"

#include "../sdlutils/SDLUtils.h"

#include "../sdlutils/InputHandler.h"

#include "../game/Game.h"

#include "../game/ConfigAsteroids.h"

#include "../ecs/Manager.h"



NewRoundState::NewRoundState() :
	_msg(sdlutils().msgs().at("NewRoundStatePacman"))
{
}

NewRoundState::~NewRoundState()
{
}

void NewRoundState::enter()
{
}

void NewRoundState::leave()
{
	sdlutils().virtualTimer().resetTime();
}

void NewRoundState::update()
{
	auto& inputHandler = ih();
	//auto pm = Game::Instance()->getManager()->getHandler(ecs::hdlr::PACMAN);
	//auto _pmTR = Game::Instance()->getManager()->getComponent<Transform>(pm);
	//
	//auto pmSys = Game::Instance()->getPacmanSys();
	//pmSys->resetPacmanPosition(); //No funcional

	if (inputHandler.keyDownEvent() && inputHandler.isKeyDown(SDL_SCANCODE_RETURN)) {
		//Mandamos mensaje de reset de posicion de todo
		Message m;
		m.id = _m_ROUND_START;
		Game::Instance()->getManager()->send(m);

		Game::Instance()->setState(Game::RUNNING);
	}

	sdlutils().clearRenderer();

	_msg.render((sdlutils().width() - _msg.width()) / 2,
		(sdlutils().height() - _msg.height()) / 2);

	sdlutils().presentRenderer();

}
