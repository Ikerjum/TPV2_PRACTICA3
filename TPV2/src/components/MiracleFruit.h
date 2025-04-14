#pragma once

#include "../ecs/Component.h"

#pragma once
#include "../ecs/Component.h"

class MiracleFruit : public ecs::Component {
public:
  //  __CMPID_DECL__(ecs::_MIRACLE_FRUIT)

       MiracleFruit();
    virtual ~MiracleFruit();

    // Getters
    bool isMiracleState() const;
    float getNormalDuration() const;
    float getMiracleDuration() const;
    float getTimeInState() const;
    float getLastStateChange() const;

    // Setters
    void setMiracleState(bool state);
    void setNormalDuration(float duration);
    void setMiracleDuration(float duration);
    void setTimeInState(float time);
    void setLastStateChange(float time);

    // Métodos de utilidad
    void toggleState();
    void resetTimers(float currentTime);

private:
    bool miracleState_;             // Estado actual (normal/milagroso)
    float normalDuration_;          // N (10-20 segundos)
    float miracleDuration_;         // M (1-5 segundos)
    float timeInState_;             // Tiempo en el estado actual
    float lastStateChange_;         // Momento del último cambio de estado
};