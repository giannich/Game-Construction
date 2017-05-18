#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "Networking.hpp"

void Networking::sendPlayerInfo(GameState* world)
{
	for( auto it = world->boats->begin(); it != world->boats->end(); ++it) 
	{
		inputStream->writeSingleState(*(it->inputState));
		inputStream->readAllInputStates(outputList);
		std::cout << it->inputState->toString() << "\n";
		sendDatagram(outputList, MAX_FRAMES + 4, "localhost", 12345);
	}
}
void Networking::receivePlayerInfo(GameState* world)
{
	receiveDatagram(outputList, MAX_FRAMES + 4, 12345);
	inputStream->writeAllInputStates(outputList);

	for( auto it = world->boats->begin(); it != world->boats->end(); ++it) 
		inputStream->readSingleState(inputStream->getCurrentFrameNumber() - 1, *(it->inputState));
}

// Error Generation
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

// Function for sending a datagram
void sendDatagram(void *msgObject, size_t objLen, std::string destIPAddress, int destPortNum)
{
	// Initialize some values
	int socketDescriptor, msgLen;
	struct sockaddr_in serverAddress;
	struct hostent *server;

	// Creates socket file descriptor for socket communication
	socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketDescriptor < 0)
		error("ERROR opening socket");

	// Converts the string into a char*
	char *destAddressPtr = new char[destIPAddress.length() + 1];
	std::strcpy(destAddressPtr, destIPAddress.c_str());

	// Gets the servername
	server = gethostbyname(destAddressPtr);
	if (server == NULL) 
		error("ERROR no such host");

	// No idea whats going on here, but I think this is mostly for setting up the server address? Need to check again...
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);
	serverAddress.sin_port = htons(destPortNum);

	// Send a message
	msgLen = sendto(socketDescriptor, msgObject, objLen, 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (msgLen < 0)
		error("Error failed sendto");

	// Finally the close descriptor
	close(socketDescriptor);
	return;
}

// Function for accepting a datagram
void receiveDatagram(void *buffer, size_t bufferSize, int receivePortNum)
{
	// Initialize some values
	int msgLength, reuseTrue;
	socklen_t senderLength;
	struct sockaddr_in receiverAddress, senderAddress;

	// Creates socket file descriptor for socket communication
	int socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketDescriptor < 0)
		error("ERROR opening socket");

	// This prevents the socket from hogging space in case it is not closed prematurely
	reuseTrue = 1;
	if (setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &reuseTrue, sizeof(int)) == -1) 
		error("ERROR socket options");

	// Sets up the server address stuff and actually binds to a socket
	receiverAddress.sin_family = AF_INET;
	receiverAddress.sin_addr.s_addr = INADDR_ANY;
	receiverAddress.sin_port = htons(receivePortNum);
	if (bind(socketDescriptor, (struct sockaddr *) &receiverAddress, sizeof(receiverAddress)) < 0)
		error("ERROR on binding");
		 
	// Listens to socket with the socketDescriptor and can accept up to 5 connections in queue
	listen(socketDescriptor, 5);
	senderLength = sizeof(senderAddress);

	// Receives the actual message
	msgLength = recvfrom(socketDescriptor, buffer, bufferSize, 0, (struct sockaddr *)&senderAddress, &senderLength);

	if (msgLength < 0)
		error("ERROR on receiving");

	// Close descriptor and return the message
	close(socketDescriptor);
}