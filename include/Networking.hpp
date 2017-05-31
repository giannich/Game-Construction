#ifndef Networking_hpp
#define Networking_hpp

#include "GameState.hpp"
#include <netinet/in.h>
#include <queue>
#define MAX_FRAMES 50
#define REGISTRATION_PORT 12344
#define SERVER_PORT 12345
#define CLIENT_PORT 12346
#define GAMESTATE_PORT 12347

class InputStream;

struct Networking
{
	char *outputList;
	InputStream *inputStream;
	std::vector <std::pair<in_addr, int>> *broadcastTargets;
	int broadcastSize;

	// IP List -> Vector of destination IP addresses
	// portsList -> Vector of destination port numbers
	// inStream -> Inputstream pointer
	Networking(std::vector <std::pair<in_addr, int>> *broadcastList, InputStream *inStream)
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
void error(const char *msg);
void sendGameStateInfo(GameState *world, std::vector <std::pair<in_addr, int>> gamestateBroadcastList);
void receiveGameStateInfo(GameState *world, bool isHost, std::queue<GameStatePatch *> *gsp_queue);
unsigned int gameSetup(int argc, char **argv, std::vector <std::pair<in_addr, int>> *broadcastList, std::vector <std::pair<in_addr, int>> *gamestateBroadcastList, std::vector<int> *playerTypeList, bool *isHost);
void receiveInputStream(GameState *world, bool isHost, std::vector<int> *playerDiscardList);
void sendDatagram(void *msgObject, size_t objLen, in_addr *serverAddressBuffer, int destPortNum);
int receiveDatagram(void *buffer, size_t bufferSize, int receivePortNum);
//int receiveDatagramAddr(void *buffer, size_t bufferSize, int receivePortNum, in_addr *serverAddressBuffer);
void sendStream(void *msgObject, size_t objLen, in_addr *serverAddressBuffer, int destPortNum);
int receiveStream(void *buffer, size_t bufferSize, int receivePortNum);
int receiveStreamAddr(void *buffer, size_t bufferSize, int receivePortNum, in_addr *serverAddressBuffer);
#endif /* Networking_hpp */
