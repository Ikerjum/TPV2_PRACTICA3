// This file is part of the course TPV2@UCM - Samir Genaim

#include <iostream>
#include <fstream>

#include "game/Game.h"
#include "game/UDPServer.h"
#include "sdlutils/SDLNetUtils.h"

void server(Uint16 port) {
	UDPServer s(port, 10);
	s.listen();
}

void client(char* host, Uint16 port) {
	if (Game::Init()) {
		if (Game::Instance()->initGame("resources/maps/little_wolf/map_0.json",host,port)) {
			Game::Instance()->start();
		}
		Game::Release();
	}
}

void start(int argc, char** argv) {

	SDLNetUtils::initSDLNet();

	if (argc == 3 && strcmp(argv[1], "server") == 0) {
		server(static_cast<Uint16>(atoi(argv[2]))); // start in server mode
	}
	else if (argc == 4 && strcmp(argv[1], "client") == 0) {
		client(argv[2], static_cast<Uint16>(atoi(argv[3]))); // start in client mode
	}
	else {
		std::cout << "Usage: " << std::endl;
		std::cout << "  " << argv[0] << " server port " << std::endl;
		std::cout << "  " << argv[0] << " client host port " << std::endl;
		std::cout << std::endl;
		std::cout << "Example:" << std::endl;
		std::cout << "  " << argv[0] << " server 2000" << std::endl;
		std::cout << "  " << argv[0] << " client localhost 2000" << std::endl;
	}

	// finalize SDLNet
	SDLNetUtils::closeSDLNet();

}

int main(int argc, char** argv) {

	try {
		start(argc, argv);
		//Game g;
		//g.init("resources/maps/little_wolf/map_0.json");
		//g.start();
		
		//if (Game::Init()) {
		//	Game::Instance()->initGame("resources/maps/little_wolf/map_0.json");
		//	Game::Instance()->start();
		//	Game::Release();
		//}
	} catch (const std::string &e) { // catch exceptions thrown as strings
		std::cerr << e << std::endl;
	} catch (const char *e) { // catch exceptions thrown as char*
		std::cerr << e << std::endl;
	} catch (const std::exception &e) { // catch exceptions thrown as a sub-type of std::exception
		std::cerr << e.what();
	} catch (...) {
		std::cerr << "Caught and exception of unknown type ...";
	}

	return 0;
}

