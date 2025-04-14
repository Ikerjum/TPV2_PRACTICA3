// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <cassert>

#include "Component.h"
#include "ecs.h"
#include "messages.h"
#include "Entity.h"
#include "System.h"

namespace ecs {

/*
 * A class for managing the list of entities, groups, etc.
 *
 * In this version the manager is responsible on several things
 * that used to be done by the class Entity (like addComponent), just
 * to make it easier to develop the memory management later.
 *
 * Note that no outside the classes of ECS can access an Entity directly,
 * they just have a reference to it.
 *
 *  */
class Manager {

public:
	Manager();
	virtual ~Manager();

	// Adding an entity simply creates an instance of Entity, adds
	// it to the list of the given group and returns it to the caller.
	//
	inline entity_t addEntity(ecs::grpId_t gId = ecs::grp::DEFAULT) {

		// create and initialise the entity
		auto e = new Entity(gId);
		e->_alive = true;

		// add the entity 'e' to list of entities of the given group
		//
		// IMPORTANT NOTE:
		//
		// Currently we immediately add the entity to the list of entities,
		// so we will actually see them in this 'frame' if we traverse the list of
		// entities afterwards!
		//
		// A better solution would be to add them to an auxiliary list, and
		// define a method 'flush()' that moves them from the auxiliary list
		// to the corresponding list of entities.
		//
		// We will have to call 'flush()' in each iteration of the
		// main loop. This way we guarantee that entities that are added in one
		// 'frame' they will appear only in the next 'frame' -- I leave it as an
		// exercise for you ... it could be incorporated in 'refresh' as well.
		//
		_entsByGroup[gId].push_back(e);

		// return it to the caller
		//
		return e;
	}

	// Setting the state of entity 'e' (alive or dead)
	//
	inline void setAlive(entity_t e, bool alive) {
		e->_alive = alive;
	}

	// Returns the state of the entity 'e' (alive o dead)
	//
	inline bool isAlive(entity_t e) {
		return e->_alive;
	}

	// Adds a component to the entity 'e'. It receives the type
	// T (to be created), and the list of arguments (if any) to
	// be passed to the constructor. The component identifier
	// 'cId' is taken from cmpId<T>.
	//
	template<typename T, typename ...Ts>
	inline T* addComponent(entity_t e, Ts &&... args) {

		constexpr cmpId_t cId = cmpId<T>;
		assert(cId < maxComponentId);

		// delete the current component, if any
		//
		removeComponent<T>(e);

		// create, initialise and install the new component
		//
		Component *c = new T(std::forward<Ts>(args)...);
		c->setContext(e);
		c->initComponent();
		e->_cmps[cId] = c;

		// return it to the user so i can be initialised if needed
		return static_cast<T*>(c);
	}

	// Removes the component of Entity 'e' at position cmpId<T>.
	//
	template<typename T>
	inline void removeComponent(entity_t e) {

		constexpr cmpId_t cId = cmpId<T>;
		assert(cId < maxComponentId);

		if (e->_cmps[cId] != nullptr) {

			// destroy it
			//
			delete e->_cmps[cId];

			// remove the pointer
			//
			e->_cmps[cId] = nullptr;
		}
	}

	// Returns the component that corresponds to position cmpId<T>, casting it
	// to T*. The casting is done just for ease of use, to avoid casting
	// outside.
	//
	template<typename T>
	inline T* getComponent(entity_t e) {

		constexpr cmpId_t cId = cmpId<T>;
		assert(cId < maxComponentId);

		return static_cast<T*>(e->_cmps[cId]);
	}

	// returns true if there is a component with identifier cmpId<T>
	// in the entity 'e'
	//
	template<typename T>
	inline bool hasComponent(entity_t e) {

		constexpr cmpId_t cId = cmpId<T>;
		assert(cId < maxComponentId);

		return e->_cmps[cId] != nullptr;
	}

	// returns the group 'gId' of entity 'e'
	//
	inline grpId_t groupId(entity_t e) {
		return e->_gId;
	}

