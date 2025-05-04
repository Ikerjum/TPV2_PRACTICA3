// This file is part of the course TPV2@UCM - Samir Genaim

#include "LittleWolf.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "../json/JSONValue.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../sdlutils/Texture.h"
//EDITABLE
#include "Game.h"
#include "Networking.h"

using std::string;
LittleWolf::LittleWolf() :
		_show_help(true), //
		_xres(), //
		_yres(), //
		_walling_width(), //
		_walling_height(), //
		_shoot_distace(), // the shoot distance -- not that it's wrt to the walling size
		_map(), //
		_players(), //
		_curr_player_id(0) { // we start with player 0
}

LittleWolf::~LittleWolf() {
	// nothing to delete, the walling are delete in the Map's destructor
}

void LittleWolf::checkCollisions(std::uint8_t id, int hit)
{
	if (hit == 1) {
		getPlayers()[id].state = DEAD;
	}
}

void LittleWolf::RestartAll()
{

	for (auto i = 0u; i < _max_player; i++) {

		if (_players[i].state != NOT_USED)
		{
			uint8_t playerId = _players[i].id;
			auto& rand = sdlutils().rand();
			// The search for an empty cell start at a random position (orow,ocol)
			uint16_t orow = rand.nextInt(0, _map.walling_height);
			uint16_t ocol = rand.nextInt(0, _map.walling_width);

			// search for an empty cell
			uint16_t row = orow;
			uint16_t col = (ocol + 1) % _map.walling_width;
			while (!((orow == row) && (ocol == col)) && _map.walling[row][col] != 0) {
				col = (col + 1) % _map.walling_width;
				if (col == 0)
					row = (row + 1) % _map.walling_height;
			}

			// handle the case where the search is failed, which in principle should never
			// happen unless we start with map with few empty cells
			if (row >= _map.walling_height)
				continue;

			_map.walling[(int)_players[i].where.y][(int)_players[i].where.x] = 0;
			_players[i].state = ALIVE; // mark the player alive
			_players[i].where.x = col + 0.5f; // where to put the player
			_players[i].where.y = row + 0.5f; // where to put the player
			_map.walling[(int)_players[i].where.y][(int)_players[i].where.x] = player_to_tile(playerId);

			bringAllToLife();
		}
	}
}

bool LittleWolf::RegisterMoreThanTwoLifes()
{
	int num_alive = 0;

	for (auto i = 0u; i < _max_player; i++) {
		if (_players[i].state == ALIVE) {
			num_alive++;
		}
	}

	if (num_alive < 2) {
		return false;
	}

	return true;
}

int LittleWolf::getPlayersUsed()
{
	int p = 0;
	for (auto i = 0u; i < _max_player; i++) {
		if (_players[i].state == ALIVE || _players[i].state == DEAD) {
			p++;
		}
	}
	return p;
}


void LittleWolf::init(SDL_Window *window, SDL_Renderer *render) {
	// for some reason it is created with a rotation of 90 degrees -- must be easier to
	// manipulate coordinates
	SDL_Texture *const texture = SDL_CreateTexture(sdlutils().renderer(),
			SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, _yres,
			_xres);

	_gpu = { window, render, texture, _xres, _yres };

}

