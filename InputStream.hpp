// InputStream.hpp
// Contains the input object that will be used to tell the physics engine how to move a boat.

//#include<SDL>
//#include<AI>
//#include<NetworkPlayer>
#inclue<queue.h>

enum TurnAxisState = {Left, Neutral, Right};
enum AccelerationAxisState = {Accelerating, Neutral, Reversing};
enum FiringState = {NotFiring, Firing};

class InputState {
public:
	TurnAxisState turn;
	AccelerationAxisState acc;
	FiringState fire;

	std::string toString();
	InputState fromString(string str);
};

class InputStream {
	int frameNumber;
	InputQueue input;
	
	std::string toString();
	InputStream fromString(std::string str);
public:
	// Need deltaTime to make sure that all machines are inputting at the same rate.
	// If one machine is running slower, then it will fill its input by duplicating the most recent command to keep a constant framerate.
	virtual InputState updateInputState(float deltaTime, GameState &gs) = 0;
};

class LocalPlayerInputStream: public InputStream {
public:
	std::string hostIP;
	// Need some SDL Data?
	// Send commands to remote host if necessary
	InputState updateInputState(float deltaTime, GameState &gs);
};

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

