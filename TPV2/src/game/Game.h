// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include <SDL_stdinc.h>
#include <vector>
#include "../utils/Singleton.h"

class LittleWolf;
class Networking;

class Game: public Singleton<Game> {
	friend Singleton<Game>;
	Game();
public:
	
	virtual ~Game();
	bool initGame(const char *map, char *host, Uint16 port);
	void start();

	LittleWolf& get_littleWolf() {
		return *_little_wolf;
	}

	Networking& get_networking() {
		return *_net;
	}


private:
	bool init();
	//void check_collisions(); LO CONTROLA LITTLEWOLF
	LittleWolf* _little_wolf;
	Networking* _net;
	
};

