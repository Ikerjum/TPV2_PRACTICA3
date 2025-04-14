#include "ConfigAsteroids.h"

#include "../json/JSON.h"
#include "../json/JSONValue.h"

#include <assert.h>

ConfigAsteroids::ConfigAsteroids(std::string filename)
{
	_jRoot = JSON::ParseFromFile(filename);

	if (_jRoot == nullptr || !_jRoot->IsObject()) {
		throw "Something went wrong while load/parsing '" + filename + "'";
	}
}

ConfigAsteroids::~ConfigAsteroids()
{
	delete _jRoot;
}

int ConfigAsteroids::numAsteroids()
{
	auto jRoot = _jRoot->AsObject();
	assert(jRoot["asteroids"] != nullptr && jRoot["asteroids"]->IsObject());
	auto jRootAsteroids = jRoot["asteroids"]->AsObject();
	return jRootAsteroids["numAsteroids"]->AsNumber();
}

float ConfigAsteroids::frequencyAsteroids()
{
	auto jRoot = _jRoot->AsObject();
	assert(jRoot["asteroids"] != nullptr && jRoot["asteroids"]->IsObject());
	auto jRootAsteroids = jRoot["asteroids"]->AsObject();
	return jRootAsteroids["frequency"]->AsNumber();
}

int ConfigAsteroids::numSplitAsteroids()
{
	auto jRoot = _jRoot->AsObject();
	assert(jRoot["asteroids"] != nullptr && jRoot["asteroids"]->IsObject());
	auto jRootAsteroids = jRoot["asteroids"]->AsObject();
	return jRootAsteroids["numSplitAsteroids"]->AsNumber();
}

float ConfigAsteroids::speedFigther()
{
	auto jRoot = _jRoot->AsObject();
	assert(jRoot["fighter"] != nullptr && jRoot["fighter"]->IsObject());
	auto jRootFighter = jRoot["fighter"]->AsObject();
	return jRootFighter["speed"]->AsNumber();
}

float ConfigAsteroids::speedRotationFigther()
{
	auto jRoot = _jRoot->AsObject();
	assert(jRoot["fighter"] != nullptr && jRoot["fighter"]->IsObject());
	auto jRootFighter = jRoot["fighter"]->AsObject();
	return jRootFighter["speedRotation"]->AsNumber();
}

float ConfigAsteroids::gunReloadTime()
{
	auto jRoot = _jRoot->AsObject();
	assert(jRoot["fighter"] != nullptr && jRoot["fighter"]->IsObject());
	auto jRootFighter = jRoot["fighter"]->AsObject();
	return jRootFighter["gunReloadTime"]->AsNumber();
}
