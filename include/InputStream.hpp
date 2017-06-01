// InputStream.hpp
// Contains the input object that will be used to tell the physics engine how to move a boat.

#ifndef InputStream_hpp
#define InputStream_hpp

#include <SDL.h>
#include <queue>
#include <boost/circular_buffer.hpp>
#include <stdio.h>
#include <string>
#include <iostream>
#include "Networking.hpp"
#include "GameState.hpp"

#define MAX_FRAMES 50
#define FRAME_LAG 5

class GameState;
class AI;

typedef enum TurnAxis 
{
	Left = 1 << 0, 
	Neutral = 1 << 1, 
	Right = 1 << 2
} TurnAxisState;

typedef enum AccelerationAxis 
{
	Accelerating = 1 << 3, 
	Idling = 1 << 4,
	Reversing = 1 << 5
} AccelerationAxisState;

typedef enum Firing 
{
	NotFiring = 1 << 6, 
	Firing = 1 << 7
} FiringState;


class InputState {
public:
	TurnAxisState turn;
	AccelerationAxisState acc;
	FiringState fire;
	InputState() {turn = Neutral; acc = Idling; fire = NotFiring;}

	InputState(char inputChar) 
	{
		turn = static_cast<TurnAxisState>(inputChar & 0b00000111);
		acc = static_cast<AccelerationAxisState>(inputChar & 0b00111000);
		fire = static_cast<FiringState>(inputChar & 0b11000000);
	}

	InputState(TurnAxisState t, AccelerationAxisState a, FiringState f) : turn(t), acc(a), fire(f) { }

	std::string toString();
	InputState fromString(std::string str);
	char toChar();
};

class InputStream {
	int frameNumber;
    //std::queue<InputState> inputStream;
	
	//std::string toString();
	//InputStream fromString(std::string str);
public:
	InputState lastInputState;
	Networking *networkingHandler;
	unsigned int playerNum;
	
	// Constructor
	// TODO: Needs to change so that it can accept an external broadcastlist pointer
	InputStream()
	{
		currentFrameNumber = 0;
	}

	// Returns a single InputState from InputStream
	InputState readSingleState(int targetFrameNumber);

	// Pushes a single InputState into the InputStream as the latest state
	void writeSingleState(InputState newInputState);

	// Reads all the InputStates from InputStram,
	// Converts them from InputState into chars,
	// And writes them into outputList
	void encodeInputStates(char *outputList);

	// Reads all the chars from outputList,
	// Converts them from chars into InputStates,
	// And finally calls writeSingleState for each new InputState
	// Only writes whichever frames we are missing
	// e.g. If current frame is 45, and outputList has frames up to 50
	// will only write the latest 5 frames
	void decodeInputStates(char *outputList);

	// Gets the current frame number
	// Used for debugging
	int getCurrentFrameNumber();
	void setCurrentFrameNumber(int targetFrameNumber);
	int getBufferSize();

	// Need deltaTime to make sure that all machines are inputting at the same rate.
	// If one machine is running slower, then it will fill its input by duplicating the most recent command to keep a constant framerate.
	virtual void update(float deltaTime, GameState &gs) = 0;
private:
	unsigned int currentFrameNumber;
	boost::circular_buffer<InputState> circular_buffer{MAX_FRAMES};
};

// Localplayer InputStream
class LocalPlayerInputStream: public InputStream {
public:
	//std::string hostIP;
	// Need some SDL Data?
	// Send commands to remote host if necessary
	//InputState updateInputState(float deltaTime, GameState &gs);
	LocalPlayerInputStream(int pNum, std::vector <std::pair<in_addr, int>> *broadcastPointer) : InputStream()
	{ 
		playerNum = pNum;
		networkingHandler = new Networking(broadcastPointer, this);
	}
	void update(float deltaTime, GameState &gs);
};

// AI InputStream
class AIInputStream: public InputStream {
public:
	AI *ai;
	AIInputStream(int pNum, AI *ai1, std::vector <std::pair<in_addr, int>> *broadcastPointer) : ai(ai1), InputStream()
	{ 
		playerNum = pNum;
		networkingHandler = new Networking(broadcastPointer, this);
	}
	// This will have access to the input state from the last frame, and the gamestate for this frame.
	// After this function is called, the currentState for the AIInputStream should be updated to what we want for next frame.
	void update(float deltaTime, GameState &gs);
};

// GIANNI'S CHANGE
// Network Player InputStream
class NetworkPlayerInputStream: public InputStream {
public:
	NetworkPlayerInputStream(int pNum, std::vector <std::pair<in_addr, int>> *broadcastPointer, bool broadcast) : InputStream()
	{ 
		playerNum = pNum; 
		networkingHandler = new Networking(broadcastPointer, this);
		isBroadcasting = broadcast;
		
		for (int i = 0; i < MAX_FRAMES; i++)
			writeSingleState(InputState());

		setCurrentFrameNumber(1);
	}
	void update(float deltaTime, GameState &gs);

private:
	bool isBroadcasting;
};

#endif /*InputStream_hpp*/
