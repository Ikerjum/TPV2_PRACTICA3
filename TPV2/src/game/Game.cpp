// This file is part of the course TPV2@UCM - Samir Genaim

#include "Game.h"

#include "Bullets.h"
#include "Fighter.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLNetUtils.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Collisions.h"
#include "Networking.h"

Game::Game() :
		bm_(nullptr), //
		fighters_(nullptr), //
		net_(nullptr) {
}

Game::~Game() {
	delete fighters_;
	delete bm_;
	delete net_;

	// release InputHandler if the instance was created correctly.
	if (InputHandler::HasInstance())
		InputHandler::Release();

	// release SLDUtil if the instance was created correctly.
	if (SDLUtils::HasInstance())
		SDLUtils::Release();

}

bool Game::init(char *host, Uint16 port) {

	net_ = new Networking();

	if (!net_->init(host, port)) {
		SDLNetUtils::print_SDLNet_error();
		return false;
	}
	std::cout << "Connected as client " << (int) net_->client_id() << std::endl;

	// initialize the SDL singleton
	if (!SDLUtils::Init("SDLNet Game", 800, 600,
			"resources/config/asteroids.multiplayer.resources.json")) {

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

	bm_ = new Bullets();
	fighters_ = new Fighter();

	// add some players
	fighters_->addPlayer(net_->client_id());

	return true;
}

void Game::start() {
	// a boolean to exit the loop
	bool exit = false;

	auto &ihdlr = ih();
	auto &vt = sdlutils().virtualTimer();
	while (!exit) {
		Uint32 startTime = vt.regCurrTime();

		// refresh the input handler
		ihdlr.refresh();
		if (ihdlr.keyDownEvent()) {

			// ESC exists the game
			if (ihdlr.isKeyDown(SDL_SCANCODE_ESCAPE)) {
				exit = true;
				continue;
			}

		}

		fighters_->update();
		bm_->update();
		net_->update();

		check_collisions();

		sdlutils().clearRenderer();

		fighters_->render();
		bm_->render();

		sdlutils().presentRenderer();

		Uint32 frameTime = vt.currRealTime() - startTime;

		if (frameTime < 10)
			SDL_Delay(10 - frameTime);
	}

	net_->disconnect();

}

void Game::check_collisions() {
	if (!net_->is_master())
		return;

	for (Bullets::Bullet &b : *bm_) {
		if (b.used) {
			for (Fighter::Player &p : *fighters_) {
				if (p.state == Fighter::ALIVE) {
					if (Collisions::collidesWithRotation(p.pos, p.width,
							p.height, p.rot, b.pos, b.width, b.height, b.rot)) {
						net_->send_dead(p.id);
						continue;
					}
				}
			}
		}
	}
}
