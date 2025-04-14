#include "MiracleFruit.h"

MiracleFruit::MiracleFruit() :
    miracleState_(false),
    normalDuration_(0),
    miracleDuration_(0),
    timeInState_(0),
    lastStateChange_(0) {}

MiracleFruit::~MiracleFruit() {}

// Getters
bool MiracleFruit::isMiracleState() const { return miracleState_; }
float MiracleFruit::getNormalDuration() const { return normalDuration_; }
float MiracleFruit::getMiracleDuration() const { return miracleDuration_; }
float MiracleFruit::getTimeInState() const { return timeInState_; }
float MiracleFruit::getLastStateChange() const { return lastStateChange_; }

// Setters
void MiracleFruit::setMiracleState(bool state) { miracleState_ = state; }
void MiracleFruit::setNormalDuration(float duration) { normalDuration_ = duration; }
void MiracleFruit::setMiracleDuration(float duration) { miracleDuration_ = duration; }
void MiracleFruit::setTimeInState(float time) { timeInState_ = time; }
void MiracleFruit::setLastStateChange(float time) { lastStateChange_ = time; }


void MiracleFruit::toggleState() {
    miracleState_ = !miracleState_;
}

void MiracleFruit::resetTimers(float currentTime) {
    lastStateChange_ = currentTime;
    timeInState_ = 0.0f;

}