#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Networking.hpp"
#include "Box2D/Box2D.h"
#include "Boat.hpp"

extern int h_errno;

#define MAX_JSON_CHARS 4096

/****************
* Initial Setup *
****************/

unsigned int gameSetup(int argc, char **argv, std::vector <std::pair<in_addr, int>> *broadcastList, std::vector <std::pair<in_addr, int>> *gamestateBroadcastList, std::vector<int> *playerTypeList, bool *isHost)
{
	int playerNum;
	int totalNumberOfPlayers;
	unsigned int randomSeed;

	// Client or host
	if(!strcmp(argv[1], "host") && (argc == 6))
	{
		std::cout << "I am a host\n";
		*isHost = true;
	}
	else if (!strcmp(argv[1], "client") && (argc == 3))
	{
		std::cout << "I am a client\n";
		*isHost = false;
	}
	else
	{
		std::cout << "Usage: ./gtest <host> <playerNums> <aiPlayerType1> <aiPlayerType2> <aiPlayerType3>\n";
		std::cout << "Usage: ./gtest <client> <serverIPAddress> <aiPlayerNums>\n";
		exit(1);
	}

	if(*isHost)
	{
		// Expected number of players
		int expectedPlayerNums = atoi(argv[2]);

		// Number of AI players
		int aiPlayerType1 = atoi(argv[3]);
		int aiPlayerType2 = atoi(argv[4]);
		int aiPlayerType3 = atoi(argv[5]);

		// Player number for host is always 0
		playerNum = 0;

		// NEEDS TO CREATE A LOCAL BOAT HERE
		playerTypeList->push_back(0);

		// For playerlist stuff
		int bufferSize = sizeof(int);
		int *intArrBuffer = (int *) malloc(bufferSize * 2);
		int *intBuffer = (int *) malloc(bufferSize);
		int tempPlayerPort;
		int tempGamestatePort;
		std::pair <in_addr, int> tempPlayer;

		// Loop listening for players
		for(int i = 1; i <= expectedPlayerNums; i++)
		{
			std::cout << "Listening in for connections\n";

			// Gets the player's port numbers
			in_addr *serverAddress = (in_addr *) malloc(sizeof(in_addr));
			receiveStreamAddr(intArrBuffer, bufferSize * 2, REGISTRATION_PORT, serverAddress);
			
			// Sends back the player number
			*intBuffer = i;
			sendStream(intBuffer, bufferSize, serverAddress, CLIENT_PORT);

			// Adds contact information on the broadcastList
			tempPlayer = std::make_pair(*serverAddress, CLIENT_PORT);
			broadcastList->push_back(tempPlayer);
			playerTypeList->push_back(1);
			tempPlayer = std::make_pair(*serverAddress, GAMESTATE_PORT);
			gamestateBroadcastList->push_back(tempPlayer);

			std::cout << "Successfully created player number " << std::to_string(i) << "\n";
		}

		// Loop for creating ai players
		for(int i = 0; i < aiPlayerType1; i++)
			playerTypeList->push_back(2);

		for(int i = 0; i < aiPlayerType2; i++)
			playerTypeList->push_back(3);

		for(int i = 0; i < aiPlayerType3; i++)
			playerTypeList->push_back(4);

		// Finally sends players a message indicating the total number of players and the random seed
		totalNumberOfPlayers = expectedPlayerNums + aiPlayerType1 + aiPlayerType2 + aiPlayerType3 + 1;
		randomSeed = time(NULL);
		unsigned int *unsignedBuffer = (unsigned int *) malloc(sizeof(unsigned int) * 2);
		unsignedBuffer[0] = totalNumberOfPlayers;
		unsignedBuffer[1] = randomSeed;

		// Debug info
		std::cout << "There are a total of " << std::to_string(totalNumberOfPlayers) << " players in this game\n";
		std::cout << "Host has generated a random seed of " << std::to_string(randomSeed) << "\n";

		// Sends info to all players
		for(int i = 0; i < expectedPlayerNums; i++)
			sendStream(unsignedBuffer, sizeof(unsigned int) * 2, &broadcastList->at(i).first, broadcastList->at(i).second);

		free(intBuffer);
		free(unsignedBuffer);
		free(intArrBuffer);
		std::cout << "Successfully finished setup process for host\n";
	}
	else // Client Code
	{
		// Server's ip address
		char *destIPAddress = (char *) malloc(sizeof(char) * 16);
		destIPAddress = argv[2];
		in_addr *hostAddress = (in_addr *) malloc(sizeof(in_addr));
		inet_aton(destIPAddress, hostAddress);

		// Sends the receiving port number to the server
		int bufferSize = sizeof(int);
		int *intArrBuffer = (int *) malloc(bufferSize * 2);
		intArrBuffer[0] = CLIENT_PORT;
		intArrBuffer[1] = GAMESTATE_PORT;
		sendStream(intArrBuffer, bufferSize * 2, hostAddress, REGISTRATION_PORT);
		
		// Gets the player number and assigns it to playerNum
		int *intBuffer = (int *) malloc(bufferSize);
		receiveStream(intBuffer, bufferSize, CLIENT_PORT);
		playerNum = *intBuffer;
		std::cout << "Assigned to player number " << std::to_string(playerNum) << "\n";
		//sendStream(intArrBuffer, bufferSize * 2, hostAddress, SERVER_PORT);

		// Receives the total number of players and the seed number
		unsigned int *unsignedBuffer = (unsigned int *) malloc(sizeof(unsigned int) * 2);
		receiveStream(unsignedBuffer, sizeof(unsigned int) * 2, CLIENT_PORT);
		totalNumberOfPlayers = unsignedBuffer[0];
		randomSeed = unsignedBuffer[1];
		std::cout << "There are a total of " << std::to_string(totalNumberOfPlayers) << " players in this game\n";
		std::cout << "Client has received a random seed of " << std::to_string(randomSeed) << "\n";

		// The broadcastlist will only have the server
		broadcastList->push_back(std::make_pair(*hostAddress, SERVER_PORT));

		// Creates the playerTypeList
		for (int i = 0; i < totalNumberOfPlayers; i++)
		{
			// If local
			if (i == playerNum)
				playerTypeList->push_back(0);

			// If network
			else
				playerTypeList->push_back(1);
		}

		free(intBuffer);
		free(unsignedBuffer);
		free(intArrBuffer);
		//free(destIPAddress);
		std::cout << "Successfully finished setup process for client\n";
	}

	return randomSeed;
}

