#ifndef Networking_hpp
#define Networking_hpp

#include "GameState.hpp"
#include "Soul.hpp"
#include <netinet/in.h>
#include <queue>
#define MAX_FRAMES 50
#define REGISTRATION_PORT 12344
#define SERVER_PORT 12345
#define CLIENT_PORT 12346
#define GAMESTATE_PORT 12347
#define ACK_SERVER_PORT 12350
#define ACK_CLIENT_PORT 12351


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

struct SoulPatch
{
	// Collected
	bool _collected;

	// Orientation & Position
	float32 _sorient;
	float32 _sposx;
	float32 _sposy;

	// Constructor
	SoulPatch(bool collected, float32 sorient, float32 sposx, float32 sposy);
};

struct GameStatePatch
{
	int playerNum;
	int soulNum;
	int frame;
	std::vector<BoatPatch *> *boatPatches;
	std::vector<SoulPatch *> *soulPatches;

	// Apply the patches
	void applyPatch(GameState *world);

	// Constructor
	GameStatePatch(int pNum, int sNum, int fNum)
	{
		playerNum = pNum;
		soulNum = sNum;
		frame = fNum;
		boatPatches = new std::vector<BoatPatch *>();
		soulPatches = new std::vector<SoulPatch *>();
	}
};

// Gamestate
void error(const char *msg);
void sendGameStateInfo(GameState *world, std::vector <std::pair<in_addr, int>> gamestateBroadcastList);
void receiveGameStateInfo(GameState *world, bool isHost, std::queue<GameStatePatch *> *gsp_queue);
unsigned int gameSetup(int argc, char **argv, std::vector <std::pair<in_addr, int>> *broadcastList, std::vector <std::pair<in_addr, int>> *gamestateBroadcastList, std::vector<int> *playerTypeList, bool *isHost);
void gamePrep(bool isHost, std::vector<int> *playerTypeList, std::vector <std::pair<in_addr, int>> *broadcastList, bool *isReady);
void receiveInputStream(GameState *world, bool isHost, std::vector<int> *playerDiscardList);
void sendDatagram(void *msgObject, size_t objLen, in_addr *serverAddressBuffer, int destPortNum);
int receiveDatagram(void *buffer, size_t bufferSize, int receivePortNum);
void sendStream(void *msgObject, size_t objLen, in_addr *serverAddressBuffer, int destPortNum);
int receiveStream(void *buffer, size_t bufferSize, int receivePortNum);
int receiveStreamAddr(void *buffer, size_t bufferSize, int receivePortNum, in_addr *serverAddressBuffer);
#endif /* Networking_hpp */