void LittleWolf::update() {

	auto &ihdlr = ih();

	if (ihdlr.keyDownEvent()) {
		// toggle help
		if (ihdlr.isKeyDown(SDL_SCANCODE_T)) {
			_show_help = !_show_help;
		}

		//BORRADO POR ENUNCIADO APARTADO 9
		// R brings deads to life
		//if (ihdlr.isKeyDown(SDL_SCANCODE_R)) {
		//	bringAllToLife();
		//}

		if (ihdlr.isKeyDown(SDL_SCANCODE_M)) {
			seeMap = !seeMap;
		}
	}

	Player &p = _players[_curr_player_id];


	if (getPlayersUsed() > 1) {
		if (!RegisterMoreThanTwoLifes()) {
			if (!beginTimerToRestart) {
				timerToRestart = sdlutils().currRealTime() + 5000;
				beginTimerToRestart = true;
			}
			std::cout << "timerToRestart: " << timerToRestart - sdlutils().currRealTime() << std::endl;
		}
	}

	if (timerToRestart != 0 && sdlutils().currRealTime() > timerToRestart && beginTimerToRestart) {
		Game::Instance()->get_networking().send_restart();
		//RestartAll();

		timerToRestart = 0;
		beginTimerToRestart = false;
		return;
	}
	

	// dead player don't move/spin/shoot
	if (p.state != ALIVE)
		return;

	spin(p);  // handle spinning
	move(p);  // handle moving
	shoot(p); // handle shooting

	Game::Instance()->get_networking().send_state(p.where.x, p.where.y, p.velocity.x, p.velocity.y, p.speed, p.acceleration,
		p.theta);

}

