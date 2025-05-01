// This file is part of the course TPV2@UCM - Samir Genaim

#include "Game.h"

#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "LittleWolf.h"
#include "Networking.h"
#include "../utils/Collisions.h"

Game::Game() :
		_little_wolf(), //
		_net()
{
}

Game::~Game() {

	// release InputHandler if the instance was created correctly.
	if (InputHandler::HasInstance())
		InputHandler::Release();

	// release SLDUtil if the instance was created correctly.
	if (SDLUtils::HasInstance())
		SDLUtils::Release();

	delete _little_wolf;
	delete _net;
}

bool Game::initGame(const char *map, char* host, Uint16 port) {


	
	_net = new Networking();

	if (!_net->init(host, port)) {
		SDLNetUtils::print_SDLNet_error();
		return false;
	}

	_little_wolf = new LittleWolf();

	// load a map
	_little_wolf->load(map);

	// initialize the SDL singleton
	if (!SDLUtils::Init("[Little Wolf: " + std::string(map) + "]",
			_little_wolf->get_xres(),
			_little_wolf->get_yres(),
			"resources/config/littlewolf.resources.json")) {

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

	_little_wolf->init(sdlutils().window(), sdlutils().renderer());

	// add some players
	//_little_wolf->addPlayer(0);
	//_little_wolf->addPlayer(1);
	//_little_wolf->addPlayer(2);
	//_little_wolf->addPlayer(3);

	_little_wolf->addPlayer(_net->client_id());

	return true;
}

void Game::start() {

	// a boolean to exit the loop
	bool exit = false;

	auto &ihdlr = ih();

	while (!exit) {
		Uint32 startTime = sdlutils().currRealTime();

		// refresh the input handler
		ihdlr.refresh();

		if (ihdlr.keyDownEvent()) {

			// ESC exists the game
			if (ihdlr.isKeyDown(SDL_SCANCODE_ESCAPE)) {
				exit = true;
				continue;
			}

		}

		_little_wolf->update();
		_net->update();

		check_collisions();

		// the clear is not necessary since the texture we copy to the window occupies the whole screen
		sdlutils().clearRenderer();

		_little_wolf->render();

		sdlutils().presentRenderer();

		Uint32 frameTime = sdlutils().currRealTime() - startTime;

		if (frameTime < 10)
			SDL_Delay(10 - frameTime);
	}

	_net->disconnect();
}

bool Game::init()
{
	return true;
}

void Game::check_collisions()
{
	//if (!net_->is_master())
	//return;
	//
	//for (Bullets::Bullet &b : *bm_) {
	//	if (b.used) {
	//		for (Fighter::Player &p : *fighters_) {
	//			if (p.state == Fighter::ALIVE) {
	//				if (Collisions::collidesWithRotation(p.pos, p.width,
	//						p.height, p.rot, b.pos, b.width, b.height, b.rot)) {
	//					net_->send_dead(p.id);
	//					continue;
	//				}
	//			}
	//		}
	//	}
	//}

	//if (!_net->is_master())
	//return;
	
	//for (Bullets::Bullet &b : *bm_) {
	//	if (b.used) {
	//		for (LittleWolf::Player &p : _little_wolf->getPlayers()) {
	//			if (p.state == LittleWolf::PlayerState::ALIVE) {
	//				if (Collisions::collidesWithRotation(p.pos, p.width,
	//						p.height, p.rot, b.pos, b.width, b.height, b.rot)) {
	//					_net->send_dead(p.id);
	//					continue;
	//				}
	//			}
	//		}
	//	}
	//}
}

