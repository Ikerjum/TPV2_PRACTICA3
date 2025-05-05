// This file is part of the course TPV2@UCM - Samir Genaim

#include "Networking.h"
#include "netwrok_messages.h"
#include <iostream>

#include "../sdlutils/SDLNetUtils.h"
#include "../utils/Vector2D.h"
#include "Game.h"
#include "LittleWolf.h"
#include "../sdlutils/SDLUtils.h"
Networking::Networking() :
		_sock(), //
		_socketSet(), //
		_p(), //
		_srvadd(), //
		_clientId(), //
		_masterId() {
}

Networking::~Networking() {
}

bool Networking::init(char *host, Uint16 port,  std::string& name) {

	if (SDLNet_ResolveHost(&_srvadd, host, port) < 0) {
		SDLNetUtils::print_SDLNet_error();
		return false;
	}

	_sock = SDLNet_UDP_Open(0);
	if (!_sock) {
		SDLNetUtils::print_SDLNet_error();
		return false;
	}

	_p = SDLNet_AllocPacket(512);
	if (!_p) {
		SDLNetUtils::print_SDLNet_error();
		return false;
	}

	_socketSet = SDLNet_AllocSocketSet(1);
	SDLNet_UDP_AddSocket(_socketSet, _sock);

	Msg m0;
	MsgWithMasterId m1;

	// request to connect
	m0._type = _CONNECTION_REQUEST;
	m1._type = _CONNECTION_REQUEST;

	SDLNetUtils::serializedSend(m0, _p, _sock, _srvadd);

	bool connected = false;
	// wait 3sec for the response, if we don't get we stop (since done will be 'true')
	if (SDLNet_CheckSockets(_socketSet, 3000) > 0) {
		if (SDLNet_SocketReady(_sock)) {
			if (SDLNetUtils::deserializedReceive(m0, _p, _sock) > 0) {
				switch (m0._type) {
				case _CONNECTION_ACCEPTED:
					m1.deserialize(_p->data);
					_clientId = m1._client_id;
					_masterId = m1._master_id;
					std::cout << ">> " << (int) _masterId << std::endl;
					std::cout << ">> " << (int) _clientId << std::endl;
					connected = true;
					break;
				case _CONNECTION_REJECTED:
					break;
				}
			}
		}
	}

	if (!connected) {
		std::cerr << "Failed to get a client id" << std::endl;
		return false;
	}

#ifdef _DEBUG
	std::cout << "Connected with id " << (int) _clientId << std::endl;
#endif

	return true;
}

bool Networking::disconnect() {
	MsgWithId m;
	m._type = _DISCONNECTED;
	m._client_id = _clientId;
	return (SDLNetUtils::serializedSend(m, _p, _sock, _srvadd) > 0);

}

void Networking::update() {

	Msg m0;
	MsgWithMasterId m1;
	PlayerStateMsg m2;
	ShootMsg m3;
	MsgWithId m4;
	PlayerInfoMsg m5;
	SoundMsg m6;
	PointsMsg m7;
	PlayerCorrectionMsg m8;
	HealthMsg m9;

	while (SDLNetUtils::deserializedReceive(m0, _p, _sock) > 0) {
		//SE EJECUTA SOLO UNA VEZ
		//std::cout << "ACTUALIZANDO DESERIALIZED" << SDLNetUtils::deserializedReceive(m0, _p, _sock) << std::endl;

		switch (m0._type) {
		case _NEW_CLIENT_CONNECTED:
			m1.deserialize(_p->data);
			_masterId = m1._master_id;
			handle_new_client(m1._client_id);
			break;

		case _DISCONNECTED:
			m1.deserialize(_p->data);
			_masterId = m1._master_id;
			handle_disconnet(m1._client_id);
			break;

		case _PLAYER_STATE:
			m2.deserialize(_p->data);
			handle_player_state(m2);
			break;

		case _PLAYER_INFO:
			m5.deserialize(_p->data);
			handle_player_info(m5);
			break;

		case _SHOOT:
			m3.deserialize(_p->data);
			handle_shoot(m3);
			break;

		case _DEAD:
			m4.deserialize(_p->data);
			handle_dead(m4);
			break;

		case _RESTART:
			handle_restart();
			break;

		case _SOUND:
			m6.deserialize(_p->data);
			handle_sound(m6);
			break;
		case _POINTS:
			m7.deserialize(_p->data);
			handle_points(m7);
			break;
		case _PLAYER_CORRECTION:
			m8.deserialize(_p->data);
			handle_player_correction(m8);
			break;
		case _HEALTH :
			m9.deserialize(_p->data);
			handle_health(m9);
			break;
		default:
			break;
		}
	}
}

void Networking::handle_new_client(Uint8 id) {
	if (id != _clientId)
		Game::Instance()->get_littleWolf().send_my_info();
}

void Networking::handle_disconnet(Uint8 id) {
	Game::Instance()->get_littleWolf().removePlayer(id);
}

