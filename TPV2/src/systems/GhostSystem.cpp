#include "GhostSystem.h"
#include "../ecs/Manager.h"
#include "../sdlutils/SDLUtils.h"
#include "../components/Transform.h"

#include "../utils/Vector2D.h"

#include "../ecs/Entity.h"

#include "../components/ImageWithFrames.h"

#include "../sdlutils/RandomNumberGenerator.h"

GhostSystem::GhostSystem() : _numGhosts(0), _timerAnimation(0), _timerSpawn(0), _immunityPacman(false)
{
}

GhostSystem::~GhostSystem()
{
}

void GhostSystem::initSystem()
{
	//Ponemos contadores a 0
	_numGhosts = 0;
	_timerAnimation = 0;
	_timerSpawn = 0;
	_immunityPacman = false;
}

void GhostSystem::update()
{

	auto e = _mngr->getHandler(ecs::hdlr::PACMAN);
	auto ghosts = _mngr->getEntities(ecs::grp::GHOSTS);
	auto _pmTR = _mngr->getComponent<Transform>(e);

	for (auto ghost : ghosts) {
		if (_mngr->hasComponent<Transform>(ghost)) {
			//Obtenemos el transform del fantasma como primera opcion
			auto ghostTR = _mngr->getComponent<Transform>(ghost);
			
			//Actualizando velocidad en funcion de la posicion de Pacman y la de los fantasmas
			//uso del randomNumberGenerator
			auto& r = sdlutils().rand();
			int rnd = r.nextInt(0, 2501); // 0 a 2500

			// 0.025 de probabilidad
			if (rnd <= 5) {
				ghostTR->_vel = ((_pmTR->_pos - ghostTR->_pos).normalize() * 1.1f);
			}

			// check left/right borders
			if (ghostTR->_pos.getX() < 0) {
				ghostTR->_pos.setX(0.0f);
				ghostTR->_vel.setX(ghostTR->_vel.getX() * -1);
			}
			//IMPORTANTE, tomamos en cuenta la posicion del fantasma
			else if (ghostTR->_pos.getX() + ghostTR->_width > sdlutils().width()) {
				ghostTR->_pos.setX(sdlutils().width() - ghostTR->_width);
				ghostTR->_vel.setX(ghostTR->_vel.getX() * -1);
			}

			// check upper/lower borders
			if (ghostTR->_pos.getY() < 0) {
				ghostTR->_pos.setY(0.0f);
				ghostTR->_vel.setY(ghostTR->_vel.getY() * -1);
			}
			//IMPORTANTE, tomamos en cuenta la posicion del fantasma
			else if (ghostTR->_pos.getY() + ghostTR->_height > sdlutils().height()) {
				ghostTR->_pos.setY(sdlutils().height() - ghostTR->_height);
				ghostTR->_vel.setY(ghostTR->_vel.getY() * -1);
			}

			//Actualizando frames de animacion
			if (_timerAnimation + 100 <= sdlutils().virtualTimer().currTime()) {
				_timerAnimation = sdlutils().virtualTimer().currTime();
				for (auto e : ghosts) {
					auto imgGhost = _mngr->getComponent<ImageWithFrames>(e);
					imgGhost->updateImage();
				}
			}

			ghostTR->_pos = ghostTR->_pos + ghostTR->_vel;
		}
	}

	//Si no hay inmunidad generamos, es instantaneo al volver a la normalidad por el tiempo acumulado
	//Tiempo de spawn y que el numero de fantasmas sea como maximo 10 para generar uno nuevo
	if (!_immunityPacman) {
		if (_timerSpawn + 5000 <= sdlutils().virtualTimer().currTime() && _numGhosts < 10) {
			_timerSpawn = sdlutils().virtualTimer().currTime();
			generateOneGhost();
			_numGhosts++;
		}
	}

}

void GhostSystem::generateOneGhost()
{
	//uso del randomNumberGenerator
	auto& r = sdlutils().rand();
	int rnd = r.nextInt(0,3); // 0,1,2,3 (para una de las 4 esquinas)

	auto ghost = _mngr->addEntity(ecs::grp::GHOSTS);
	auto pacman = _mngr->getHandler(ecs::hdlr::PACMAN);

	auto trPacman = _mngr->getComponent<Transform>(pacman);
	auto trGhost = _mngr->addComponent<Transform>(ghost);

	auto s = 50.0f;

	//Generacion en el centro de la pantalla
	//float x = ((sdlutils().width() - s)) / 2.0f;
	//float y = ((sdlutils().height() - s)) / 2.0f;

	float x = 0.0f;
	float y = 0.0f;

	//Eleccion de esquina
	switch (rnd) {
		case 0:
			x = 0.0f - s;
			y = 0.0f - s;
			break;
		case 1:
			x = sdlutils().width() + s;
			y = 0.0f - s;
			break;
		case 2:
			x = sdlutils().width() + s;
			y = sdlutils().height() + s;
			break;
		case 3:
			x = 0.0f - s;
			y = sdlutils().height() + s;
			break;
		default:
			break;
	}

	//Iniciamos su transform en la esquina designada
	trGhost->init(Vector2D(x, y), Vector2D(), s, s, 0.0f);
	//Primer movimiento
	trGhost->_vel = ((trPacman->_pos - trGhost->_pos).normalize() * 1.1f);
	//Añadimos el componente de imagen
	_mngr->addComponent<ImageWithFrames>(ghost, &sdlutils().images().at("sprites"), s, s, 0, 4, 7);
}

void GhostSystem::resetGhosts()
{
	//Matamos a todos los fantasmas
	killAllGhosts();
}

void GhostSystem::killAllGhosts()
{
	//Recorremos todos los fantamas
	auto ghosts = _mngr->getEntities(ecs::grp::GHOSTS);
	for (auto e : ghosts) {
		_mngr->setAlive(e,false);
	}
	_numGhosts = 0;
	_timerAnimation = 0;
	_timerSpawn = 0;
}

void GhostSystem::immunityGhosts()
{
	// Cambiar apariencia de fantasmas
	auto ghosts = _mngr->getEntities(ecs::grp::GHOSTS);
	for (auto g : ghosts) {

		auto img = _mngr->getComponent<ImageWithFrames>(g);

		img->setFil(6);
		img->setCol(3);
		img->setNumFrames(1);
		img->resetFramePosition();
	}
}

void GhostSystem::notimmunityGhosts()
{
	// Restaurar apariencia de fantasmas
	auto ghosts = _mngr->getEntities(ecs::grp::GHOSTS);
	for (auto g : ghosts) {
		auto img = _mngr->getComponent<ImageWithFrames>(g);

		img->setFil(0);
		img->setCol(4);
		img->setNumFrames(7);
		img->resetFramePosition();
	}
}

void GhostSystem::recieve(const Message& m)
{
	switch (m.id) {
	case _m_IMMUNITY_START:
		immunityGhosts();
		_immunityPacman = true;
		break;
	case _m_IMMUNITY_END:
		notimmunityGhosts();
		_immunityPacman = false;
		break;
	case _m_ROUND_START:
		break;
	case _m_ROUND_OVER:
		resetGhosts();
		break;
	case _m_GAME_OVER:
		resetGhosts();
		break;
	default:
		break;
	}
}
