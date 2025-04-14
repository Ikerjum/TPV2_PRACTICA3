// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <array>
#include <bitset>
#include <cassert>
#include <vector>

#include "Component.h"
#include "ecs.h"

namespace ecs {

/*
 * A struct that represents a collection of components.
 *
 */
struct Entity {
public:
	Entity(grpId_t gId) :
			_cmps(), //
			_alive(),  //
			_gId(gId) //
	{
	}

	// we delete the copy constructor/assignment because it is
	// not clear how to copy the components
	//
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	// Exercise: define move constructor/assignment for class Entity

	// Destroys the entity
	//
	virtual ~Entity() {

		// we delete all available components
		//
		for (auto i = 0u; i < maxComponentId; i++)
			if (_cmps[i] != nullptr) {
				delete _cmps[i];
				_cmps[i] = nullptr;
			}
	}

private:

	// the field currCmps_ can be removed, and instead we can traverse cmps_
	// and process non-null elements. We keep it because sometimes the order
	// in which the components are executed is important

	friend Manager;

	// the list of components is not really needed when using systems,
	// but for now we keep it just in case
	//
	std::array<Component*, maxComponentId> _cmps;
	bool _alive;
	ecs::grpId_t _gId;
};

} // end of name space
