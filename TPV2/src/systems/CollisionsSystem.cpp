// This file is part of the course TPV2@UCM - Samir Genaim

#include "CollisionsSystem.h"

#include "../components/Transform.h"
#include "../ecs/Manager.h"
#include "../utils/Collisions.h"
#include "../components/Health.h"
#include "../sdlutils/SDLUtils.h"

CollisionsSystem::CollisionsSystem() : isMiraclePC(false) {
	// TODO Auto-generated constructor stub

}

CollisionsSystem::~CollisionsSystem() {
	// TODO Auto-generated destructor stub
}

void CollisionsSystem::initSystem() {
}

void CollisionsSystem::update() {

	// the PacMan's Transform
	//
	auto pm = _mngr->getHandler(ecs::hdlr::PACMAN);
	auto pTR = _mngr->getComponent<Transform>(pm);

	// For safety, we traverse with a normal loop until the current size. In this
	// particular case we could use a for-each loop since the list stars is not
	// modified.
	//
	auto &fruits = _mngr->getEntities(ecs::grp::FRUITS);
	auto n = fruits.size();
	for (auto i = 0u; i < n; i++) {
		auto f = fruits[i];
		if (_mngr->isAlive(f)) { // if the fruit is active (it might have died in this frame)

			// the Fruit's Transform
			//
			auto fTR = _mngr->getComponent<Transform>(f);

			// check if PacMan collides with the Star (i.e., eat it)
			if (Collisions::collides(			//
					pTR->_pos, pTR->_width, pTR->_height, //
					fTR->_pos, fTR->_width, fTR->_height)) {

				Message m;
				m.id = _m_PACMAN_FOOD_COLLISION;
				m.food_collision_data.fruit = f;
				_mngr->send(m);
				sdlutils().musics().at("pacman_chomp").play(0);

			}
		}
	}

	auto& ghosts = _mngr->getEntities(ecs::grp::GHOSTS);
	auto m = ghosts.size();
	for (auto i = 0u; i < m; i++) {
		auto e = ghosts[i];
		if (_mngr->isAlive(e)) { //si el fantasma esta vivo

			// transform del fantasma
			auto eTR = _mngr->getComponent<Transform>(e);

			// comprueba colision con el pacman
			if (Collisions::collides(			//
				pTR->_pos, pTR->_width, pTR->_height, //
				eTR->_pos, eTR->_width, eTR->_height)) {

				if (!isMiraclePC) { //En caso de ser un pacman normal
					auto pmHealth = _mngr->getComponent<Health>(pm);
					if (pmHealth->getLives() > 1) { //Todavia tiene mas de 1 vida
						Message m;
						m.id = _m_ROUND_OVER;
						_mngr->send(m);
						sdlutils().soundEffects().at("pacman_death").setVolume(16);
						sdlutils().soundEffects().at("pacman_death").play(0, 1);
					}
					else { // Tiene 0 vidas
						Message m;
						m.id = _m_GAME_OVER;
						m.game_over_data.win = false;
						_mngr->send(m);
					}
				}
				else { //En caso de ser un pacman milagroso, matamos al fantasma
					_mngr->setAlive(e, false); 
					sdlutils().soundEffects().at("pacman_eat").setVolume(16);
					sdlutils().soundEffects().at("pacman_eat").play(0, 1);
				}

			}
		}
	}

}

void CollisionsSystem::changeMiraclePC(bool b)
{
	isMiraclePC = b;
}

void CollisionsSystem::recieve(const Message& m)
{
	switch (m.id) {
	case _m_IMMUNITY_START:
		changeMiraclePC(true);
		break;
	case _m_IMMUNITY_END:
		changeMiraclePC(false);
		break;
	default:
		break;
	}

}