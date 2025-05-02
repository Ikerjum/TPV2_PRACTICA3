// This file is part of the course TPV2@UCM - Samir Genaim

#include "Networking.h"
#include "netwrok_messages.h"
#include <iostream>

#include "../sdlutils/SDLNetUtils.h"
#include "../utils/Vector2D.h"
#include "Game.h"
#include "LittleWolf.h"

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

bool Networking::init(char *host, Uint16 port) {

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

	while (SDLNetUtils::deserializedReceive(m0, _p, _sock) > 0) {
		//SE EJECUTA SOLO UNA VEZ
		//std::cout << "ACTUALIZANDO DESERIALIZED" << SDLNetUtils::deserializedReceive(m0, _p, _sock) << std::endl;

		//NO HAY PROBLEMAS DE TAMAÑO
		if (_p->len < sizeof(Msg)) {
			std::cerr << "Paquete recibido demasiado pequeño: " << _p->len << " bytes" << std::endl;
			continue;
		}

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
		//EL ERROR PERSISTE SI COMENTO TODO ESTE CASO
		case _PLAYER_STATE:
			m2.deserialize(_p->data);
			//LLEGA A EJECUTARSE SOLO UNA VEZ
			//std::cout << "Datos deserializados: whereX=" << m2.whereX
			//	<< " whereY=" << m2.whereY << " velocityX=" << m2.velocityX
			//	<< " velocityY=" << m2.velocityY << " speed=" << m2.speed
			//	<< " acceleration=" << m2.acceleration << " theta="
			//	<< m2.theta << std::endl;
			handle_player_state(m2);
			break;
		//SI COMENTO ESTE CASO EL ERROR DESAPARECE
		case _PLAYER_INFO:
			m5.deserialize(_p->data);
			handle_player_info(m5);
			break;

		case _SHOOT:
			//m3.deserialize(_p->data);
			//handle_shoot(m3);
			break;

		case _DEAD:
			m4.deserialize(_p->data);
			handle_dead(m4);
			break;

		case _RESTART:
			handle_restart();
			break;

		default:
			break;
		}
	}
}

void Networking::handle_new_client(Uint8 id) {
	//NO LLEGA A EJECUTARSE NUNCA
	//std::cout << "Nuevo cliente conectado: " << (int)id << std::endl;
	if (id != _clientId)
		Game::Instance()->get_littleWolf().send_my_info();
	//std::cout << "NUEVO CLIENTE CONECTADISIMO" << std::endl;
}

void Networking::handle_disconnet(Uint8 id) {
	Game::Instance()->get_littleWolf().removePlayer(id);
}

void Networking::send_state(float whereX, float whereY, float velocityX, float velocityY, float speed, float acceleration,
	float theta) {

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
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::handle_player_state(const PlayerStateMsg &m) {
	//LLEGA A EJECUTARSE SOLO UNA VEZ
	std::cout << "PROCESANDO ESTADO DEL JUGADOR CON ID: " << (int)m._client_id << std::endl;
	if (m._client_id != _clientId) {
		Game::Instance()->get_littleWolf().update_player_state(m._client_id, m.whereX, m.whereY,
			m.velocityX, m.velocityY, m.speed, m.acceleration, m.theta);
	}
	//LLEGA A EJECUTARSE SOLO UNA VEZ
	std::cout << "ESTADO DEL JUGADOR PROCESADO" << std::endl;
}

void Networking::send_shoot(Vector2D p, Vector2D v, int width, int height,
		float r) {
	ShootMsg m;
	m._type = _SHOOT;
	m._client_id = _clientId;
	m.x = p.getX();
	m.y = p.getY();
	m.vx = v.getX();
	m.vy = v.getY();
	m.w = width;
	m.h = height;
	m.rot = r;
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::handle_shoot(const ShootMsg &m) {
	LittleWolf::Player p;
	p.where.x = m.x;
	p.where.y = m.y;
	Game::Instance()->get_littleWolf().shoot(p);

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
	float theta, uint8_t state) {

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
	m.state = state;
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::handle_player_info(const PlayerInfoMsg &m) {
	if (m._client_id != _clientId) {
		Game::Instance()->get_littleWolf().update_player_info(m._client_id, m.whereX, m.whereY,
				m.velocityX, m.velocityY, m.speed, m.acceleration, m.theta, m.state);
	}
}

void Networking::send_restart() {
	Msg m;
	m._type = _RESTART;
	SDLNetUtils::serializedSend(m, _p, _sock, _srvadd);
}

void Networking::handle_restart() {
	Game::Instance()->get_littleWolf().bringAllToLife();
}
