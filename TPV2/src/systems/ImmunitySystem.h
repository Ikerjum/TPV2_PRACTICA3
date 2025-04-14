#pragma once
#include "../ecs/System.h"
#include "../components/Health.h"

class ImmunitySystem : public ecs::System {
public:
   // __SYSID_DECL__(ecs::sys::IMMUNITY)

    ImmunitySystem();
    virtual ~ImmunitySystem();

    void initSystem() override;
    void update() override;
    void recieve(const Message& m) override;

private:
    void startImmunity(ecs::entity_t pacman);
    void endImmunity(ecs::entity_t pacman);
};