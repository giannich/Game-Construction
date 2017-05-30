#ifndef Networking_hpp
#define Networking_hpp

#include "GameState.hpp"
#include <queue>
#define MAX_FRAMES 50

class InputStream;

struct Networking
{
	char *outputList;
	InputStream *inputStream;
	std::vector <std::pair<std::string, int>> *broadcastTargets;
	int broadcastSize;

	// IP List -> Vector of destination IP addresses
	// portsList -> Vector of destination port numbers
	// inStream -> Inputstream pointer
	Networking(std::vector <std::pair<std::string, int>> *broadcastList, InputStream *inStream)
	{
		outputList = (char *) malloc((MAX_FRAMES + 8) * sizeof(char));
		broadcastTargets = broadcastList;
		broadcastSize = broadcastTargets->size();
		inputStream = inStream;
	}

	// For sending InputStreams
	void broadcastInputStream();
};

struct BoatPatch
{
	// Linear Velocity
	float32 _velx;
	float32 _vely;

	// Rotational Velocity
	float32 _rotvel;

	// Orientation & Position
	float32 _orient;
	float32 _posx;
	float32 _posy;

	// Current Souls
	int _souls;

	// Constructor
	BoatPatch(float32 velx, float32 vely, float32 rotvel, float32 orient, float32 posx, float32 posy, int souls);
};

struct GameStatePatch
{
	int playerNum;
	std::vector<BoatPatch *> *boatPatches;

	// Apply the patches
	void applyPatch(GameState *world);

	// Constructor
	GameStatePatch(int pNum)
	{
		playerNum = pNum;
		boatPatches = new std::vector<BoatPatch *>();
	}
};

// Gamestate
void sendGameStateInfo(GameState *world, std::vector <std::pair<std::string, int>> gamestateBroadcastList);
void receiveGameStateInfo(GameState *world, int receivePortNum, bool isHost, std::queue<GameStatePatch *> *gsp_queue);
unsigned int gameSetup(char **argv, std::vector <std::pair<std::string, int>> *broadcastList, std::vector <std::pair<std::string, int>> *gamestateBroadcastList, std::vector<int> *playerTypeList);
void receiveInputStream(GameState *world, int receivePortNum, std::vector<int> *playerDiscardList);
void error(const char *msg);
void sendDatagram(void *msgObject, size_t objLen, std::string destIPAddress, int destPortNum);
int receiveDatagram(void *buffer, size_t bufferSize, int receivePortNum);

#endif /* Networking_hpp */
