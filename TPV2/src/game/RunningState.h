#include "GameState.h"

class RunningState : public GameState {
public:
	RunningState();
	~RunningState();
	void enter() override;
	void leave() override;
	void update() override;
};