#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <thread>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define BUFFER_SIZE 2048
#define PLAYER_NUM 4

using std::cout;
using std::cin;
using std::string;

typedef struct PlayerState
{
	int vertical;
	int horizontal;
	int firing;
} PlayerState;

typedef struct Vec2
{
	float x;
	float y;
} Vec2;

typedef struct PlayerData
{
	Vec2 position;
	Vec2 velocity;
	float rotation;
	float angularVelocity;
} PlayerData;

static int userType = 0;

void error(const char *msg);

// Sending and Receiving Thread Block
void sendThread();
void receiveThread();

// Sending Block
void sendDatagram(string msg, string destAddress, int destPortNum);
void sendPlayerInput(string playerState, string hostAddress, int hostPort);
void sendPlayerData(string playerData, string clientAddress, int clientPort);

// Receiving Block
string receiveDatagram(int receivePortNum);
string receivePlayerInput(int receivingPort);
string receivePlayerData(int receivingPort);

// Ecoding and Decoding Block
string encodePlayerInput(PlayerState playerState);
void decodePlayerInput(PlayerState *playerState, string msg);
string encodePlayerData(PlayerData dataArr[], int playerNum);
void decodePlayerData(PlayerData dataArr[], int playerNum, string msg);

// Utility Functions Block
void printPlayerData(PlayerData dataArr[], int playerNum);
void printPlayerInput(PlayerState playerState);
void resetPlayerData(PlayerData dataArr[], int playerNum);

// Error Generation
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

// Testing
int main(int argc, char *argv[])
{
	// Set what type of user it is
	userType = atoi(argv[1]);

	// Fork threads
	cout << "Forking Threads\n";
	std::thread send = std::thread(sendThread);
	std::thread receive = std::thread(receiveThread);
	
	// Join Threads
	receive.join();
	send.join();
	cout << "Joining Threads\n";

	return 0;
}

// Handles outgoing packets
void sendThread()
{
	string sndMsg;
	string userInput;
	int numberInput;

	// User is server
	if (userType)
	{
		cout << "Server: Starting send thread\n";

		// Populates the player data array with random values
		PlayerData playerDataArr[PLAYER_NUM];
		resetPlayerData(playerDataArr, PLAYER_NUM);

		while(1)
		{
			cin >> userInput;
			if (userInput == "send")
				sndMsg = encodePlayerData(playerDataArr, PLAYER_NUM);
			else if (userInput == "reset")
			{
				resetPlayerData(playerDataArr, PLAYER_NUM);
				cout << "Server: Successfully reset player data\n";
				cout << "==================================================\n";
				continue;
			}
			else if (userInput == "quit")
				sndMsg = userInput;
			else
			{
				cout << "Server: Unrecognized input\n";
				continue;
			}

			cout << "Server: Sending message: " << sndMsg << "\n";
			sendPlayerData(sndMsg, "localhost", 12346);
			if (sndMsg == "quit")
				break;
			cout << "==================================================\n";
		}

		cout << "Server: Closing send thread\n";
	}

	// User if client
	else
	{
		cout << "Client: Starting send thread\n";

		// Populates test state combinations
		PlayerState statesArray[8];
		PlayerState tempState;

		for (int a = 0; a < 2; a++)
			for (int b = 0; b < 2; b++)
				for (int c = 0; c < 2; c++)
				{
					tempState.vertical = a;
					tempState.horizontal = b;
					tempState.firing = c;
					statesArray[a + (b * 2) + (c * 4)] = tempState;
				}

		while(1)
		{
			cin >> userInput;
			if (userInput == "send")
			{
				// Need to check if it is within 0 to 7
				cin >> numberInput;
				tempState = statesArray[numberInput];
				sndMsg = encodePlayerInput(tempState);
			}
			else if (userInput == "quit")
				sndMsg = userInput;
			else
			{
				cout << "Server: Unrecognized input\n";
				continue;
			}

			cout << "Client: Sending message: " << sndMsg << "\n";
			sendPlayerInput(sndMsg, "localhost", 12345);
			if (userInput == "quit")
				break;
			cout << "==================================================\n";
		}

		cout << "Client: Closing send thread\n";
	}
}

