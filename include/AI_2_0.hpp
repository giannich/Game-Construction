#ifndef AI_2_0_hpp

#define AI_2_0_hpp

#include "AI.hpp"
#include <iostream>
#include <fstream>

class AI_2_0 : public AI
{
public:
	int size[4];//exclude bias
	float **weights[3];
	float *layers[4];
	std::ofstream out;
	int isOutEnabled;
	float randomness;
	AI_2_0(Track *t, int my_player, int num_boats, char* inFile, char* outFile, float randomness);
	InputState getCommand(std::vector<Boat>&,std::vector<Soul>&);
	float getNNout();
	~AI_2_0();
};
#endif