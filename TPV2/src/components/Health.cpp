#include "Health.h"
#include <cassert>
#include "../ecs/Manager.h"
#include "../sdlutils/SDLUtils.h"
#include "Transform.h"
#include "../sdlutils/Texture.h"
#include "../game/Game.h"

Health::Health(int initialLives) :
    _lives(initialLives),
    _initialLives(initialLives),
    _isImmune(false),
    _immunityStartTime(0)
{
}

Health::~Health() {}

void Health::initComponent() {
}



void Health::loseLife() {
    if (!_isImmune) { // Solo pierde vida si no es inmune
        _lives--;
    }
}

void Health::reset() {
    _lives = _initialLives;
    deactivateImmunity();
}

int Health::getLives() const {
    return _lives;
}

void Health::activateImmunity(uint32_t startTime) {
    if (!_isImmune) {
        _isImmune = true;
        _immunityStartTime = startTime;
    }
}

void Health::deactivateImmunity() {
    if(_isImmune)
    _isImmune = false;
    
}

bool Health::isImmune() const {
    return _isImmune;
}

uint32_t Health::getImmunityStartTime() const
{
    if (!_isImmune) {
        return 0;  
    }
    return _immunityStartTime;
}
