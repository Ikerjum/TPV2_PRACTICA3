#include "GameState.h"

class Texture;
class FigtherUtils;

class NewGameState : public GameState {
public:
	NewGameState();
	~NewGameState();
	void enter() override;
	void leave() override;
	void update() override;
private:
	Texture& _msg;
};