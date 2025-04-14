// This file is part of the course TPV2@UCM - Samir Genaim

#include "Manager.h"

namespace ecs {

Manager::Manager() :
		_hdlrs(), //
		_entsByGroup(), //
		_sys(), //
		_msgs(), //
		_msgs_aux() //
{

	// for each group we reserve space for 100 entities,
	// just to avoid resizing
	//
	for (auto &groupEntities : _entsByGroup) {
		groupEntities.reserve(100);
	}

	// allocate enough space for the messages queue,
	// just to avoid resizing
	//
	_msgs.reserve(100);
	_msgs_aux.reserve(100);
}

Manager::~Manager() {

	// delete all entities
	//
	for (auto &ents : _entsByGroup) {
		for (auto e : ents)
			delete e;
	}

	for (auto i = 0u; i < maxSystemId; i++)
		if (_sys[i] != nullptr)
			delete _sys[i];
}

void Manager::refresh() {

	// remove dead entities from the groups lists, and also those
	// do not belong to the group anymore
	for (ecs::grpId_t gId = 0; gId < ecs::maxGroupId; gId++) {
		auto &groupEntities = _entsByGroup[gId];
		groupEntities.erase(
				std::remove_if(groupEntities.begin(), groupEntities.end(),
						[](entity_t e) {
							if (e->_alive) {
								return false;
							} else {
								delete e;
								return true;
							}
						}), groupEntities.end());
	}

}

} // end of namespace
