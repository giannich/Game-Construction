#ifndef AI_1_2_hpp

#define AI_1_2_hpp

#include "AI.hpp"
#include <iostream>

class AI_1_2 : public AI
{
public:
	vec2 *L, *R;
	int N;
	float accThreshold;
	float revThreshold;
	AI_1_2(Track *t, int my_player, int num_boats, float width, float acceleratingThreshold = 0.8, float reversingThreshold = 0.5);
	InputState getCommand(std::vector<Boat>&);
};
#endif