#include "GameState.h"

class Texture;

class PausedState : public GameState {
public:
	PausedState();
	~PausedState();
	void enter() override;
	void leave() override;
	void update() override;
private:
	Texture& _msg;
};