/***************
* InputStreams *
***************/

// Encodes the InputState and broadcasts it to everyone in the broadcast list
// This will be called by the LocalPlayerInputStream, AIInputStream, and NetworkPlayerInputStream
void Networking::broadcastInputStream()
{
	//std::cout << "Sending a new InputStream packet to " << std::to_string(broadcastSize) << " other receivers\n";

	// Encodes the inutStream into outputList
	inputStream->encodeInputStates(outputList);

	// Broadcasts the outputlist to all the destination addresses
	for (unsigned int i = 0; i < broadcastSize; i++)
		sendDatagram(outputList, MAX_FRAMES + 12, &broadcastTargets->at(i).first, broadcastTargets->at(i).second);
}

// Receives a datagram and decodes it to the correct InputStream
// This will be called at the beginning of the game
void receiveInputStream(GameState *world, bool isHost, std::vector<int> *playerDiscardList)
{
	// Mallocs the encodedInputStream 
	char *encodedInputStream = (char *) malloc((MAX_FRAMES + 12) * sizeof(char));
	unsigned int playerNumber;
	int receivePortNum;

	if (isHost)
		receivePortNum = SERVER_PORT;
	else
		receivePortNum = CLIENT_PORT;

	// Loops and receives all datagrams
	while(1)
	{
		//std::cout << "Waiting on new packets\n";
		// Receives a datagram
		receiveDatagram(encodedInputStream, MAX_FRAMES + 12, receivePortNum);

		// Decode the Player Number
		playerNumber = ((unsigned int) (encodedInputStream[MAX_FRAMES + 4]) & 255) +
					   ((unsigned int) (encodedInputStream[MAX_FRAMES + 5] << 8) & 65280) +
					   ((unsigned int) (encodedInputStream[MAX_FRAMES + 6] << 16) & 16711680) +
					   ((unsigned int) (encodedInputStream[MAX_FRAMES + 7] << 24) & 4278190080);

		// Skips if receiving own player packets
		if (std::find(playerDiscardList->begin(), playerDiscardList->end(), playerNumber) != playerDiscardList->end())
			continue;

		//std::cout << "Received packet from player number " << std::to_string(playerNumber) << "\n";

		// Decode the InputStream in the right boat
		world->boats->at(playerNumber)->inputStream->decodeInputStates(encodedInputStream);
	}
}

