
#pragma once
#include "../ecs/System.h"

class FoodSystem : public ecs::System {
public:

	FoodSystem();
	virtual ~FoodSystem();
	void initSystem() override;
	void update() override;
	void addFood();
	void onFoodEaten(ecs::entity_t e);
	void resetFruits();
	void resetMiracleFruits();
	void recieve(const Message& m) override;
private:
	unsigned int _currNumOfFood;
};

