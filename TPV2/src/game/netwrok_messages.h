// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include <SDL_stdinc.h>
#include "../sdlutils/SDLNetUtils.h"

enum MsgType : Uint8 {
	_NONE = 0, //
	_CONNECTION_REQUEST, //
	_CONNECTION_ACCEPTED, //
	_CONNECTION_REJECTED, //
	_DISCONNECTED,
	_NEW_CLIENT_CONNECTED, //
	_PLAYER_STATE, // sent frequently to boradcast the state
	_PLAYER_INFO, // sent only the first time the player connects
	_SHOOT, //
	_DEAD, //
	_RESTART, 
	_SOUND,
	_POINTS
};

struct Msg {
	Uint8 _type;

	_IMPL_SERIALIAZION_(_type)
};

struct MsgWithId: Msg {
	Uint8 _client_id;

	_IMPL_SERIALIAZION_WITH_BASE_(Msg,_client_id)
};

struct MsgWithMasterId: MsgWithId {
	Uint8 _master_id;
	_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId,_master_id)
};

struct PlayerStateMsg: MsgWithId {

	//float x;
	//float y;
	//int w;
	//int h;
	//float rot;

	float whereX;
	float whereY;
	float velocityX;
	float velocityY;
	float speed;
	float acceleration;
	float theta;
	float fovA1;
	float fovA2;
	float fovB1;
	float fovB2;
	//_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId, x,y,w,h,rot)
	_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId, whereX, whereY, velocityX, velocityY, speed, acceleration, theta, fovA1, fovA2, fovB1, fovB2)
};

struct PlayerInfoMsg: MsgWithId {

	//float x;
	//float y;
	//int w;
	//int h;
	//float rot;
	//Uint8 state;
	
	float whereX;
	float whereY;
	float velocityX;
	float velocityY;
	float speed;
	float acceleration;
	float theta;
	float fovA1;
	float fovA2;
	float fovB1;
	float fovB2;
	uint8_t state;
	char name[11];
	int points;

	//_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId, x,y,w,h,rot,state)
	_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId,whereX, whereY,velocityX, velocityY,speed,acceleration,theta, fovA1, fovA2, fovB1, fovB2, state, name, 11u, points)

};

struct ShootMsg: MsgWithId {

	//float x;
	//float y;
	//float vx;
	//float vy;
	//int w;
	//int h;
	//float rot;
	//NO SE PUEDE CON BOOLEANOS

	//_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId, x,y,vx,vy,w,h,rot)
	_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId)

};

struct SoundMsg : MsgWithId {
	Uint8 soundType;
	float originX;
	float originY;

	_IMPL_SERIALIAZION_WITH_BASE_(MsgWithId, soundType, originX, originY)
};

struct PointsMsg : MsgWithId {
	
};