void Networking::send_state(float whereX, float whereY, float velocityX, float velocityY, float speed, float acceleration,
	float theta, float fovA1, float fovA2, float fovB1, float fovB2) {

	PlayerStateMsg m;

	m._type = _PLAYER_STATE;
	m._client_id = _clientId;
	m.whereX = whereX;
	m.whereY = whereY;
	m.velocityX = velocityX;
	m.velocityY = velocityY;
	m.speed = speed;
	m.acceleration = acceleration;
	m.theta = theta;
	m.fovA1 = fovA1;
	m.fovA2 = fovA2;
	m.fovB1 = fovB1;
	m.fovB2 = fovB2;
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::handle_player_state(const PlayerStateMsg &m) {
	if (m._client_id != _clientId) {
		LittleWolf& lw = Game::Instance()->get_littleWolf();
		if (is_master()) {
			bool isValid = lw.validate_movement(m._client_id, m.whereX, m.whereY);

			
			if (!isValid) {
				auto p = lw.getPlayers()[m._client_id];
				PlayerCorrectionMsg corr;
				corr._type = _PLAYER_CORRECTION;
				corr._client_id = m._client_id;
				corr.whereX = p.where.x; 
				corr.whereY = p.where.y;
				corr.velocityX = p.velocity.x;
				corr.velocityY = p.velocity.y;
				corr.speed = p.speed;
				corr.acceleration = p.acceleration;
				corr.theta = p.theta;
				corr.fovA1 = p.fov.a.x;
				corr.fovA2 = p.fov.a.y;
				corr.fovB1 = p.fov.b.x;
				corr.fovB2 = p.fov.b.y;
				SDLNetUtils::serializedSend(corr, _p, _sock, _srvadd);
				return; 
			}
		}

		
		lw.update_player_state(m._client_id, m.whereX, m.whereY,
			m.velocityX, m.velocityY, m.speed, m.acceleration, m.theta, m.fovA1, m.fovA2, m.fovB1, m.fovB2);
	}
}

void Networking::send_shoot(Uint8 shooterId) {
	ShootMsg m;
	m._type = _SHOOT;
	m._client_id = shooterId;

	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::handle_shoot(const ShootMsg &m) {
	
	
	
	if (is_master()) {
		Game::Instance()->get_littleWolf().checkCollisions(m._client_id);
			
	}
}

void Networking::send_dead(Uint8 id) {
	MsgWithId m;
	m._type = _DEAD;
	m._client_id = id;
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::handle_dead(const MsgWithId &m) {
	
	Game::Instance()->get_littleWolf().killPlayer(m._client_id);
}

void Networking::send_my_info(float whereX,float whereY, float velocityX, float velocityY, float speed, float acceleration,
	float theta, float fovA1, float fovA2, float fovB1, float fovB2, uint8_t state, std::string name, int points, int health) {

	PlayerInfoMsg m;

	m._type = _PLAYER_INFO;
	m._client_id = _clientId;
	m.whereX = whereX;
	m.whereY = whereY;
	m.velocityX = velocityX;
	m.velocityY = velocityY;
	m.speed = speed;
	m.acceleration = acceleration;
	m.theta = theta;
	m.fovA1 = fovA1;
	m.fovA2 = fovA2;
	m.fovB1 = fovB1;
	m.fovB2 = fovB2;
	m.state = state;
	m.points = points;
	m.health = health;
	Game::Instance()->string_to_chars(name, m.name);
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::handle_player_info(const PlayerInfoMsg &m) {
	if (m._client_id != _clientId) {
		Game::Instance()->get_littleWolf().update_player_info(m._client_id, m.whereX, m.whereY,
				m.velocityX, m.velocityY, m.speed, m.acceleration, m.theta, m.fovA1, m.fovA2, m.fovB1, m.fovB2, m.state, m.name, m.points, m.health);
	}
}

void Networking::send_restart() {
	if (is_master()) { //El master solo manda el mensaje
		Msg m;
		m._type = _RESTART;
		SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
	}
}

void Networking::send_sound(Uint8 SoundType, float X, float Y)
{
	SoundMsg m;
	m._type = _SOUND;
	m._client_id = _clientId;
	m.soundType = SoundType;
	m.originX = X;
	m.originY = Y;
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::send_points(Uint8 id)
{
	PointsMsg m;
	m._type = _POINTS;
	m._client_id = id;
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::send_health(Uint8 id, int distance, Uint8 shooterId)
{
	HealthMsg m;
	m._type = _HEALTH;
	m._client_id = id;
	m.distance = distance;
	m.shooter = shooterId;
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);

}

void Networking::handle_restart() { //Se comprueba si es el master antes de esto
	
	Game::Instance()->get_littleWolf().setBeginTimerToRestart(false); //Necesario porque sino se ejecuta dos veces

	Game::Instance()->get_littleWolf().setTimerToRestart(0); //Necesario porque sino se ejecuta dos veces


	if (is_master()) {
		std::cout << "restartall se ejecuta en master" << std::endl;
		Game::Instance()->get_littleWolf().RestartAll();
	}	
	
}

void Networking::handle_sound(const SoundMsg& m)
{
	if (m._client_id != _clientId)
	{
		LittleWolf::Point point = { m.originX, m.originY };
		Game::Instance()->get_littleWolf().playSound(point, m.soundType);
	}
}

void Networking::handle_points(const PointsMsg& m)
{
	
	Game::Instance()->get_littleWolf().managePoints(m._client_id);
}

void Networking::handle_player_correction(const PlayerCorrectionMsg& m)
{
	Game::Instance()->get_littleWolf().update_player_state(
		m._client_id,
		m.whereX, m.whereY,
		m.velocityX, m.velocityY,
		m.speed, m.acceleration,
		m.theta, m.fovA1, m.fovA2, m.fovB1, m.fovB2
	);
}

void Networking::handle_health(const HealthMsg& m)
{
	 
		if (Game::Instance()->get_littleWolf().getDamage(m._client_id, 100 - m.distance)) {
			if (m.shooter == _clientId)
			send_points(m.shooter);
			send_dead(m._client_id);
		}
	
}
