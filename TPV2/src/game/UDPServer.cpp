// This file is part of the course TPV2@UCM - Samir Genaim

#include "UDPServer.h"

#include <iostream>
#include <SDL_net.h>

#include "../sdlutils/SDLNetUtils.h"
#include "netwrok_messages.h"

UDPServer::UDPServer(const Uint16 port, uint8_t max_clients) {
	_sock = SDLNet_UDP_Open(port);
	if (!_sock) {
		SDLNetUtils::print_SDLNet_error_and_exit();
	}

	_p = SDLNet_AllocPacket(512);
	if (!_p) {
		SDLNetUtils::print_SDLNet_error_and_exit();
	}

	_socketSet = SDLNet_AllocSocketSet(1);
	SDLNet_UDP_AddSocket(_socketSet, _sock);

	_max_clients = max_clients;
	_clients = new ClientInfo[max_clients]();

	_done = false;
	_port = port;
}

UDPServer::~UDPServer() {
	// free the packet
	SDLNet_FreePacket(_p);

	// free the socket set
	SDLNet_FreeSocketSet(_socketSet);

	// close the server socket
	SDLNet_UDP_Close(_sock);

	// free the clients array
	delete[] _clients;
}

void UDPServer::listen() {

	Msg m0;
	MsgWithId m1;
	MsgWithMasterId m2;

	std::cout << "The server is up and kicking ..." << std::endl;

	bool done = false;
	while (!done) {

		// The call to SDLNet_CheckSockets returns the number of sockets with activity
		// in socketSet. The 2nd parameter tells the method to wait up to 1 hour
		// if there is no activity -- no need to put it 0 unless we really don't want to
		// block (like when used in a video game). With 0 it would consume CPU unnecessarily.
		if (SDLNet_CheckSockets(_socketSet, SDL_MAX_UINT32) > 0) {

			// when the server wakes up due to a trigger from method shutdown_server, the
			// value of done is true so we skip the loop body
			//
			if (done)
				continue;

			// if there is an activity in serverUDPSock we process it. Note that
			// before calling SDLNet_SocketReady we must have called SDLNet_CheckSockets
			if (SDLNet_SocketReady(_sock)) {

				// get the packet - returns -1, 0 or 1
				if (SDLNetUtils::deserializedReceive(m0, _p, _sock) > 0) { // a packet received in p
					// now we have a switch depending on the kind of message
					switch (m0._type) {

					// for future use ...
					case _NONE:
						break;

						// the message is a connection request
					case _CONNECTION_REQUEST: {

						// seek a free slot
						Uint8 j = 0;
						while (j < _max_clients && _clients[j].connected)
							j++;

						// if a free slot is found we will have j < MAX_CLIENT
						if (j < _max_clients) {

							// make the slot connected, and store the client's address
							_clients[j].connected = true;
							_clients[j].address = _p->address;

							// send a message to the client indicating that the connection was accepted
							m2._type = _CONNECTION_ACCEPTED;
							m2._client_id = j;
							m2._master_id = who_is_the_master(); // will not return -1 for sure
							SDLNetUtils::serializedSend(m2, _p, _sock); // IP is already in p_->address

							// tell all clients, except the sender, that a new client connected
							m2._type = _NEW_CLIENT_CONNECTED;
							send_to_all_except(m2, m2._client_id);

							// print information
							std::cout << "New client with id " << (int) j
									<< " connected from ";
							SDLNetUtils::print_ip(_p->address, true);

						} else {
							// if not free slots, send a message to the client rejecting the connection
							m0._type = _CONNECTION_REJECTED;
							SDLNetUtils::serializedSend(m0, _p, _sock); // IP is already in p_->address
						}
						break;
					}

						// the message is a client informing about disconnection
					case _DISCONNECTED: {

						// we need to deserialize again since we have des wrt Msg only
						m1.deserialize(_p->data);

						// mark the corresponding slot as free.
						_clients[m1._client_id].connected = false;

						// the new master, if any ...
						int the_master = who_is_the_master();

						if (the_master != -1) {

							m2._type = m1._type;
							m2._client_id = m1._client_id;
							m2._master_id = the_master;

							// tell all clients that someone has disconnected, and who is the new master
							send_to_all_except(m2);
						}

						std::cout << "Client  " << (int) m1._client_id
								<< " disconnected " << std::endl;

						break;
					}

					default: {
						// just forward the message to all clients -- the message must be a subtype
						// of MsgWithId

						// we need to deserialize again since m0 does not have the id
						send_packet_to_all_except(); // the message is already in the packet
						break;
					}
					}

				}
			}
		}

	}

}

void UDPServer::send_packet_to_all_except(int ex_id) {
	for (auto i = 0u; i < _max_clients; i++) {
		if (i != static_cast<Uint8>(ex_id) && _clients[i].connected) {
			_p->address = _clients[i].address;
			SDLNet_UDP_Send(_sock, -1, _p); // we just forward the whole packet, it is supposed to be ready
		}
	}
}

int UDPServer::who_is_the_master() {

	// the master is the client with the minimum identifier
	for (auto i = 0u; i < _max_clients; i++) {
		if (_clients[i].connected)
			return i;
	}

	return -1;
}

