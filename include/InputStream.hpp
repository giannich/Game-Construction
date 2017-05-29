// InputStream.hpp
// Contains the input object that will be used to tell the physics engine how to move a boat.

#ifndef InputStream_hpp
#define InputStream_hpp

#include<SDL.h>
//#include<AI>
//#include<NetworkPlayer>
#include<queue>
#include "GameState.hpp"

typedef enum TurnAxis {Left, Neutral, Right} TurnAxisState;
typedef enum AccelerationAxis {Accelerating, Idling, Reversing} AccelerationAxisState;
typedef enum Firing {NotFiring, Firing} FiringState;

class GameState;
class AI;

class InputState {
public:
	TurnAxisState turn;
	AccelerationAxisState acc;
	FiringState fire;
    InputState() {turn = Neutral; acc = Idling; fire = NotFiring;}
	InputState(TurnAxisState t, AccelerationAxisState a, FiringState f) : turn(t), acc(a), fire(f) { }

	std::string toString();
    InputState fromString(std::string str);
};

class InputStream {
	int frameNumber;
    //std::queue<InputState> inputStream;
	
	//std::string toString();
	//InputStream fromString(std::string str);
public:
	InputState lastInputState;
	int playerNum;
	// Need deltaTime to make sure that all machines are inputting at the same rate.
	// If one machine is running slower, then it will fill its input by duplicating the most recent command to keep a constant framerate.
	virtual void update(float deltaTime, GameState &gs) = 0;
};

class LocalPlayerInputStream: public InputStream {
public:
	//std::string hostIP;
	// Need some SDL Data?
	// Send commands to remote host if necessary
	//InputState updateInputState(float deltaTime, GameState &gs);
	LocalPlayerInputStream(int pNum) { playerNum = pNum; }
	void update(float deltaTime, GameState &gs);
};

class AIInputStream: public InputStream {
public:
	AI *ai;
	AIInputStream(int pNum, AI *ai1) : ai(ai1) { playerNum = pNum;}
	// This will have access to the input state from the last frame, and the gamestate for this frame.
	// After this function is called, the currentState for the AIInputStream should be updated to what we want for next frame.
	void update(float deltaTime, GameState &gs);
};

/*
class NetworkPlayerInputStream: public InputStream {
	NetworkPlayer *socketInfo;
public:
	InputState updateInputState(float deltaTime, GameState &gs);
};
*/

#endif /*InputStream_hpp*/
