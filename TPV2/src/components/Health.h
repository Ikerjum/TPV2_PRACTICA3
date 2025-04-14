#pragma once
#include "../ecs/Component.h"

class Health : public ecs::Component {
public:
    //__CMPID_DECL__(ecs::cmp::HEALTH)

    Health(int lives = 3);
    ~Health();

    void initComponent() override;

    void loseLife();
    void reset();
    int getLives() const;

    // Métodos para ImmunitySystem
    void activateImmunity(uint32_t startTime);
    void deactivateImmunity();
    bool isImmune() const;
    uint32_t getImmunityStartTime() const;

private:
    int _lives;
    int _initialLives;
    bool _isImmune;
    uint32_t _immunityStartTime;
};