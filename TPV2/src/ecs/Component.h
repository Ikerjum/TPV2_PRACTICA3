// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include "ecs.h"

namespace ecs {

/*
 * Structs that inherit from Component must define a field
 *
 *   	constexpr static ecs::cmpId_t id = value;
 *
 * where value is from the enum ecs::cmpId (see ecs.h). This
 * how we assign numeric identifiers to components (so we can
 * easily put them in an array). The list of possible identifiers
 * is defined as an enum in ecs.h
 *
 * We use a struct to emphasise that it is actually a data structure
 * in ECS, and also to have default visibility as public.
 * *
 */
struct Component {
public:
	Component() :
			_ent() //
	{
	}

	// Destroys the component.
	// Careful! _ent not be destroyed
	//
	virtual ~Component() {
	}

	// This method is used to set the context, which is a
	// reference to the entity in which the component is
	// installed.
	//
	inline void setContext(Entity *ent) {
		_ent = ent;
	}

	// We assume that initComponent will be called when adding a
	// component to an entity, immediately after setContext. It is not
	// use much in ECS
	//
	virtual void initComponent() {
	}


protected:
	//

	// a reference to the entity, should not be deleted on destruction.
	// having a reference to the entity is sometimes useful if we have a
	// reference to a component and wants to retrieve another component
	// of the same entity. Can be removed as well!

	entity_t _ent;
};

} // end of namespace
