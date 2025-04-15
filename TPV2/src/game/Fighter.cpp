// This file is part of the course TPV2@UCM - Samir Genaim

#include "Fighter.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

#include "../sdlutils/InputHandler.h"
#include "../sdlutils/macros.h"
#include "../sdlutils/SDLUtils.h"
#include "../sdlutils/Texture.h"

#include "Game.h"
#include "Bullets.h"
#include "Networking.h"

Fighter::Fighter() :
		_curr_player_id(0), //
		_fighter_img(sdlutils().images().at("fighter")), //
		_ihdlr(ih()), //
		_show_help(true) {
}

Fighter::~Fighter() {
}

void Fighter::addPlayer(std::uint8_t id) {
	initPlayer(id);
	_curr_player_id = id;
	send_my_info();
}

void Fighter::initPlayer(std::uint8_t id) {
	assert(id < _players.size() && _players[id].state == NOT_USED);

	auto &rand = sdlutils().rand();

	int x = rand.nextInt(0, sdlutils().width() - 40);
	int y = rand.nextInt(0, sdlutils().height() - 40);

	_players[id] = { id, Vector2D(), Vector2D(x, y), 40, 40, 2.0f, 0.0f, ALIVE,
			0u };

}

void Fighter::update() {

	Player &p = _players[_curr_player_id];

	// dead player don't move/spin/shoot
	if (p.state != ALIVE)
		return;

	spin(p);  // handle spinning
	move(p);  // handle moving
	shoot(p); // handle shooting

	// move
	p.pos = p.pos + p.vel;

	// deaccelerate
	p.vel = p.vel * 0.995f;
	if (p.vel.magnitude() < 0.05f)
		p.vel.set(0.0f, 0.0f);

	// show at opposite side
	if (p.pos.getY() + p.height < 0.0f) {
		p.pos.setY(sdlutils().height() - 1);
	} else if (p.pos.getY() > sdlutils().height()) {
		p.pos.setY(-p.height + 1);
	}

	if (p.pos.getX() + p.width < 0.0f) {
		p.pos.setX(sdlutils().width() - 1);
	} else if (p.pos.getX() > sdlutils().width()) {
		p.pos.setX(-p.width + 1);
	}

	Game::Instance()->get_networking().send_state(p.pos, p.width, p.height,
			p.rot);

	if (_ihdlr.keyDownEvent()) {

		// toggle help
		if (_ihdlr.isKeyDown(SDL_SCANCODE_T)) {
			_show_help = !_show_help;
		}

		// R brings deads to life
		if (_ihdlr.isKeyDown(SDL_SCANCODE_R)) {
			Game::Instance()->get_networking().send_restart();
		}
	}
}

void Fighter::render() {

	for (auto i = 0u; i < _players.size(); i++) {
		Player &p = _players[i];
		if (p.state == ALIVE) {
			SDL_Rect dest = build_sdlrect(p.pos, p.width, p.height);
			_fighter_img.render(dest, p.rot);
		}
	}

	if (_players[_curr_player_id].state == ALIVE)
		draw_limits(_players[_curr_player_id]);

	std::string msg = "P " + std::to_string(_curr_player_id);
	Texture t(sdlutils().renderer(), msg, sdlutils().fonts().at("MFR24"),
			build_sdlcolor(0xffffffff));

	SDL_Rect dest = { 10, 10, 20, 20 };
	t.render(dest);

// show help
	if (_show_help) {
		int y = sdlutils().height();
		for (const char *s : { "usage_1", "usage_2", "usage_3", "usage_4",
				"usage_6" }) {
			auto &t = sdlutils().msgs().at(s);
			y = y - t.height();
			t.render(0, y);
		}
	}
}