/*************
* GameStates *
*************/

// Encodes a GameState and sends it through UDP
void sendGameStateInfo(GameState *world, std::vector <std::pair<in_addr, int>> gamestateBroadcastList)
{
	// Convert GameState into a ptree
	boost::property_tree::ptree pt;

	// Get the number of players
	int playerNum = world->boats->size();
	pt.put("playerNum", playerNum);

	// Get the number of souls
	int soulNum = world->souls->size();
	pt.put("soulNum", soulNum);

	// Get the latest frome number
	int latestFrameNum = world->boats->at(0)->inputStream->getCurrentFrameNumber();
	pt.put("frameNum", latestFrameNum);


	// For each player, encode the information into a ptree
	// And add the ptree to the ptree list
	for (int i = 0; i < playerNum; i++)
	{
		// It is still a pointer here, so I don't think we can dynamically do this...
		Boat playerBoat = *(world->boats->at(i));

		pt.put("linearVelocityX" + std::to_string(i), playerBoat.rigidBody->GetLinearVelocity().x);
		pt.put("linearVelocityY" + std::to_string(i), playerBoat.rigidBody->GetLinearVelocity().y);
		pt.put("rotVelocity" + std::to_string(i), playerBoat.rigidBody->GetAngularVelocity());
		pt.put("orientation" + std::to_string(i), playerBoat.rigidBody->GetAngle());
		pt.put("positionX" + std::to_string(i), playerBoat.rigidBody->GetPosition().x);
		pt.put("positionY" + std::to_string(i), playerBoat.rigidBody->GetPosition().y);
		pt.put("currentSouls" + std::to_string(i), playerBoat.currentSouls);
	}

	// For each soul, encode the information into a ptree
	// And add the ptree to the ptree list
	for (int i = 0; i < soulNum; i++)
	{
		pt.put("collected" + std::to_string(i), world->souls->at(i)->collected);
		pt.put("sPositionX" + std::to_string(i), world->souls->at(i)->rigidBody->GetPosition().x);
		pt.put("sPositionY" + std::to_string(i), world->souls->at(i)->rigidBody->GetPosition().y);
		pt.put("sOrientation" + std::to_string(i), world->souls->at(i)->rigidBody->GetAngle());
	}

	// Convert ptree into a stringstream
	std::stringstream ss;
	write_json(ss, pt);

	// Debugging info
	//std::cout << ss.str();

	// Convert stringstream into a string
	std::string aString = "";
	aString = ss.str();

	// Convert string into a char *
	char stringBuffer[aString.length()];
	std::size_t bufferSize = aString.copy(stringBuffer, aString.length(), 0);
	stringBuffer[aString.length()] = '\0';

	// Finally send the datagram
	for (int i = 0; i < gamestateBroadcastList.size(); i++)
	{
		std::cout << "Broadcasting GameState to port number " << std::to_string(gamestateBroadcastList.at(i).second) << "!\n";
		sendDatagram(&stringBuffer, bufferSize, &gamestateBroadcastList.at(i).first, gamestateBroadcastList.at(i).second);
	}
}

