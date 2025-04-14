#include <string>

class JSONValue;

class ConfigAsteroids {
public:
	ConfigAsteroids(std::string filename);
	virtual ~ConfigAsteroids();

	//Control+F pegando esto para obtener todos los sitios donde se usa el config
	//Game::Instance()->getConfigAsteroids()

	//Datos por defecto
	//	"asteroids" :
	//{
	//		"numAsteroids" : 3,
	//		"frequency" : 5000.0,
	//		"numSplitAsteroids" : 2
	//},
	//
	//	"fighter" :
	//{
	//		"speed" : 3.5,
	//		"speedRotation" : 3.0,
	//		"gunReloadTime" : 50.0
	//}

	//ASTEROIDS - NEWROUNDSTATE
	int numAsteroids();

	//ASTEROIDS - RUNNINGSTATE
	float frequencyAsteroids();

	//ASTEROIDS - ASTEROIDSUTILS
	int numSplitAsteroids();

	//FIGTHER - FIGTHERCTRL
	float speedFigther();
	float speedRotationFigther();

	//FIGTHER - FIGTHERGUN
	float gunReloadTime();
	
private:

	//void configurePaddle(Container* paddle, std::string id, GameObject* ball);
	JSONValue* _jRoot;

};