void Fighter::draw_limits(Player &p) {
	SDL_Renderer *renderer = sdlutils().renderer();

// assuming the (0,0) point is the middle of the object, the following are
// vectors to the corners of its bounding rectangle
	Vector2D lu = Vector2D(-p.width / 2.0f, -p.height / 2.0f);
	Vector2D ru = Vector2D(p.width / 2.0f, -p.height / 2.0f);
	Vector2D rb = Vector2D(p.width / 2.0f, p.height / 2.0f);
	Vector2D lb = Vector2D(-p.width / 2.0f, p.height / 2.0f);

// rotate the corners, so we get a rotated rectangle
	lu = lu.rotate(p.rot);
	ru = ru.rotate(p.rot);
	rb = rb.rotate(p.rot);
	lb = lb.rotate(p.rot);

// the center of the object
	float x = p.pos.getX() + p.width / 2.0f;
	float y = p.pos.getY() + p.height / 2.0f;

// draw lines between the corners, after shifting them by (x,y)
	SDL_SetRenderDrawColor(renderer, COLOREXP(build_sdlcolor(0x0000ff00)));
	SDL_RenderDrawLine(renderer, lu.getX() + x, lu.getY() + y, ru.getX() + x,
			ru.getY() + y);
	SDL_RenderDrawLine(renderer, ru.getX() + x, ru.getY() + y, rb.getX() + x,
			rb.getY() + y);
	SDL_RenderDrawLine(renderer, rb.getX() + x, rb.getY() + y, lb.getX() + x,
			lb.getY() + y);
	SDL_RenderDrawLine(renderer, lb.getX() + x, lb.getY() + y, lu.getX() + x,
			lu.getY() + y);

// draw center point
	SDL_SetRenderDrawColor(renderer, COLOREXP(build_sdlcolor(0xff000000)));
	SDL_Rect dest = build_sdlrect(x - 1, y - 1, 3, 3);
	SDL_RenderFillRect(renderer, &dest);

// draw velocity vector
	SDL_SetRenderDrawColor(renderer, COLOREXP(build_sdlcolor(0x00ff0011)));

	Vector2D vel = p.vel;
	float wh = std::min(p.height, p.width) / 2.0f; // minimum of width an height
	vel = vel * wh / 2.0f;
	SDL_RenderDrawLine(renderer, x, y, vel.getX() + x, vel.getY() + y);
}

void Fighter::move(Player &p) {
	if (_ihdlr.isKeyDown(SDL_SCANCODE_W)) {

		p.vel = p.vel + Vector2D(0, -1).rotate(p.rot) * 0.1f;
		if (p.vel.magnitude() > p.speed)
			p.vel = p.vel.normalize() * p.speed;
	}
}

void Fighter::spin(Player &p) {
	if (_ihdlr.isKeyDown(SDL_SCANCODE_L)) {
		p.rot += 5.0f;
	} else if (_ihdlr.isKeyDown(SDL_SCANCODE_H)) {
		p.rot -= 5.0f;
	}
}

void Fighter::shoot(Player &p) {
	if (_ihdlr.keyDownEvent() && _ihdlr.isKeyDown(SDL_SCANCODE_SPACE)) {
		if (sdlutils().virtualTimer().currTime() < p.lastShoot + 250)
			return;

		p.lastShoot = sdlutils().virtualTimer().currTime();

		auto c = p.pos + Vector2D(p.width / 2.0f, p.height / 2.0f);

		int bwidth = 5;
		int bheight = 20;

		Vector2D bp = c
				- Vector2D(0.0f, p.height / 2.0f + bheight / 2 + 10).rotate(
						p.rot) - Vector2D(bwidth / 2, bheight / 2);

		Vector2D bv = Vector2D(0, -1).rotate(p.rot)
				* (p.vel.magnitude() + 5.0f);

		Game::Instance()->get_networking().send_shoot(bp, bv, bwidth, bheight,
				Vector2D(0, -1).angle(bv));

	}
}

void Fighter::removePlayer(std::uint8_t id) {
	_players[id].state = Fighter::NOT_USED;
}

void Fighter::update_player_state(Uint8 id, float x, float y, float w, float h,
		float rot) {

	Player &p = _players[id];

	p.pos.set(x, y);
	p.id = id;
	p.width = w;
	p.height = h;
	p.rot = rot;

}

void Fighter::killPlayer(std::uint8_t id) {
	_players[id].state = Fighter::DEAD;
}

void Fighter::update_player_info(Uint8 id, float x, float y, float w, float h,
		float rot, uint8_t state) {
	Player &p = _players[id];

	p.pos.set(x, y);
	p.id = id;
	p.width = w;
	p.height = h;
	p.rot = rot;
	p.state = static_cast<PlayerState>(state);
}

void Fighter::send_my_info() {
	Player &p = _players[_curr_player_id];

	Game::Instance()->get_networking().send_my_info(p.pos, p.width, p.height,
			p.rot, p.state);
}

void Fighter::bringAllToLife() {
	for (auto i = 0u; i < _players.size(); i++) {
		Player &p = _players[i];
		if (p.state == DEAD) {
			p.state = ALIVE;
			p.vel.set(0.0f, 0.f);
		}
	}
}