// Receives a message from UDP and decodes it into a GameState
void receiveGameStateInfo(GameState *world, bool isHost, std::queue<GameStatePatch *> *gsp_queue)
{
	// Receive a char *
	char stringBuffer[MAX_JSON_CHARS];
	int msgLen;

	while (!isHost)
	{
		msgLen = receiveDatagram(stringBuffer, MAX_JSON_CHARS, GAMESTATE_PORT);

		// Convert char * to string
		std::string midString(stringBuffer);

		// Convert string to a stringstream
		std::stringstream ss;
		ss << midString.substr(0, msgLen);

		// Debugging info
		//std::cout << ss.str();

		// Convert stringstream to a ptree
		boost::property_tree::ptree pt;
		read_json(ss, pt);

		// Convert ptree to a GameState, careful, this is a long process

		// Gets the player number, and if it fails, will get 0
		int playerNum = pt.get<int>("playerNum", 0.0f);
		int soulNum = pt.get<int>("soulNum", 0.0f);
		int frameNum = pt.get<int>("frameNum", 0.0f);
		GameStatePatch *aPatch = new GameStatePatch(playerNum, soulNum, frameNum);

		// Iterates through the boat list
		for (int i = 0; i < playerNum; i++)
		{
			// Linear Velocity
			float32 velx = pt.get<float32>("linearVelocityX" + std::to_string(i));
			float32 vely = pt.get<float32>("linearVelocityY" + std::to_string(i));

			// Rotational Velocity
			float32 rotvel = pt.get<float32>("rotVelocity" + std::to_string(i));

			// Orientation & Position
			float32 orient = pt.get<float32>("orientation" + std::to_string(i));
			float32 posx = pt.get<float32>("positionX" + std::to_string(i));
			float32 posy = pt.get<float32>("positionY" + std::to_string(i));

			// Current Souls
			int souls = pt.get<int>("currentSouls" + std::to_string(i));
			
			// Creates and Pushes back the boatpatch
			BoatPatch *aBoatPatch = new BoatPatch(velx, vely, rotvel, orient, posx, posy, souls);
			aPatch->boatPatches->push_back(aBoatPatch);
		}

		for (int i = 0; i < soulNum; i++)
		{
			// Collected Status
			bool collected = pt.get<bool>("collected" + std::to_string(i));
			
			// Orientation and Position
			float32 sorient = pt.get<float32>("sOrientation" + std::to_string(i));
			float32 sposx = pt.get<float32>("sPositionX" + std::to_string(i));
			float32 sposy = pt.get<float32>("sPositionY" + std::to_string(i));

			// Creates and Pushes back the soulpatch
			SoulPatch *aSoulPatch = new SoulPatch(collected, sorient, sposx, sposy);
			aPatch->soulPatches->push_back(aSoulPatch);
		}

		// Finally push the gamestatepatch into the queue
		gsp_queue->push(aPatch);
	}
}

// Constructor
BoatPatch::BoatPatch(float32 velx, float32 vely, float32 rotvel, float32 orient, float32 posx, float32 posy, int souls)
{
	_velx = velx;
	_vely = vely;
	_rotvel = rotvel;
	_orient = orient;
	_posx = posx;
	_posy = posy;
	_souls = souls;
}

// Constructor
SoulPatch::SoulPatch(bool collected, float32 sorient, float32 sposx, float32 sposy)
{
	_collected = collected;
	_sorient = sorient;
	_sposx = sposx;
	_sposy = sposy;
}

// Applies the patches to original gamestate
void GameStatePatch::applyPatch(GameState *world)
{
	for (int i = 0; i < playerNum; i++)
	{
		// Linear Velocity
		world->boats->at(i)->rigidBody->SetLinearVelocity(b2Vec2(boatPatches->at(i)->_velx, boatPatches->at(i)->_vely));

		// Rotational Velocity
		world->boats->at(i)->rigidBody->SetAngularVelocity(boatPatches->at(i)->_rotvel);

		// Orientation & Position
		world->boats->at(i)->rigidBody->SetTransform(b2Vec2(boatPatches->at(i)->_posx, boatPatches->at(i)->_posy), boatPatches->at(i)->_orient);

		// Current Souls
		world->boats->at(i)->currentSouls = boatPatches->at(i)->_souls;
	}

	for (int i = 0; i < soulNum; i++)
	{
		// Collected
		world->souls->at(i)->collected = soulPatches->at(i)->_collected;

		// Orientation & Position
		world->souls->at(i)->rigidBody->SetTransform(b2Vec2(soulPatches->at(i)->_sposx, soulPatches->at(i)->_sposy), soulPatches->at(i)->_sorient);
	}
}

/*****************
* UDP Networking *
*****************/

// Error Generation
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

