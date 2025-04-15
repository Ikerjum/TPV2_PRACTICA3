// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <SDL_net.h>

class UDPServer {
public:
	UDPServer(const Uint16 port, uint8_t max_clients);
	virtual ~UDPServer();
	void listen();

private:

	template<typename T>
	inline void send_to_all_except(T &m, int ex_id = -1) {
		Uint8 *end = m.serialize(_p->data);
		_p->len = end - _p->data;
		send_packet_to_all_except(ex_id);
	}

	void send_packet_to_all_except(int ex_id = -1);
	int who_is_the_master();

	struct ClientInfo {
		bool connected;
		IPaddress address;
	};

	UDPsocket _sock;
	UDPpacket *_p;
	SDLNet_SocketSet _socketSet;
	uint8_t _max_clients;
	ClientInfo *_clients;
	Uint16 _port;
	bool _done;
};

