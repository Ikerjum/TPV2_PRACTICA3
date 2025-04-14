// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include "../ecs/System.h"

class Texture;
class Vector2D;
struct Transform;

class RenderSystem: public ecs::System {
public:

	RenderSystem();
	virtual ~RenderSystem();
	void initSystem() override;
	void update() override;
private:
	void drawStars();
	void drawGhosts();
	void drawPacMan();
	void drawMsgs();
	void drawHealth();
	void draw(Transform *tr, Texture *tex, Vector2D actFrame);
};

