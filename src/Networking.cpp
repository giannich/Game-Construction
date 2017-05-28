#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Networking.hpp"
#include "Box2D/Box2D.h"
#include "Boat.hpp"

#define MAX_JSON_CHARS 4096

/****************
* Initial Setup *
****************/

void gameSetup(char **argv, std::vector <std::pair<std::string, int>> *broadcastList, std::vector<int> *playerTypeList)
{
	bool isHost;
	int recPortNum;
	int serverPortNum;
	int playerNum;
	int expectedPlayerNums;
	int aiPlayerNums;
	int totalNumberOfPlayers;

	// Client or host
	if(!strcmp(argv[1], "host"))
	{
		std::cout << "I am a host\n";
		isHost = true;
	}
	else if (!strcmp(argv[1], "client"))
	{
		std::cout << "I am a client\n";
		isHost = false;
	}
	else
		exit(1);

	if(isHost)
	{
		// Receiving port number
		recPortNum = atoi(argv[2]);

		// Expected number of players
		expectedPlayerNums = atoi(argv[3]);

		// Number of AI players
		aiPlayerNums = atoi(argv[4]);

		// Player number for host is always 0
		playerNum = 0;

		// NEEDS TO CREATE A LOCAL BOAT HERE
		playerTypeList->push_back(0);

		// For playerlist stuff
		std::string destIPAddress = "localhost";
		int bufferSize = sizeof(int);
		int *intBuffer = (int *) malloc(bufferSize);
		int tempPlayerPort;
		std::pair <std::string, int> tempPlayer;

		// Loop listening for players
		for(int i = 1; i <= expectedPlayerNums; i++)
		{
			std::cout << "Listening in for connections\n";

			// Gets the player's port number
			receiveDatagram(intBuffer, bufferSize, recPortNum);
			tempPlayerPort = *intBuffer;

			std::cout << "Accepted connection number " << std::to_string(i) << " from player port number " << std::to_string(tempPlayerPort) << "\n";

			// Sends back the player number
			*intBuffer = i;
			sendDatagram(intBuffer, bufferSize, destIPAddress, tempPlayerPort);

			// Adds contact information on the broadcastList
			tempPlayer = std::make_pair(destIPAddress, tempPlayerPort);
			broadcastList->push_back(tempPlayer);
			playerTypeList->push_back(1);

			std::cout << "Successfully created player number " << std::to_string(i) << "\n";
			// NEEDS TO CREATE A NETWORK BOAT HERE
		}

		// Loop for creating ai players
		for(int i = 0; i < aiPlayerNums; i++)
			playerTypeList->push_back(2);

		// Finally sends players a message indicating the total number of players
		totalNumberOfPlayers = expectedPlayerNums + aiPlayerNums + 1;
		*intBuffer = totalNumberOfPlayers;

		std::cout << "Sending total number of players to broadcast list\n";

		for(int i = 0; i < expectedPlayerNums; i++)
			sendDatagram(intBuffer, bufferSize, broadcastList->at(i).first, broadcastList->at(i).second);

		std::cout << "Successfully finished setup process for host\n";
	}
	else
	{
		// Receiving port number
		recPortNum = atoi(argv[2]);

		// Server's port number
		serverPortNum = atoi(argv[3]);

		// Server's ip address
		std::string destIPAddress = "localhost";

		// Port number
		int bufferSize = sizeof(int);
		int *intBuffer = (int *) malloc(bufferSize);

		// Sends the receiving port number to the server
		*intBuffer = recPortNum;
		sendDatagram(intBuffer, bufferSize, destIPAddress, serverPortNum);
		std::cout << "Sending receiving port number " << std::to_string(recPortNum) << " to server at port number " << std::to_string(serverPortNum) << "\n";

		// Gets the player number and assigns it to playerNum
		receiveDatagram(intBuffer, bufferSize, recPortNum);
		playerNum = *intBuffer;

		std::cout << "Assigned to player number " << std::to_string(playerNum) << "\n";

		// Finally receives the total number of players that are in the game from the host
		receiveDatagram(intBuffer, bufferSize, recPortNum);
		totalNumberOfPlayers = *intBuffer;

		std::cout << "There are a total of " << std::to_string(totalNumberOfPlayers) << " players in this game\n";

		// The broadcastlist will only have the server
		broadcastList->push_back(std::make_pair(destIPAddress, serverPortNum));

		// NEEDS TO CREATE NETWORK BOATS HERE IN A FOR LOOP
		for (int i = 0; i < totalNumberOfPlayers; i++)
		{
			// If local
			if (i == playerNum)
				playerTypeList->push_back(0);

			// If network
			else
				playerTypeList->push_back(1);
		}

		std::cout << "Successfully finished setup process for client\n";
	}

	return;
}

/***************
* InputStreams *
***************/

// Encodes the InputState and broadcasts it to everyone in the broadcast list
// This will be called by the LocalPlayerInputStream, AIInputStream, and NetworkPlayerInputStream
// TODO: FIGURE OUT WHERE TO CALL THIS!!!
void Networking::broadcastInputStream()
{
	std::cout << "Sending a new InputStream packet to " << std::to_string(broadcastSize) << " other receivers\n";

	// Encodes the inutStream into outputList
	inputStream->encodeInputStates(outputList);

	// Broadcasts the outputlist to all the destination addresses
	for (unsigned int i = 0; i < broadcastSize; i++)
		sendDatagram(outputList, MAX_FRAMES + 8, broadcastTargets->at(i).first, broadcastTargets->at(i).second);
}

