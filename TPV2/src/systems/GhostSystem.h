#pragma once

#include "../ecs/System.h"

#include <vector>

struct Transform;
class Vector2D;

class GhostSystem : public ecs::System {
public:

	GhostSystem();
	virtual ~GhostSystem();
	void initSystem() override;
	void update() override;
	void generateOneGhost();
	void resetGhosts();
	//voids para mensajes recibidos
	void killAllGhosts();
	void immunityGhosts();
	void notimmunityGhosts();
	//
	//recieve de mensajes, override, recibe un mensaje y ejecuta una función especifica del propio sistema
	void recieve(const Message& m) override;
private:
	int _numGhosts;
	float _timerAnimation;
	float _timerSpawn;
	bool _immunityPacman;
};
