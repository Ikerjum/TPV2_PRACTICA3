// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include <vector>

#include "../ecs/ecs.h"

#include "../utils/Singleton.h"

#include "GameState.h"

class Game : public Singleton<Game> {
private:
	friend class Singleton<Game>;
	Game();
	bool init();
public:
	void initGame();
	virtual ~Game();
	void start();
	enum State {
		RUNNING, PAUSED, NEWGAME, NEWROUND, GAMEOVER
	};
	inline void setState(State s) {
		_state->leave();
		switch (s) {
		case RUNNING:
			_state = _running_state;
			break;
		case PAUSED:
			_state = _paused_state;
			break;
		case NEWGAME:
			_state = _newgame_state;
			break;
		case NEWROUND:
			_state = _newround_state;
			break;
		case GAMEOVER:
			_state = _gameover_state;
			break;
		}
		_state->enter();
	}
	ecs::System* getPacmanSys() { return _pacmanSys; }
	ecs::System* getFoodSys() { return _foodSys; }
	ecs::System* getGhostsSys() { return _ghostsSys; }
	ecs::System* getrenderSys() { return _renderSys; }
	ecs::System* getcollisionSys() { return _collisionSys; }
	ecs::System* getImmunitySys() { return _immunitySys; }

	ecs::Manager* getManager() { return _mngr; }

	bool getVictory() { return _victory; }
	void setVictory(bool victory) { _victory = victory; }
private:
	ecs::Manager *_mngr;
	//SISTEMAS
	ecs::System *_pacmanSys;
	ecs::System *_gameCtrlSys;
	ecs::System* _ghostsSys;
	ecs::System *_foodSys;
	ecs::System *_renderSys;
	ecs::System *_collisionSys;
	ecs::System* _immunitySys;
	//ESTADOS
	GameState* _state;
	GameState* _paused_state;
	GameState* _running_state;
	GameState* _newgame_state;
	GameState* _newround_state;
	GameState* _gameover_state;
	//CONDICION VICTORIA
	bool _victory;

};