// Handles incoming packets
void receiveThread()
{
	PlayerState tempState;
	string rcvMsg;

	// User is server
	if (userType)
	{
		cout << "Server: Starting receive thread\n";

		while(1)
		{
			rcvMsg = receivePlayerInput(12345);
			cout << "Server: Received Message: " << rcvMsg << "\n";
			if (rcvMsg == "quit")
				break;

			decodePlayerInput(&tempState, rcvMsg);
			printPlayerInput(tempState);

			cout << "==================================================\n";
		}

		cout << "Server: Closing receive thread\n";
	}

	// User if client
	else
	{
		cout << "Client: Starting receive thread\n";

		PlayerData playerDataArr[PLAYER_NUM];

		while(1)
		{
			rcvMsg = receivePlayerData(12346);
			cout << "Client: Received Message: " << rcvMsg << "\n";
			if (rcvMsg == "quit")
				break;

			decodePlayerData(playerDataArr, PLAYER_NUM, rcvMsg);
			printPlayerData(playerDataArr, PLAYER_NUM);

			cout << "==================================================\n";
		}

		cout << "Client: Closing reveice thread\n";
	}
}

// Function for sending a datagram
void sendDatagram(string msg, string destAddress, int destPortNum)
{
	// Initialize some values
	int socketDescriptor, msgLen;
	struct sockaddr_in serverAddress;
	struct hostent *server;

	// Creates socket file descriptor for socket communication
	socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketDescriptor < 0)
		error("ERROR opening socket");

	// Gets the servername
	server = gethostbyname("localhost");
	if (server == NULL) 
		error("ERROR no such host");

	// No idea whats going on here, but it's similar to hw1
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);
	serverAddress.sin_port = htons(destPortNum);

	// Converts the string into a char*
	char stringBuffer[msg.length()];
	std::size_t bufferSize = msg.copy(stringBuffer, msg.length(), 0);
	stringBuffer[msg.length()] = '\0';

	// Send a message
	cout << "Sending Message to port number " << destPortNum << "\n";
	msgLen = sendto(socketDescriptor, stringBuffer, bufferSize, 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (msgLen < 0)
		error("Error failed sendto");

	// Finally the close descriptor
	close(socketDescriptor);
	return;
}

// Function for translating PlayerState into a string and sending it
void sendPlayerInput(string playerState, string hostAddress, int hostPort)
{
	string msg = playerState;
	sendDatagram(msg, hostAddress, hostPort);	
}

// Function for translating PlayerData[] into JSON and then sending it
void sendPlayerData(string playerData, string clientAddress, int clientPort)
{
	string msg = playerData;
	sendDatagram(msg, clientAddress, clientPort);
}

// Function for accepting a datagram
string receiveDatagram(int receivePortNum)
{
	// Initialize some values
	string returnMessage;
	int msgLength, reuseTrue;
	socklen_t senderLength;
	struct sockaddr_in receiverAddress, senderAddress;
	char buffer[BUFFER_SIZE];

	// Rest buffer memory
	memset(buffer, 0, BUFFER_SIZE - 1);

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
	// TODO: Loop it to make it accept a certain number of bytes for the full message
	// Figure out if JSON can pack everything...
	msgLength = recvfrom(socketDescriptor, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&senderAddress, &senderLength);
	
	if (msgLength < 0)
		error("ERROR on receiving");

	// Converts the char* into string
	returnMessage = std::string(buffer);

	// Close descriptor and return the message
	close(socketDescriptor);
	return returnMessage;
}

// Function for receiving data and translating it into PlayerState
string receivePlayerInput(int receivingPort)
{
	// TODO: Translate msg into JSON format and then into a PlayerState object
	PlayerState playerInputState;
	string msg;
	msg = receiveDatagram(receivingPort);
	return msg;
}

// Function for receiving data and translating it into PlayerData[]
string receivePlayerData(int receivingPort)
{
	// TODO: Translate msg into JSON format and then into many PlayerData objects
	// Append those objects into an array and return that information
	PlayerData gameStateInfo[] = {};
	string msg;
	msg = receiveDatagram(receivingPort);
	return msg;
}

/**********************
* ENCODERS & DECODERS *
**********************/

