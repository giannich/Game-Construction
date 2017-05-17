// InputStream.hpp
// Contains the input object that will be used to tell the physics engine how to move a boat.

#ifndef InputStream_hpp
#define InputStream_hpp

//#include<SDL>
//#include<AI>
//#include<NetworkPlayer>
#include<queue>

// Gianni's includes
#include <boost/circular_buffer.hpp>
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

	std::string toString();
    InputState fromString(std::string str);
    char toChar();
};

class InputStream {
public:

    // Constructor
    InputStream();

    // Reads a single InputState
	void readSingleState(int targetFrameNumber, InputState &inputAddr);

	// Writes a single InputState
	void writeSingleState(InputState newInputState);

	// Reads a bunch of InputStates
	void readAllInputStates(char *outputList);

	// Writes a bunch of InputStates
	void writeAllInputStates(char *outputList);

	// Gets the current frame number
	int getCurrentFrameNumber();

private:

    unsigned int currentFrameNumber;
    boost::circular_buffer<InputState> circular_buffer{MAX_FRAMES};

    // Jordan's Comments Below

	// Need deltaTime to make sure that all machines are inputting at the same rate.
	// If one machine is running slower, then it will fill its input by duplicating the most recent command to keep a constant framerate.
	//virtual InputState updateInputState(float deltaTime, GameState &gs) = 0;
};

class LocalPlayerInputStream: public InputStream {
public:
	//std::string hostIP;
	// Need some SDL Data?
	// Send commands to remote host if necessary
	//InputState updateInputState(float deltaTime, GameState &gs);
};
/*
class AIInputStream: public InputStream {
public:
	// This will have access to the input state from the last frame, and the gamestate for this frame.
	// After this function is called, the currentState for the AIInputStream should be updated to what we want for next frame.
	InputState updateInputState(float deltaTime, GameState &gs);
};

class NetworkPlayerInputStream: public InputStream {
	NetworkPlayer *socketInfo;
public:
	InputState updateInputState(float deltaTime, GameState &gs);
};
*/

#endif /*InputStream_hpp*/
