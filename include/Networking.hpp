#ifndef Networking_hpp
#define Networking_hpp

#include "GameState.hpp"
#include "InputStream.hpp"

struct Networking
{
	char *outputList;
	InputStream *inputStream;
	std::vector<std::string> destIPAddressList;
	std::vector<int> destPortNumList;
	int broadcastSize;
	int receivePortNum;

	Networking(std::vector<std::string> IPList, std::vector<int> portsList, int recPortNum, InputStream *inStream)
	{
		outputList = (char *) malloc((MAX_FRAMES + 8) * sizeof(char));
		destIPAddressList = IPList;
		destPortNumList = portsList;
		broadcastSize = IPList.size();
		receivePortNum = recPortNum;
		inputStream = inStream;
	}

	// For sending InputStreams and GameState Info
	void broadcastInputStream();
	void sendGameStateInfo(GameState* world);
	void receiveGameStateInfo(GameState* world);
};

void receiveInputStream(GameState *world, int receivePortNum);
void error(const char *msg);
void sendDatagram(void *msgObject, size_t objLen, std::string destIPAddress, int destPortNum);
int receiveDatagram(void *buffer, size_t bufferSize, int receivePortNum);

#endif /* Networking_hpp */
