#ifndef AI_1_0_hpp

#define AI_1_0_hpp

#include "AI.hpp"
#include <iostream>

class AI_1_0 : public AI
{
public:
	int segAhead;
	float accThreshold;
	float revThreshold;
	float turnThreshold;
	AI_1_0(Track *t, int my_player, int num_boats, int s = 5, float acceleratingThreshold = 0.8, float reversingThreshold = 0.5, float turningThreshold = .99);
	InputState getCommand(std::vector<Boat>&);
};
#endif