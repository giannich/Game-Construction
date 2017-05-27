#pragma once

#include "Testbed\include\Track.hpp"
#include "Testbed\include\InputStream.hpp"

class Boat;

class AI
{
public:
	Track *track;
	int myPlayer;
	int numBoats;
	virtual InputState getCommand(Boat **boats) = 0;
};