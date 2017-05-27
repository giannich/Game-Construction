// InputStream.hpp
// Contains the input object that will be used to tell the physics engine how to move a boat.

#ifndef InputStream_hpp
#define InputStream_hpp

#include<SDL.h>
//#include<AI>
//#include<NetworkPlayer>
#include<queue>
#include "GameState.hpp"

// Gianni's includes
#include <boost/circular_buffer.hpp>
#include <stdio.h>
#include <string>
#include "Networking.hpp"
#define MAX_FRAMES 50

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

class GameState;
class SimpleAI;

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
public:
	InputState lastInputState;
	Networking *networkingHandler;
	unsigned int playerNum;
	std::vector <std::pair<std::string, int>> *broadcastPointer;

	// TODO: Take off later
	std::vector <std::pair<std::string, int>> broadcastList;
	
	// Constructor
	// TODO: Needs to change so that it can accept an external broadcastlist pointer
	InputStream()
	{
		// Networking settings here
		std::string targetAddress = "localhost";
		int targetPort = 12346;

		// Networking setup code here
		std::pair <std::string, int> broadcastTarget = std::make_pair(targetAddress, targetPort);
		broadcastList.push_back(broadcastTarget);


		networkingHandler = new Networking(&broadcastList, this);
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
	LocalPlayerInputStream(int pNum) { playerNum = pNum; }
	void update(float deltaTime, GameState &gs);
};

// AI InputStream
class AIInputStream: public InputStream {
public:
	SimpleAI *ai;
	AIInputStream(int pNum, SimpleAI *ai1) : ai(ai1), InputStream() { playerNum = pNum;}
	// This will have access to the input state from the last frame, and the gamestate for this frame.
	// After this function is called, the currentState for the AIInputStream should be updated to what we want for next frame.
	void update(float deltaTime, GameState &gs);
};

// GIANNI'S CHANGE
// Network Player InputStream
class NetworkPlayerInputStream: public InputStream {
public:
	NetworkPlayerInputStream(int pNum) : InputStream() { playerNum = pNum; }
	void update(float deltaTime, GameState &gs);
};

#endif /*InputStream_hpp*/
