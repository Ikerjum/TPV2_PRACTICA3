// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <SDL_net.h>
#include <SDL_stdinc.h>

#include "netwrok_messages.h"
class Vector2D;
class Littlewolf;

class Networking {
public:
	Networking();
	virtual ~Networking();

	bool init(char *host, Uint16 port, std::string& name);
	bool disconnect();
	void update();

	Uint8 client_id() {
		return _clientId;
	}

	bool is_master() {
		return _clientId == _masterId;
	}

	//void send_state(const Vector2D &pos, float w, float h, float rot);
	//void send_my_info(const Vector2D &pos, float w, float h, float rot,
	//		Uint8 state);

	void send_state(float whereX, float whereY, float velocityX, float velocityY, float speed, float acceleration,
		float theta, float fovA1, float fovA2, float fovB1, float fovB2);
	void send_my_info(float whereX, float whereY, float velocityX, float velocityY, float speed, float acceleration,
		float theta, float fovA1, float fovA2, float fovB1, float fovB2, uint8_t state, std::string name, int points, int health);

	//struct Player {
	//	uint8_t id;          // the id
	//	Line fov;            // focal view
	//	Point where;         // current position (wrt. to the grid)
	//	Point velocity;      // current velocity
	//	float speed;         // maximum speed
	//	float acceleration;  // acceleration
	//	float theta;         // rotation (in rad)
	//	PlayerState state;   // the state
	//};
	
	//void send_shoot(Vector2D p, Vector2D v, int width, int height, float r);
	void send_shoot(Uint8 shooterId);
	//void send_shoot(Vector2D p);
	void send_dead(Uint8 id);
	void send_restart();

	void send_sound(Uint8 soundType, float X, float Y);

	void send_points(Uint8 id);

	void send_health(Uint8 id, int distance, Uint8 shooterId);

private:

	void handle_new_client(Uint8 id);
	void handle_disconnet(Uint8 id);
	void handle_player_state(const PlayerStateMsg &m);
	void handle_player_info(const PlayerInfoMsg &m);
	void handle_shoot(const ShootMsg &m);
	void handle_dead(const MsgWithId &m);
	void handle_restart();

	void handle_sound(const SoundMsg &m);
	void handle_points(const PointsMsg& m);
	void handle_player_correction(const PlayerCorrectionMsg& m);
	void handle_health(const HealthMsg& m);

	UDPsocket _sock;
	SDLNet_SocketSet _socketSet;
	UDPpacket *_p;
	IPaddress _srvadd;
	Uint8 _clientId;
	Uint8 _masterId;
};

