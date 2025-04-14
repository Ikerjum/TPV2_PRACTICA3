// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <cstdint>
#include "../ecs/ecs.h"

using msgId_type = uint8_t;
enum msgId : msgId_type {
	_m_NEW_GAME,
	_m_ROUND_START,
	_m_ROUND_OVER,
	_m_GAME_OVER,
	_m_PACMAN_GHOST_COLLISION, //no usado, collisionSystem maneja directamente los efectos de la colision
	_m_PACMAN_FOOD_COLLISION,
	_m_IMMUNITY_START,
	_m_IMMUNITY_END
};


struct Message {
	msgId_type id;

	// if we use union we save memory, but then use only primitive
	// types, otherwise you will need to define constructors almost
	// every where.

	union {

		

		// _m_PACMAN_FOOD_COLLISION
		struct {
			ecs::entity_t fruit;
		} food_collision_data;

		//_m_IMMUNITY_START
		struct {
			ecs::entity_t pacman;
		} immunity_start_data;

		// _m_IMMUNITY_END
		struct {
			ecs::entity_t pacman;
		} immunity_end_data;


		//_m_GAME_OVER
		struct {
			bool win;
		} game_over_data;
	};

};
