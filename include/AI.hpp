#pragma once

#include "Track.hpp"
#include "InputStream.hpp"

class Boat;

class AI
{
public:
	Track *track;
	int myPlayer;
	int numBoats;
	virtual InputState getCommand(std::vector<Boat>&, std::vector<Soul>&) = 0;
};