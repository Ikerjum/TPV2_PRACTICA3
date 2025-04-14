// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include "../ecs/System.h"

#include "../utils/Vector2D.h"

struct Transform;
class Texture;
class ImageWithFrames;

class PacManSystem: public ecs::System {
public:

	PacManSystem();
	virtual ~PacManSystem();
	void initSystem() override;
	void update() override;

	//metodos por mensajes
	void resetPacmanPosition();
	void loseRound();
	void resetLives();
	void immunityEnd();

	void recieve(const Message& m) override;

	inline auto getScore() {
		return _score;
	}

	inline void setScore(unsigned int score) {
		_score = score;
	}

	inline void incrScore(unsigned int n) {
		_score += n;
	}

private:
	Transform *_pmTR;
	ImageWithFrames* _pmIWF;
	Texture* _tex;
	Vector2D _actFrame;
	float _timer;
	unsigned int _score;
};

