#include "RunningState.h"

#include "../sdlutils/InputHandler.h"

#include "../game/Game.h"
#include "../sdlutils/SDLUtils.h"

#include "../sdlutils/Texture.h"

#include "../components/Health.h"

#include "../utils/Collisions.h"

#include "../components/Transform.h"
#include "../game/ConfigAsteroids.h"
#include "../systems/CollisionsSystem.h"
#include "../systems/PacManSystem.h"
#include "../systems/RenderSystem.h"

#include "../systems/FoodSystem.h"

#include "../ecs/Manager.h"


RunningState::RunningState()
{
}

RunningState::~RunningState()
{
}

void RunningState::enter()
{
}

void RunningState::leave()
{
}

void RunningState::update()
{

	sdlutils().virtualTimer().regCurrTime();

	auto& inputHandler = ih();

	if (inputHandler.keyDownEvent() && inputHandler.isKeyDown(SDL_SCANCODE_P)) {
		Game::Instance()->setState(Game::PAUSED);
	}

	//sdlutils().clearRenderer();

	//_pacmanSys->update();
	Game::Instance()->getPacmanSys()->update();
	//_startsSys->update();
	Game::Instance()->getFoodSys()->update();
	//_ghostsSys->update();
	Game::Instance()->getGhostsSys()->update();
	//_collisionSys->update();
	Game::Instance()->getcollisionSys()->update();
	//_immunitySys->update();
	Game::Instance()->getImmunitySys()->update();
	//_mngr->refresh();
	Game::Instance()->getManager()->refresh();

	sdlutils().clearRenderer();
	
	//_renderSys->update();
	Game::Instance()->getrenderSys()->update();
	
	sdlutils().presentRenderer();

	//manager->update();
	//auto shipPos = manager->getComponent<Transform>(ship)->getPos();
	//auto shipWidth = manager->getComponent<Transform>(ship)->getWidth();
	//auto shipHeight = manager->getComponent<Transform>(ship)->getHeight();
	//auto shipRot = manager->getComponent<Transform>(ship)->getRot();
	//COLISIONES
	//const Vector2D &o1Pos, float o1Width,
	//	float o1Height, float o1Rot, const Vector2D &o2Pos, float o2Width,
	//	float o2Height, float o2Rot
	//for (int i = 0; i < manager->getEntities(ecs::grp::ASTEROIDS).size(); ++i) {
	//	auto asteroid = manager->getEntities(ecs::grp::ASTEROIDS)[i];
	//	auto astPos = manager->getComponent<Transform>(asteroid)->getPos();
	//	auto astWidth = manager->getComponent<Transform>(asteroid)->getWidth();
	//	auto astHeight = manager->getComponent<Transform>(asteroid)->getHeight();
	//	auto astRot = manager->getComponent<Transform>(asteroid)->getRot();
	//
	//	for (auto i = manager->getComponent<Gun>(ship)->begin(); i != manager->getComponent<Gun>(ship)->end(); ++i) {
	//		auto bulletPos = i->pos;
	//		auto bulletWidth = i->width;
	//		auto bulletHeight = i->height;
	//		auto bulletRot = i->rot;
	//
	//		if (Collisions::collidesWithRotation(astPos, astWidth, astHeight, astRot, bulletPos, bulletWidth, bulletHeight, bulletRot)) {
	//			Game::Instance()->getAsteroidsFacade()->split_asteroid(asteroid);
	//			i->used = false;
	//		}
	//	}
	//
	//	if (Collisions::collidesWithRotation(shipPos, shipWidth, shipHeight, shipRot, astPos, astWidth, astHeight, astRot)) {
	//		manager->getComponent<Health>(ship)->loseOneLive();
	//		if (manager->getComponent<Health>(ship)->checkLives() > 0) Game::Instance()->setState(Game::NEWROUND);
	//	}
	//
	//
	//}
	//manager->render();
	//manager->refresh();
	//GENERACION DE ASTEROIDE CADA 5 SEGS
	//ARCHIVO CONFIG PARA EL TIEMPO DE ESPERA A GENERAR UN NUEVO ASTEROIDE
	//if (_timer + Game::Instance()->getConfigAsteroids()->frequencyAsteroids() <= sdlutils().virtualTimer().currTime()) {
	//	_timer = sdlutils().virtualTimer().currTime();
	//	Game::Instance()->getAsteroidsFacade()->create_asteroids(1);
	//}
	//sdlutils().presentRenderer();

}