// Receives a datagram and decodes it to the correct InputStream
// This will be called at the beginning of the game
void receiveInputStream(GameState *world, int receivePortNum, std::vector<int> *playerDiscardList)
{
	// Mallocs the encodedInputStream 
	char *encodedInputStream = (char *) malloc((MAX_FRAMES + 8) * sizeof(char));
	unsigned int playerNumber;

	// Loops and receives all datagrams
	while(1)
	{
		std::cout << "Waiting on new packets\n";
		// Receives a datagram
		receiveDatagram(encodedInputStream, MAX_FRAMES + 8, receivePortNum);

		// Decode the Player Number
		playerNumber = ((unsigned int) (encodedInputStream[MAX_FRAMES + 4]) & 255) +
					   ((unsigned int) (encodedInputStream[MAX_FRAMES + 5] << 8) & 65280) +
					   ((unsigned int) (encodedInputStream[MAX_FRAMES + 6] << 16) & 16711680) +
					   ((unsigned int) (encodedInputStream[MAX_FRAMES + 7] << 24) & 4278190080);

		// Skips if receiving own player packets
		if (std::find(playerDiscardList->begin(), playerDiscardList->end(), playerNumber) != playerDiscardList->end())
			continue;

		std::cout << "Received packet from player number " << std::to_string(playerNumber) << "\n";

		// Decode the InputStream in the right boat
		world->boats->at(playerNumber).inputStream->decodeInputStates(encodedInputStream);
	}
}

/*************
* GameStates *
*************/

/* 
Name:						Type:			Getter:					Setter:

- Linear Velocity X			b2Vec2			GetLinearVelocity()		SetLinearVelocity(const b2Vec2& v)
- Rotational Velocity 		float32			GetAngularVelocity()	SetAngularVelocity(float32 omega)
- Orientation 				float32			GetAngle()				SetTransform(const b2Vec2& position, float32 angle)
- Position 					b2Vec2			GetPosition()			SetTransform(const b2Vec2& position, float32 angle)
- Current Souls 			Int 			currentSouls 			currentSouls
- InputStream 				InputStream 	inputStream 			inputStream

http://stackoverflow.com/questions/2114466/creating-json-arrays-in-boost-using-property-trees

*/

// Encodes a GameState and sends it through UDP
void Networking::sendGameStateInfo(GameState *world)
{
	// Convert GameState into a ptree
	boost::property_tree::ptree pt;

	// Get the number of players
	int playerNum = world->boats->size();
	pt.put("playerNum", playerNum);

	// For each player, encode the information into a ptree
	// And add the ptree to the ptree list
	for (int i = 0; i < playerNum; i++)
	{
		// It is still a pointer here, so I don't think we can dynamically do this...
		Boat playerBoat = world->boats->at(i);

		pt.put("linearVelocityX" + std::to_string(i), playerBoat.rigidBody->GetLinearVelocity().x);
		pt.put("linearVelocityY" + std::to_string(i), playerBoat.rigidBody->GetLinearVelocity().y);
		pt.put("rotVelocity" + std::to_string(i), playerBoat.rigidBody->GetAngularVelocity());
		pt.put("orientation" + std::to_string(i), playerBoat.rigidBody->GetAngle());
		pt.put("positionX" + std::to_string(i), playerBoat.rigidBody->GetPosition().x);
		pt.put("positionY" + std::to_string(i), playerBoat.rigidBody->GetPosition().y);
		pt.put("currentSouls" + std::to_string(i), playerBoat.currentSouls);

		// Need to encode this from enum to chars?
		//pt.put("inputStream", playerBoat->inputStream);
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
	sendDatagram(&stringBuffer, bufferSize, "localhost", 12346);
}

// Receives a message from UDP and decodes it into a GameState
void Networking::receiveGameStateInfo(GameState *world)
{
	// Receive a char *
	char stringBuffer[MAX_JSON_CHARS];
	int msgLen = receiveDatagram(stringBuffer, MAX_JSON_CHARS, 12346);

	// Convert char * to string
	std::string midString(stringBuffer);

	// Convert string to a stringstream
	std::stringstream ss;
	ss << midString.substr(0, msgLen);

	// Debugging info
	std::cout << ss.str();

	// Convert stringstream to a ptree
	boost::property_tree::ptree pt;
	read_json(ss, pt);

	// Convert ptree to a GameState, careful, this is a long process

	// Gets the player number, and if it fails, will get 0
	int playerNum = pt.get<int>("playerNum", 0.0f);

	// Iterates through the boat list
	for (int i = 0; i < playerNum; i++)
	{
		// Linear Velocity
		float32 velx = pt.get<float32>("linearVelocityX" + std::to_string(i));
		float32 vely = pt.get<float32>("linearVelocityY" + std::to_string(i));
		world->boats->at(i).rigidBody->SetLinearVelocity(b2Vec2(velx, vely));

		// Rotational Velocity
		float32 rotvel = pt.get<float32>("rotVelocity" + std::to_string(i));
		world->boats->at(i).rigidBody->SetAngularVelocity(rotvel);

		// Orientation & Position
		float32 orient = pt.get<float32>("orientation" + std::to_string(i));
		float32 posx = pt.get<float32>("positionX" + std::to_string(i));
		float32 posy = pt.get<float32>("positionY" + std::to_string(i));
		world->boats->at(i).rigidBody->SetTransform(b2Vec2(posx, posy), orient);

		// Current Souls
		int souls = pt.get<int>("currentSouls" + std::to_string(i));
		world->boats->at(i).currentSouls = souls;

		// InputStream
		//InputStream input = pt.get<InputStream>("inputStream" + std::to_string(i));
		//world->boats->at(i).InputStream = input;
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