// Function for sending a datagram
void sendDatagram(void *msgObject, size_t objLen, in_addr *serverAddressBuffer, int destPortNum)
{
	// Initialize some values
	int socketDescriptor, msgLen;
	struct sockaddr_in serverAddress;
	struct hostent *server;

	// Creates socket file descriptor for socket communication
	socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketDescriptor < 0)
		error("ERROR opening socket");

	// Sets up the server address
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr = *serverAddressBuffer;
	serverAddress.sin_port = htons(destPortNum);

	// Send a message
	msgLen = sendto(socketDescriptor, msgObject, objLen, 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (msgLen < 0)
		error("Error failed sendto");

	//std::cout << "Sent message of length " << std::to_string(msgLen) << "\n";

	// Finally the close descriptor
	close(socketDescriptor);
	return;
}

// Function for accepting a datagram
int receiveDatagram(void *buffer, size_t bufferSize, int receivePortNum)
{
	// Initialize some values
	int msgLen, reuseTrue;
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
	msgLen = recvfrom(socketDescriptor, buffer, bufferSize, 0, (struct sockaddr *)&senderAddress, &senderLength);

	if (msgLen < 0)
		error("ERROR on receiving");

	//std::cout << "Received message of length " << std::to_string(msgLen) << "\n";

	// Close descriptor and return the message
	close(socketDescriptor);

	return msgLen;
}

/*****************
* TCP Networking *
*****************/

// Function for sending a stream
void sendStream(void *msgObject, size_t objLen, in_addr *serverAddressBuffer, int destPortNum)
{
	// Initialize some values
	int socketDescriptor, msgLen;
	struct sockaddr_in serverAddress;
	struct hostent *server;

	// Creates socket file descriptor for socket communication
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDescriptor < 0)
		error("ERROR opening socket");

	// Sets up the server address
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr = *serverAddressBuffer;
	serverAddress.sin_port = htons(destPortNum);
	if (connect(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
    	error("ERROR connecting");

    // Writes
	msgLen = write(socketDescriptor, msgObject, objLen);
    if (msgLen < 0)
        error("ERROR writing to socket");

	//std::cout << "Sent message of length " << std::to_string(msgLen) << "\n";

	// Finally the close descriptor
	close(socketDescriptor);
	return;
}

// Function for accepting a stream
int receiveStream(void *buffer, size_t bufferSize, int receivePortNum)
{
	// Initialize some values
	int msgLen, reuseTrue, newSocketDescriptor;
	socklen_t senderLength;
	struct sockaddr_in receiverAddress, senderAddress;

	// Creates socket file descriptor for socket communication
	int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
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

	// Accepts the connection here
	newSocketDescriptor = accept(socketDescriptor, (struct sockaddr *) &senderAddress, &senderLength);
	if (newSocketDescriptor < 0)
		error("ERROR on accept");

	// Clears the buffer and reads from the accepted connection
	memset(buffer, 0, bufferSize);
	msgLen = read(newSocketDescriptor, buffer, bufferSize);

	if (msgLen < 0)
		error("ERROR on receiving");

	//std::cout << "Received message of length " << std::to_string(msgLen) << "\n";

	// Close descriptor and return the message
	close(socketDescriptor);
	close(newSocketDescriptor);
	return msgLen;
}

// Function for accepting a stream
int receiveStreamAddr(void *buffer, size_t bufferSize, int receivePortNum, in_addr *serverAddressBuffer)
{
	// Initialize some values
	int msgLen, reuseTrue, newSocketDescriptor;
	socklen_t senderLength;
	struct sockaddr_in receiverAddress, senderAddress;

	// Creates socket file descriptor for socket communication
	int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
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

	// Accepts the connection here
	newSocketDescriptor = accept(socketDescriptor, (struct sockaddr *) &senderAddress, &senderLength);
	if (newSocketDescriptor < 0)
		error("ERROR on accept");

	// Clears the buffer and reads from the accepted connection
	memset(buffer, 0, bufferSize);
	msgLen = read(newSocketDescriptor, buffer, bufferSize);

	// Stores the server address into the buffer
	*serverAddressBuffer = senderAddress.sin_addr;

	if (msgLen < 0)
		error("ERROR on receiving");

	//std::cout << "Received message of length " << std::to_string(msgLen) << "\n";

	// Close descriptor and return the message
	close(socketDescriptor);
	close(newSocketDescriptor);
	return msgLen;
}

