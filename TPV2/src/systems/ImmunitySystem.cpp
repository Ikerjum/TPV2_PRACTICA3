#include "ImmunitySystem.h"
#include "../ecs/Manager.h"
#include "../components/Health.h"
#include "../components/Image.h"
#include "../sdlutils/SDLUtils.h"
#include "../components/ImageWithFrames.h"
ImmunitySystem::ImmunitySystem() {
    
}

ImmunitySystem::~ImmunitySystem() {
   
}

void ImmunitySystem::initSystem() {
    
}

void ImmunitySystem::update() {
    auto pacman = _mngr->getHandler(ecs::hdlr::PACMAN);
    auto health = _mngr->getComponent<Health>(pacman);

    // Comprobar tiempo de inmunidad
    if (health->isImmune() &&
        sdlutils().virtualTimer().currRealTime() - health->getImmunityStartTime() > 10000) {
        endImmunity(pacman);
    }
}

void ImmunitySystem::recieve(const Message& m) {
    
    switch (m.id) {
    case _m_IMMUNITY_START:
        startImmunity(m.immunity_start_data.pacman);
        break;
    default:
        break;
    }
}

void ImmunitySystem::startImmunity(ecs::entity_t pacman) {
    
    auto health = _mngr->getComponent<Health>(pacman);

    // Activar inmunidad
    health->activateImmunity(sdlutils().virtualTimer().currRealTime());

}

void ImmunitySystem::endImmunity(ecs::entity_t pacman) {
    auto health = _mngr->getComponent<Health>(pacman);
    health->deactivateImmunity();

    // Enviar mensaje de fin de inmunidad
    Message m;
    m.id = _m_IMMUNITY_END;
    _mngr->send(m);


}