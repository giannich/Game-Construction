// InputStream.hpp
// Contains the input object that will be used to tell the physics engine how to move a boat.

//#include<SDL>
//#include<AI>
//#include<NetworkPlayer>

enum TurnAxisState = {Left, Neutral, Right};
enum AccelerationAxisState = {Accelerating, Neutral, Braking, Reversing};
enum FiringState = {NotFiring, Firing};

class InputState {
public:
	TurnAxisState turn;
	AccelerationAxisState acc;
	FiringState fire;

	InputState fromString(string str);
	std::string toString();
};

class InputStream {
public:
	virtual InputState updateInputState(GameState &gs) = 0;
};

class PlayerInputStream: public InputStream {
public:
	// Need some SDL Data?
	InputState updateInputState(GameState &gs);
};

class AIInputStream: public InputStream {
	InputState currentState;
public:
	// This will have access to the input state from the last frame, and the gamestate for this frame.
	// After this function is called, the currentState for the AIInputStream should be updated to what we want for next frame.
	InputState updateInputState(GameState &gs);
};

class NetworkPlayerInputStream: public InputStream {
	NetworkPlayer *socketInfo;
public:
	InputState updateInputState(GameState &gs);
};

