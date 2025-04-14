// This file is part of the course TPV2@UCM - Samir Genaim

#include "Game.h"

#include "../ecs/Manager.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../systems/CollisionsSystem.h"
#include "../systems/PacManSystem.h"
#include "../systems/RenderSystem.h"
#include "../systems/FoodSystem.h"
#include "../systems/GhostSystem.h"
#include "../systems/ImmunitySystem.h"

#include "../utils/Vector2D.h"
#include "../utils/Collisions.h"
#include "../game/NewGameState.h"
#include "../game/NewRoundState.h"
#include "../game/RunningState.h"
#include "../game/PausedState.h"
#include "../game/GameOverState.h"
using ecs::Manager;

Game::Game() :
		_mngr(),
		_pacmanSys(),
		_gameCtrlSys(),
		_foodSys(),
		_renderSys(),
		_collisionSys(),
		_immunitySys(),
		_state(nullptr),
		_newgame_state(nullptr),
		_newround_state(nullptr),
		_paused_state(nullptr),
		_running_state(nullptr),
		_victory(false)
{

}

Game::~Game() {
	delete _mngr;

	// release InputHandler if the instance was created correctly.
	if (InputHandler::HasInstance())
		InputHandler::Release();

	// release SLDUtil if the instance was created correctly.
	if (SDLUtils::HasInstance())
		SDLUtils::Release();

	delete _newgame_state;
	delete _newround_state;
	delete _running_state;
	delete _paused_state;
	delete _gameover_state;

	//delete _pacmanSys;
	//delete _startsSys;
	//delete _gameCtrlSys;
	//delete _renderSys;
	//delete _collisionSys;
}

bool Game::init() {

	// initialize the SDL singleton
	if (!SDLUtils::Init("PacMan, Stars, ...", 800, 600,
			"resources/config/resources.json")) {

		std::cerr << "Something went wrong while initializing SDLUtils"
				<< std::endl;
		return false;
	}

	// initialize the InputHandler singleton
	if (!InputHandler::Init()) {
		std::cerr << "Something went wrong while initializing SDLHandler"
				<< std::endl;
		return false;

	}

	return true;
}

void Game::initGame()
{
	_mngr = new Manager();

	//STATES
	_newgame_state = new NewGameState();
	_newround_state = new NewRoundState();
	_running_state = new RunningState();
	_paused_state = new PausedState();
	_gameover_state = new GameOverState();

	//SYSTEMS
	_pacmanSys = _mngr->addSystem<PacManSystem>();
	_foodSys = _mngr->addSystem<FoodSystem>();
	_ghostsSys = _mngr->addSystem<GhostSystem>();
	_renderSys = _mngr->addSystem<RenderSystem>();
	_collisionSys = _mngr->addSystem<CollisionsSystem>();
	_immunitySys = _mngr->addSystem<ImmunitySystem>();
	_state = _newgame_state;
	_state->enter();
}

void Game::start() {

	// a boolean to exit the loop
	bool exit = false;

	auto &ihdlr = ih();

	while (!exit) {
		Uint32 startTime = sdlutils().currRealTime();

		// refresh the input handler
		ihdlr.refresh();

		if (ihdlr.isKeyDown(SDL_SCANCODE_ESCAPE)) {
			exit = true;
			continue;
		}

		//UPDATE DEL ESTADO (si usamos el mecanismo de enviar mensajes con retraso)
		_state->update();

		//FLUSHMESSAGES

		Uint32 frameTime = sdlutils().currRealTime() - startTime;

		if (frameTime < 10)
			SDL_Delay(10 - frameTime);
	}

}

