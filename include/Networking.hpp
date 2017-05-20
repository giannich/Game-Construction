#ifndef Networking_hpp
#define Networking_hpp

#include "GameState.hpp"
#include "InputStream.hpp"

struct Networking
{
	char *outputList;
	InputStream *inputStream;

	Networking()
	{
		outputList = (char *) malloc((MAX_FRAMES + 4) * sizeof(char));
		inputStream = new InputStream();
	}

	void sendPlayerInfo(GameState* world);
	void receivePlayerInfo(GameState* world);
	void sendGameStateInfo(GameState* world);
	void receiveGameStateInfo(GameState* world);
};

void error(const char *msg);
void sendDatagram(void *msgObject, size_t objLen, std::string destIPAddress, int destPortNum);
int receiveDatagram(void *buffer, size_t bufferSize, int receivePortNum);

#endif /* Networking_hpp */