	// returns the vector of all entities of a given group
	//
	inline const auto& getEntities(grpId_t gId = grp::DEFAULT) {
		return _entsByGroup[gId];
	}

	// associates the entity 'e' to the handler 'hId'
	//
	inline void setHandler(hdlrId_t hId, entity_t e) {
		assert(hId < maxHandlerId);
		_hdlrs[hId] = e;
	}

	// returns the entity associated to the handler 'hId'
	//
	inline entity_t getHandler(hdlrId_t hId) {
		assert(hId < maxHandlerId);
		return _hdlrs[hId];
	}

	// Adds a System to the manager. It receives the type
	// T of the system (to be created), and the list of
	// arguments (if any) to be passed to the system. The
	// system's identifier 'cId' is taken from sysId<T>.
	//
	template<typename T, typename ...Ts>
	inline T* addSystem(Ts &&... args) {

		constexpr sysId_t sId = sysId<T>;
		assert(sId < maxSystemId);

		removeSystem<T>();

		// create, initialise and install the new component
		//
		System *s = new T(std::forward<Ts>(args)...);
		s->setContext(this);
		s->initSystem();
		_sys[sId] = s;

		// return it to the user so it can be initialised if needed
		return static_cast<T*>(s);
	}

	// Removes the system at position sysId<T>.
	//
	template<typename T>
	inline void removeSystem() {

		constexpr sysId_t sId = sysId<T>;
		assert(sId < maxSystemId);

		if (_sys[sId] != nullptr) {

			// destroy it
			//
			delete _sys[sId];

			// remove the pointer
			//
			_sys[sId] = nullptr;
		}
	}

	// Returns the system that corresponds to position sysId<T>, casting it
	// to T*. The casting is done just for ease of use, to avoid casting
	// outside.
	//
	template<typename T>
	inline T* getSystem() {

		constexpr sysId_t sId = sysId<T>;
		assert(sId < maxSystemId);

		return static_cast<T*>(_sys[sId]);
	}

	inline void send(const Message &m, bool delay = false) {
		if (!delay) {
			for (System *s : _sys) {
				if (s != nullptr)
					s->recieve(m);
			}
		} else {
			// will make a copy of m, we could use std::move to move it
			// but then we will have to remove const from above
			_msgs.emplace_back(m);
		}
	}

	// this method should be called in the main loop to send queued
	// messages, i.e., those were sent using send(m,true)
	//
	inline void flushMessages() {

		// we traverse until msgs_.size(), so if new message
		// we added we don't send them now. If you wish to send
		// them as will you should write this loop in a different way
		// and maybe using std::list would be better.
		//
		auto size = _msgs.size();
		for (auto i = 0u; i < size; i++) {
			auto &m = _msgs[i];
			for (System *s : _sys) {
				if (s != nullptr)
					s->recieve(m);
			}
		}

		// delete all message that we have sent. This might be expensive
		// since it has to shift all remaining elements to the left. A better
		// solution would be to keep two vectors 'v1' and 'v2', when sending a
		// message we always add it to 'v1' and in flush we swap them and send
		// all messages in v2. After flush we simply clear v2
		//
		_msgs.erase(_msgs.begin(), _msgs.begin() + size);
	}

	// THIS IS THE VERSION THAT SWAP QUEUES, IF YOU WANT TO USE IT
	//
	// this method should be called in the main loop to send queued
	// messages, i.e., those were sent using send(m,true)
	//
	inline void flushMessagesWithSwap() {
		std::swap(_msgs, _msgs_aux);
		for (auto &m : _msgs_aux) {
			for (System *s : _sys) {
				if (s != nullptr)
					s->recieve(m);
			}
		}

		_msgs_aux.clear();
	}
	

	void refresh();

private:

	std::array<entity_t, maxHandlerId> _hdlrs;
	std::array<std::vector<entity_t>, maxGroupId> _entsByGroup;
	std::array<System*, maxSystemId> _sys;

	std::vector<Message> _msgs;
	std::vector<Message> _msgs_aux;

};

} // end of namespace