void LittleWolf::load(std::string filename) {

	// Load JSON walling file.
	std::unique_ptr<JSONValue> jValueRoot(JSON::ParseFromFile(filename));

	// check it was loaded correctly
	// the root must be a JSON object
	if (jValueRoot == nullptr || !jValueRoot->IsObject()) {
		throw "Something went wrong while load/parsing the walling file'"
				+ filename + "'";
	}

	// we know the root is JSONObject
	JSONObject root = jValueRoot->AsObject();
	JSONValue *jValue = nullptr;

	// ** read the user walling

	// - uh = user wall height, uw = user wall width, and sf = scaling factor
	// - the resolution of the window will be uw*sf x uh*sf
	uint16_t uh = 0, uw = 0, sf = 0;

	// read the scaling factor
	jValue = root["scaling_factor"];
	if (jValue != nullptr) {
		if (jValue->IsNumber()) {
			sf = static_cast<uint16_t>(jValue->AsNumber());
		} else {
			throw "'Missing or incorrect value for scaling_factor";
		}
	} else {
		sf = 2;
	}

	if (sf % _walling_size_factor != 0)
		throw "Scaling factor should be divisible by '"
				+ std::to_string(_walling_size_factor) + ": "
				+ std::to_string(sf);

#ifdef _DEBUG
	std::cout << "Resolution scaling factor: " << (int) sf << std::endl;
#endif

	jValue = root["walling"];
	JSONArray json_walling;

	if (jValue != nullptr) {
		if (jValue->IsArray()) {
			json_walling = jValue->AsArray();
			uh = json_walling.size();
			assert(uh > 0);
			uw = json_walling[0]->AsString().size();
		}
	}

	_xres = uw * sf;
	_yres = uh * sf;

#ifdef _DEBUG
	std::cout << "Walling size: " << (int) uw << "x" << (int) uh << std::endl;
	std::cout << "Window size: " << (int) _xres << "x" << (int) _yres
			<< std::endl;
#endif

	_walling_width = _xres / _walling_size_factor;
	_walling_height = _yres / _walling_size_factor;

	// the shoot distance -- note that it's wrt to the walling size
	_shoot_distace = std::min(_walling_width, _walling_height) / 2;

	// rows of the user walling
	uint8_t **walling = new uint8_t*[uh];
	for (auto i = 0u; i < uh; i++) {
		std::string row = json_walling[i]->AsString();

		if (row.size() != uw)
			throw "Size of row '" + std::to_string(i)
					+ "' is not equal to the walling width '"
					+ std::to_string(uw) + "'";

		const char *buffer = json_walling[i]->AsString().c_str();

		// create and initialize the row
		walling[i] = new uint8_t[uw];
		for (auto j = 0u; j < uw; j++) {
			char tile = buffer[j];
			if (tile < 48 | tile > 57) {
				throw "Invalid value for tile (" + std::to_string(i) + ","
						+ std::to_string(j) + ") :'" + tile
						+ "' (must be a digit character)";
			}
			walling[i][j] = tile - '0';
		}
	}

	// we keep the user walling in the map, just in case it is useful
	// for something later.
	_map.user_walling = walling;
	_map.user_walling_width = uw;
	_map.user_walling_height = uh;

#ifdef _DEBUG
	std::cout << "Loaded the following user walling (" << uw << "x" << uh << ")"
			<< std::endl;
	std::cout << std::endl;
	for (auto i = 0u; i < uh; i++) {
		for (auto j = 0u; j < uw; j++) {
			std::cout << (int) walling[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif

	// ** construct the walling

	// We fill it all with red tile first, because the walling_height/walling_width might
	// be not divisible by user_walling_height and map_.user_walling_width.
	//
	// COMMENT: This is not needed anymore, because of the check  (sf % _walling_size_factor != 0)
	//          that we have above. I leave it anyway for now.
	//
	_map.walling = new uint8_t*[_walling_height];
	_map.walling_height = _walling_height;
	_map.walling_width = _walling_width;
	for (auto i = 0u; i < _walling_height; i++) {
		_map.walling[i] = new uint8_t[_walling_width];
		for (auto j = 0u; j < _walling_width; j++)
			_map.walling[i][j] = 1;
	}

	// each tile in the user's walling will be mapped to a block of tiles (cell) of the same color,
	// with the following width and height
	uint16_t cell_height = _walling_height / _map.user_walling_height;
	uint16_t cell_width = _walling_width / _map.user_walling_width;

	// fill in the walling
	for (int i = 0; i < _map.user_walling_height; i++)
		for (int j = 0; j < _map.user_walling_width; j++)
			for (int k = 0; k < cell_height; k++)// tile (i,j) in the user's walling is mapped to a block of tiles
				for (int l = 0; l < cell_width; l++) {
					_map.walling[i * cell_height + k][j * cell_width + l] =
							_map.user_walling[i][j];
				}

}

bool LittleWolf::addPlayer(std::uint8_t id, std::string& name) {
	assert(id < _max_player);

	if (_players[id].state != NOT_USED)
		return false;

	auto &rand = sdlutils().rand();

	_players[id].state = ALIVE;

	// The search for an empty cell start at a random position (orow,ocol)
	uint16_t orow = rand.nextInt(0, _map.walling_height);
	uint16_t ocol = rand.nextInt(0, _map.walling_width);

	// search for an empty cell
	uint16_t row = orow;
	uint16_t col = (ocol + 1) % _map.walling_width;
	while (!((orow == row) && (ocol == col)) && _map.walling[row][col] != 0) {
		col = (col + 1) % _map.walling_width;
		if (col == 0)
			row = (row + 1) % _map.walling_height;
	}

	// handle the case where the search is failed, which in principle should never
	// happen unless we start with map with few empty cells
	if (row >= _map.walling_height)
		return false;

	// initialize the player
	Player p = { //
			id, //
			name,
					viewport(0.8f), // focal
					{ col + 0.5f, row + 0.5f }, // Where.
					{ 0.0f, 0.0f }, 			// Velocity.
					2.0f, 			// Speed.
					0.9f, 			// Acceleration.
					0.0f, 			// Rotation angle in radians.
					ALIVE 			// Player state
			};
	// not that player <id> is stored in the map as player_to_tile(id) -- which is id+10
	_map.walling[(int) p.where.y][(int) p.where.x] = player_to_tile(id);
	_players[id] = p;
	_players[id].state = ALIVE;
	_curr_player_id = id;
	send_my_info();

	return true;
}

void LittleWolf::playSound(Point where, Uint8 sound)
{
	float volume = mag(sub(where, _players[_curr_player_id].where));

	if (sound == 0) {
		auto playSound = &sdlutils().soundEffects().at("gunshot");
		playSound->setVolume(volume);
		playSound->play();
	}
	else {
		auto playSound = &sdlutils().soundEffects().at("pain");
		playSound->setVolume(volume);
		playSound->play();
	}
}

void LittleWolf::send_my_info()
{
	Player& p = _players[_curr_player_id];

	Game::Instance()->get_networking().send_my_info(p.where.x,p.where.y,p.velocity.x,p.velocity.y,p.speed,p.acceleration,p.theta,p.state, p.name);
}

void LittleWolf::killPlayer(std::uint8_t id)
{
	_players[id].state = DEAD;
}

void LittleWolf::removePlayer(std::uint16_t id)
{
	_players[id].state = NOT_USED;
	_map.walling[(int)_players[id].where.y][(int)_players[id].where.x] = 0;
	send_my_info();
}

void LittleWolf::update_player_info(std::uint8_t id, float whereX, float whereY, float velocityX, float velocityY, float speed, float acceleration, float theta, std::uint8_t state, const char name[11])
{
	
	Player& p = _players[id];

	p.id = id;
	p.where.x = whereX;
	p.where.y = whereY;
	p.velocity.x = velocityX;
	p.velocity.y = velocityY;
	p.speed = speed;
	p.acceleration = acceleration;
	p.theta = theta;
	p.state = static_cast<PlayerState>(state);
	char aux[11];
	for (int i = 0; i < 11; ++i) {
		aux[i] = name[i];
	}
	Game::Instance()->chars_to_string(p.name, aux);
	_map.walling[(int)p.where.y][(int)p.where.x] = 0;

}

void LittleWolf::update_player_state(std::uint8_t id, float whereX, float whereY, float velocityX, float velocityY, float speed, float acceleration, float theta)
{
	Player& p = _players[id];
	_map.walling[(int)p.where.y][(int)p.where.x] = 0;

	p.id = id;
	p.where.x = whereX;
	p.where.y = whereY;
	p.velocity.x = velocityX;
	p.velocity.y = velocityY;
	p.speed = speed;
	p.acceleration = acceleration;
	p.theta = theta;

	_map.walling[(int)p.where.y][(int)p.where.x] = player_to_tile(id);
}

void LittleWolf::render() {

	// if the player is dead or we press M we only render upper view , otherwise the normal view
	if (_players[_curr_player_id].state == DEAD || seeMap)
		render_upper_view();
	else
		render_map(_players[_curr_player_id]);

	// render the identifiers, state, etc
	render_players_info();

	// show help
	if (_show_help) {
		int y = sdlutils().height();
		for (const char *s : { "usage_1", "usage_2", "usage_3", "usage_4",
				"usage_5" }) {
			auto &t = sdlutils().msgs().at(s);
			y = y - t.height() - 10;
			t.render(0, y);
		}
	}
}

LittleWolf::Hit LittleWolf::cast(const Point where, Point direction,
		uint8_t **walling, bool ignore_players, bool ignore_deads) {
	// Determine whether to step horizontally or vertically on the grid.
	Point hor = sh(where, direction);
	Point ver = sv(where, direction);
	Point ray = mag(sub(hor, where)) < mag(sub(ver, where)) ? hor : ver;
	// Due to floating point error, the step may not make it to the next grid square.
	// Three directions (dy, dx, dc) of a tiny step will be added to the ray
	// depending on if the ray hit a horizontal wall, a vertical wall, or the corner
	// of two walls, respectively.
	Point dc = mul(direction, 0.01f);
	Point dx = { dc.x, 0.0f };
	Point dy = { 0.0f, dc.y };
	Point test = add(ray,
	// Tiny step for corner of two grid squares.
			mag(sub(hor, ver)) < 1e-3f ? dc :
			// Tiny step for vertical grid square.
			dec(ray.x) == 0.0f ? dx :
			// Tiny step for a horizontal grid square.
					dy);
	const Hit hit = { tile(test, walling), ray };
	// If a wall was not hit, then continue advancing the ray.

	if (hit.tile > 0 && hit.tile < 10) {
		return hit;
	} else if (hit.tile > 9 && !ignore_players
			&& (!ignore_deads || _players[hit.tile - 10].state != DEAD)) {
		return hit;
	} else {
		return cast(ray, direction, walling, ignore_players, ignore_deads);
	}
}

LittleWolf::Wall LittleWolf::project(const int xres, const int yres,
		const float focal, const Point corrected) {
	// Normal distance of corrected ray is clamped to some small value else wall size will shoot to infinity.
	const float normal = corrected.x < 1e-2f ? 1e-2f : 0.05 * corrected.x;
	const float size = 0.5f * focal * xres / normal;
	const int top = (yres + size) / 2.0f;
	const int bot = (yres - size) / 2.0f;
	// Top and bottom values are clamped to screen size else renderer will waste cycles
	// (or segfault) when rasterizing pixels off screen.
	const Wall wall = { top > yres ? yres : top, bot < 0 ? 0 : bot, size };
	return wall;
}

void LittleWolf::render_map(Player &p) {
	// lock the texture
	const Display display = lock(_gpu);

	const Line camera = rotate(p.fov, p.theta);

	// Ray cast for all columns of the window.
	for (int x = 0; x < _gpu.xres; x++) {

		// draw walls
		const Point direction = lerp(camera, x / (float) _gpu.xres);
		const Hit hit = cast(p.where, direction, _map.walling, true, false);
		const Point ray = sub(hit.where, p.where);
		const Point corrected = turn(ray, -p.theta);
		const Wall wall = project(_gpu.xres, _gpu.yres, p.fov.a.x, corrected);

		for (int y = 0; y < wall.bot; y++)
			put(display, x, y, 0x00000000);

		for (int y = wall.bot; y < wall.top; y++)
			put(display, x, y, color(hit.tile));

		for (int y = wall.top; y < _gpu.yres; y++)
			put(display, x, y, 0x00000000);

		// draw players
		const Point direction_p = lerp(camera, x / (float) _gpu.xres);
		const Hit hit_p = cast(p.where, direction_p, _map.walling, false,
				false);
		const Point ray_p = sub(hit_p.where, p.where);
		const Point corrected1 = turn(ray_p, -p.theta);
		const Wall wall_p = project(_gpu.xres, _gpu.yres, p.fov.a.x,
				corrected1);

		if (hit_p.tile >= 10) {
			int size_p = 0;
			uint8_t id = hit_p.tile - 10;

			if (_players[id].state == ALIVE)
				size_p = wall_p.top - (wall_p.top - wall_p.bot) / 3;   //
			else
				size_p = wall_p.bot + (wall_p.top - wall_p.bot) / 10; // when it is dead it is shorter

			for (int y = wall_p.bot; y < size_p; y++)
				put(display, x, y, color(hit_p.tile));
		}

	}

	// draw a rifle sight at the center
	for (int i = -10; i < 10; i++) {
		put(display, _gpu.xres / 2, _gpu.yres / 2 + i, 0xAAAAAAAA);
		put(display, _gpu.xres / 2 + i, _gpu.yres / 2, 0xAAAAAAAA);
	}

	// unlock the texture
	unlock(_gpu);

	// copy the texture to the renderer
	const SDL_Rect dst = { (_gpu.xres - _gpu.yres) / 2, (_gpu.yres - _gpu.xres)
			/ 2, _gpu.yres, _gpu.xres, };
	SDL_RenderCopyEx(_gpu.renderer, _gpu.texture, NULL, &dst, -90, NULL,
			SDL_FLIP_NONE);

}

void LittleWolf::render_upper_view() {

	// lock texture
	const Display display = lock(_gpu);

	for (int x = 0; x < _gpu.xres; x++)
		for (int y = 0; y < _gpu.yres; y++)
			put(display, x, y, 0x00000000);

	for (auto x = 0u; x < _map.walling_height; x++)
		for (auto y = 0u; y < _map.walling_width; y++) {

			// each non empty position in the walling is drawn as a square in the window,
			// because the walling size is smaller than the resolution by 'walling_size_factor'
			if (_map.walling[x][y] != 0)
				for (int i = 0; i < _walling_size_factor; i++)
					for (int j = 0; j < _walling_size_factor; j++)
						put(display, y * _walling_size_factor + i,
								_gpu.yres - 1 - x * _walling_size_factor + j,
								color(_map.walling[x][y]));
		}

	// unlock texture
	unlock(_gpu);

	const SDL_Rect dst = { (_gpu.xres - _gpu.yres) / 2, (_gpu.yres - _gpu.xres)
			/ 2, _gpu.yres, _gpu.xres, };
	SDL_RenderCopyEx(_gpu.renderer, _gpu.texture, NULL, &dst, -90, NULL,
			SDL_FLIP_NONE);

	// add labels to each player, with corresponding rotation
	for (int i = 0u; i < _max_player; i++) {
		Player &p = _players[i];
		if (p.state != NOT_USED) {
			Texture info(sdlutils().renderer(), "P" + std::to_string(i),
					sdlutils().fonts().at("MFR12"),
					build_sdlcolor(color_rgba(i + 10)));

			int w = info.width();
			int h = info.height();

			SDL_Rect src = build_sdlrect(0.0f, 0.0f, w, h);
			SDL_Rect dest = build_sdlrect(
					p.where.x * _walling_size_factor - w / 2,
					p.where.y * _walling_size_factor - h / 2, w, h);

			info.render(src, dest, p.theta * _rd);

		}
	}

}

void LittleWolf::render_players_info() {

	uint_fast16_t y = 0;
	
	for (auto i = 0u; i < _max_player; i++) {
		PlayerState s = _players[i].state;

		// render player info if it is used
		if (s != NOT_USED) {
			std::cout << "player existe: " << i << std::endl;
			std::string msg = (i == _curr_player_id ? "*" : " ")
				+ std::string(_players[i].name)
				+ (s == DEAD ? " (dead)" : "");

			Texture info(sdlutils().renderer(), msg,
					sdlutils().fonts().at("MFR24"),
					build_sdlcolor(color_rgba(i + 10)));

			SDL_Rect dest = build_sdlrect(0, y, info.width(), info.height());

			info.render(dest);
			y += info.height() + 5;

		}
	}

	//SI ACTIVAMOS EL TIMER DE REINICIO
	if (beginTimerToRestart) {
		Texture infoTimer(sdlutils().renderer(), "The game will restart in: " + std::to_string((timerToRestart - sdlutils().currRealTime()) / 1000) + " seconds.",
			sdlutils().fonts().at("MFR24"),
			build_sdlcolor(color_rgba(255)));
		SDL_Rect dest = build_sdlrect((sdlutils().width() / 2) - (infoTimer.width() / 2), 0, infoTimer.width(), infoTimer.height());
		infoTimer.render(dest);
	}
}

void LittleWolf::move(Player &p) {
	auto &ihdrl = ih();

	// W forwards, S backwards, D right, L left

	const Point last = p.where, zero = { 0.0f, 0.0f };

	// Accelerates with key held down.
	if (ihdrl.isKeyDown(SDL_SCANCODE_W) || ihdrl.isKeyDown(SDL_SCANCODE_S)
			|| ihdrl.isKeyDown(SDL_SCANCODE_D)
			|| ihdrl.isKeyDown(SDL_SCANCODE_A)) {
		const Point reference = { 1.0f, 0.0f };
		const Point direction = turn(reference, p.theta);
		const Point acceleration = mul(direction, p.acceleration);
		if (ihdrl.isKeyDown(SDL_SCANCODE_W))
			p.velocity = add(p.velocity, acceleration);
		if (ihdrl.isKeyDown(SDL_SCANCODE_S))
			p.velocity = sub(p.velocity, acceleration);
		if (ihdrl.isKeyDown(SDL_SCANCODE_D))
			p.velocity = add(p.velocity, rag(acceleration));
		if (ihdrl.isKeyDown(SDL_SCANCODE_A))
			p.velocity = sub(p.velocity, rag(acceleration));
	} else { // Otherwise, decelerates (exponential decay).
		p.velocity = mul(p.velocity, 1.0f - p.acceleration / p.speed);
	}

	// Caps velocity if top speed is exceeded.
	if (mag(p.velocity) > p.speed)
		p.velocity = mul(unit(p.velocity), p.speed);

	// Moves.
	p.where = add(p.where, p.velocity);
	// Sets velocity to zero if there is a collision and puts p back in bounds.

	// if player hits a wall or a different player, we take the player back
	// to previous position and put velocity to 0
	if (tile(p.where, _map.walling) != 10 + _curr_player_id
			&& tile(p.where, _map.walling) != 0) {
		p.velocity = zero;
		p.where = last;
	} else { // otherwise we make a move
		int y0 = (int) last.y;
		int x0 = (int) last.x;
		int y1 = (int) p.where.y;
		int x1 = (int) p.where.x;
		if (x0 != x1 || y0 != y1) {
			_map.walling[y1][x1] = _map.walling[y0][x0];
			_map.walling[y0][x0] = 0;
		}
	}
}

void LittleWolf::spin(Player &p) {
	auto &ihdlr = ih();

	// L spin right, H spin left -- when left shift is held the player spins slowly

	// turn by 0.05rad, but if left shift is pressed make is 0.005rad
	float d = 0.05f;
	if (ihdlr.isKeyDown(SDL_SCANCODE_LSHIFT)) {
		d = 0.005f;
	}

	if (ihdlr.isKeyDown(SDL_SCANCODE_H))
		p.theta -= d;
	if (ihdlr.isKeyDown(SDL_SCANCODE_L))
		p.theta += d;
}

bool LittleWolf::shoot(Player &p) {
	auto &ihdlr = ih();

	int damage = 0;

	// Space shoot -- we use keyDownEvent to force a complete press/release for each bullet
	if (ihdlr.keyDownEvent() && ihdlr.isKeyDown(SDL_SCANCODE_SPACE)) {
		
		// play gun shot sound
		sdlutils().soundEffects().at("gunshot").play();
		
		SoundMsg m;
		m._type = _SOUND;
		m._client_id = _curr_player_id;
		m.soundType = 0;
		m.originX = _players[_curr_player_id].where.x;
		m.originY = _players[_curr_player_id].where.y;

		// we shoot in several directions, because with projection what you see is not exact
		for (float d = -0.05; d <= 0.05; d += 0.005) {

			// search which tile was hit
			const Line camera = rotate(p.fov, p.theta + d);
			Point direction = lerp(camera, 0.5f);
			direction.x = direction.x / mag(direction);
			direction.y = direction.y / mag(direction);
			const Hit hit = cast(p.where, direction, _map.walling, false, true);

#ifdef _DEBUG
			printf(
					"Shoot by player %d hit a tile with value %d! at distance %f\n",
					p.id, hit.tile, mag(sub(p.where, hit.where)));
#endif

			// if we hit a tile with a player id and the distance from that tile is smaller
			// than shoot_distace, we mark the player as dead
			if (hit.tile > 9 && mag(sub(p.where, hit.where)) < _shoot_distace) {
				uint8_t id = tile_to_player(hit.tile);
				_players[id].state = DEAD;
				playSound(hit.where, 1);
				
				SoundMsg m;
				m._type = _SOUND;
				m._client_id = _curr_player_id;
				m.soundType = 1;
				m.originX = _players[_curr_player_id].where.x;
				m.originY = _players[_curr_player_id].where.y;
				
				damage = 1;
				Game::Instance()->get_networking().send_shoot(_players[id].id, damage);
				return true;
			}
		}
	}
	
	return false;
}

//void LittleWolf::switchToNextPlayer() {
//
//	// search the next player in the palyer's array
//	int j = (_curr_player_id + 1) % _max_player;
//	while (j != _curr_player_id && _players[j].state == NOT_USED)
//		j = (j + 1) % _max_player;
//
//	// move to the next player view
//	_curr_player_id = j;
//
//}

void LittleWolf::bringAllToLife() {
	// bring all dead players to life -- all stay in the same position
	for (auto i = 0u; i < _max_player; i++) {
		if (_players[i].state == DEAD) {
			_players[i].state = ALIVE;
		}
	}
}