// Encodes the player input from PlayerState into string
string encodePlayerInput(PlayerState playerState)
{
	string retMsg = std::to_string(playerState.vertical)
				  + "-" + std::to_string(playerState.horizontal)
				  + "-" + std::to_string(playerState.firing);
	return retMsg;
}

// Decodes the player input from PlayerState into string
void decodePlayerInput(PlayerState *playerState, string msg)
{
	// Converts the string into a char*
	char stringBuffer[msg.length()];
	std::size_t bufferSize = msg.copy(stringBuffer, msg.length(), 0);
	stringBuffer[msg.length()] = '\0';

	// Grabs the player state
	playerState->vertical = atoi(strtok(stringBuffer, "-"));
	playerState->horizontal = atoi(strtok(NULL, "-"));
	playerState->firing = atoi(strtok(NULL, "-"));
}

// Encodes the player input from PlayerState into string
string encodePlayerData(PlayerData dataArr[], int playerNum)
{
	string retMsg = std::to_string(playerNum);

	for (int i = 0; i < playerNum; i++)
	{
		retMsg += 	"-" + std::to_string(dataArr[i].position.x) + "-" + std::to_string(dataArr[i].position.y) +
					"-" + std::to_string(dataArr[i].velocity.x) + "-" + std::to_string(dataArr[i].velocity.y) +
					"-" + std::to_string(dataArr[i].rotation) + "-" + std::to_string(dataArr[i].angularVelocity);
	}
	return retMsg;
}

// Decodes the player data from string into an array
void decodePlayerData(PlayerData dataArr[], int playerNum, string msg)
{
	// Converts the string into a char*
	char stringBuffer[msg.length()];
	std::size_t bufferSize = msg.copy(stringBuffer, msg.length(), 0);
	stringBuffer[msg.length()] = '\0';

	// Should contain the number of players...
	strtok(stringBuffer, "-");

	for (int i = 0; i < playerNum; i++)
	{
		// Updates the player states
		dataArr[i].position.x = atoi(strtok(NULL, "-"));
		dataArr[i].position.y = atoi(strtok(NULL, "-"));
		dataArr[i].velocity.x = atoi(strtok(NULL, "-"));
		dataArr[i].velocity.y = atoi(strtok(NULL, "-"));
		dataArr[i].rotation = atoi(strtok(NULL, "-"));
		dataArr[i].angularVelocity = atoi(strtok(NULL, "-"));
	}
}

/*************************************
* PRETTY PRINTING AND OTHER UTILITES *
*************************************/

// Prints out the player input information
void printPlayerInput(PlayerState playerState)
{
	// Print out player state
	cout << "==================================================\n";
	cout << "Server: Received Following Player State:" 
		 << "\n\tVertical:\t" << std::to_string(playerState.vertical)
		 << "\n\tHorizontal:\t" << std::to_string(playerState.horizontal)
		 << "\n\tFiring:\t\t" << std::to_string(playerState.firing) << "\n";
}

// Prints out the game state information
void printPlayerData(PlayerData dataArr[], int playerNum)
{
	cout << "==================================================\n";
	cout << "Client: Received Following Player Data:\n";

	for (int i = 0; i < PLAYER_NUM; i++)
	{
		printf("Player number: %d", i);
		printf("\n\tPosition:\tx: %.2f y: %.2f", dataArr[i].position.x, dataArr[i].position.y);
		printf("\n\tVelocity:\tx: %.2f y: %.2f", dataArr[i].velocity.x, dataArr[i].velocity.y);
		printf("\n\tRotation:\t%.2f\n\tAngular:\t%.2f\n", dataArr[i].rotation, dataArr[i].angularVelocity);
	}
}

// Reseeds the player data
void resetPlayerData(PlayerData dataArr[], int playerNum)
{
	srand (time(NULL));

	for (int i = 0; i < playerNum; i++)
	{
		dataArr[i].position.x = rand() % 100 + 0;
		dataArr[i].position.y = rand() % 100 + 0;
		dataArr[i].velocity.x = rand() % 100 + 0;
		dataArr[i].velocity.y = rand() % 100 + 0;
		dataArr[i].rotation = rand() % 100 + 0;
		dataArr[i].angularVelocity = rand() % 100 + 0;
	}
}