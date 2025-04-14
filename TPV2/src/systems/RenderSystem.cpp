// This file is part of the course TPV2@UCM - Samir Genaim

#include "RenderSystem.h"

#include "../components/Image.h"

#include "../components/ImageWithFrames.h"
#include "../components/Transform.h"
#include "../ecs/Manager.h"
#include "../sdlutils/macros.h"
#include "../sdlutils/SDLUtils.h"
#include "../sdlutils/Texture.h"
#include "../components/Health.h"
#include "PacManSystem.h"

RenderSystem::RenderSystem() {

}

RenderSystem::~RenderSystem() {
}

void RenderSystem::initSystem() {
}

void RenderSystem::update() {
	drawMsgs();
	drawStars();
	drawPacMan();
	drawGhosts();
	drawHealth();
}

void RenderSystem::drawStars() {
	// draw stars
	for (auto e : _mngr->getEntities(ecs::grp::FRUITS)) {
		auto tr = _mngr->getComponent<Transform>(e);
		auto IWF = _mngr->getComponent<ImageWithFrames>(e);
		auto tex = IWF->getTexture();
		draw(tr,tex,IWF->getActFrame());
	}
}

void RenderSystem::drawGhosts()
{
	// draw ghosts
	for (auto e : _mngr->getEntities(ecs::grp::GHOSTS)) {
		auto tr = _mngr->getComponent<Transform>(e);
		auto IWF = _mngr->getComponent<ImageWithFrames>(e);
		auto tex = IWF->getTexture();
		draw(tr, tex, IWF->getActFrame());
	}
}

void RenderSystem::drawPacMan() {

	auto e = _mngr->getHandler(ecs::hdlr::PACMAN);
	auto tr = _mngr->getComponent<Transform>(e);
	auto IWF = _mngr->getComponent<ImageWithFrames>(e);
	auto tex = IWF->getTexture();
	draw(tr,tex,IWF->getActFrame());
}


void RenderSystem::drawMsgs() {
	// draw the score
	//
	auto score = _mngr->getSystem<PacManSystem>()->getScore();

	Texture scoreTex(sdlutils().renderer(), std::to_string(score),
			sdlutils().fonts().at("ARIAL24"), build_sdlcolor(0x444444ff));

	SDL_Rect dest = build_sdlrect( //
			(sdlutils().width() - scoreTex.width()) / 2.0f, //
			10.0f, //
			scoreTex.width(), //
			scoreTex.height());

	scoreTex.render(dest);
}

void RenderSystem::drawHealth()
{
	auto e = _mngr->getHandler(ecs::hdlr::PACMAN);
	auto health = _mngr->getComponent<Health>(e);
	auto total = health->getLives();



	//Texture* heart(sdlutils().renderer(), &sdlutils().images().at("heart"));
	
	Texture* h = &sdlutils().images().at("heart");

	for (int i = 0; i < total;++i) {
		//pintar corazones
		SDL_Rect heart = build_sdlrect(i * 30, 0, 30, 30);
		h->render(heart, 0.0f);
	}

}

void RenderSystem::draw(Transform *tr, Texture *tex, Vector2D actFrame) {
	SDL_Rect frame = build_sdlrect(actFrame, tex->width()/8.0f, tex->height()/8.0f);
	SDL_Rect dest = build_sdlrect(tr->_pos, tr->_width, tr->_height);
	assert(tex != nullptr);
	tex->render(frame,dest,tr->_rot);
}
