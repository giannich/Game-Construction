//
//  InputStream.cpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#include <stdio.h>
#include <iostream>
#include "InputStream.hpp"
#include "Box2D/Box2D.h"
#include "Boat.hpp"

std::string InputState::toString()
{
	std::string turnStr = "?", accStr = "?", fireStr = "?";

	switch (turn)
	{
		case Left:
		{
			turnStr = "Left";
		}
			break;
		case Neutral:
		{
			turnStr = "Neutral";
		}
			break;
		case Right:
		{
			turnStr = "Right";
		}
			break;
	}
	switch (acc)
	{
		case Accelerating:
		{
			accStr = "Accelerating";
		}
			break;
		case Reversing:
		{
			accStr = "Reversing";
		}
			break;
		case Idling:
		{
			accStr = "Idling";
		}
			break;
	}

	switch (fire)
	{
		case Firing:
		{
			fireStr = "Firing";
		}
			break;
		case NotFiring:
		{
			fireStr = "NotFiring";
		}
			break;
	}

	std::string retString = turnStr + " | " + accStr + " | " + fireStr;
	return retString;
}

// Transforms InputState into a char
char InputState::toChar()
{
	return (turn | acc | fire);
}

// Reads a single InputState
InputState InputStream::readSingleState(int targetFrameNumber)
{
	//std::cout << "In readSingleState looking for frame number: " << std::to_string(targetFrameNumber) << "\n";
	
	// Try catch block
	try
	{
		// This is when the physics engine is asking for a frame that is too old
		if ((currentFrameNumber - targetFrameNumber) >= circular_buffer.capacity())
			throw -1;

		// This is when the physics engine is asking for a frame that is not inputted yet
		else if (targetFrameNumber > currentFrameNumber)
			throw -2;
	}
	catch (int errorNum)
	{
		if (errorNum == -1)
		{
			std::cout << "Physics engine is asking for a frame that is too old!\n";
			exit(1);
		}
		else if (errorNum == -2)
		{
			std::cout << "Physics engine is asking for a frame that is too new!\n";
			exit(1);
		}
		else
		{
			std::cout << "This error should not exist!\n";
			exit(1);
		}	
	}

	// This is when the physics engine is asking for a frame that is present in the InputStream
	return circular_buffer.at(circular_buffer.size() - (currentFrameNumber - targetFrameNumber));
}

// Writes a single InputState
void InputStream::writeSingleState(InputState newInputState)
{
	//std::cout << "Writing inputstate for player number " << std::to_string(playerNum) << "\n";
	//std::cout << "For player number " << std::to_string(playerNum) << " the inputstate is: " << newInputState.toString() << "\n";
	circular_buffer.push_back(newInputState);
	currentFrameNumber++;
}

// Reads a bunch of InputStates
void InputStream::encodeInputStates(char *outputList)
{
	// Fills the outputList with all the elements from the buffer in order
	for (int i = 0; i < circular_buffer.size(); i++)
		outputList[i] = circular_buffer.at(i).toChar();

	// Debug info
	//std::cout << "Encoding player number " << std::to_string(playerNum) << "'s InputStream with CurrentFrameNumber: " << std::to_string(currentFrameNumber) << "\n";

	// Encode Frame Number
	outputList[MAX_FRAMES + 0] = (char) (currentFrameNumber);
	outputList[MAX_FRAMES + 1] = (char) (currentFrameNumber >> 8);
	outputList[MAX_FRAMES + 2] = (char) (currentFrameNumber >> 16);
	outputList[MAX_FRAMES + 3] = (char) (currentFrameNumber >> 24);

	// Encode Player Number
	outputList[MAX_FRAMES + 4] = (char) (playerNum);
	outputList[MAX_FRAMES + 5] = (char) (playerNum >> 8);
	outputList[MAX_FRAMES + 6] = (char) (playerNum >> 16);
	outputList[MAX_FRAMES + 7] = (char) (playerNum >> 24);
}

// Reads a bunch of InputStates
void InputStream::decodeInputStates(char *outputList)
{
	// Decode Frame Number
	unsigned int latestFrameNumber;

	latestFrameNumber = ((unsigned int) (outputList[MAX_FRAMES + 0]) & 255) +
						((unsigned int) (outputList[MAX_FRAMES + 1] << 8) & 65280) +
						((unsigned int) (outputList[MAX_FRAMES + 2] << 16) & 16711680) +
						((unsigned int) (outputList[MAX_FRAMES + 3] << 24) & 4278190080);
						
	int framesDifference = latestFrameNumber - currentFrameNumber;

	// Try catch block
	try
	{
		// Frames that are received are too old
		if (framesDifference < 0)
			throw -1;
	}
	catch (int errorNum)
	{
		if (errorNum == -1)
			std::cout << "Received frames are too old! Probably received out of order...\n";
		else
			std::cout << "This error should not exist!\n";
	}

	//std::cout << "Latest frame number: " << std::to_string(latestFrameNumber) << "\n";

	// Get the start and fill it up
	int startFromIndex;

	if (latestFrameNumber < MAX_FRAMES)
		startFromIndex = latestFrameNumber - framesDifference;
	else
		startFromIndex = MAX_FRAMES - framesDifference;

	//std::cout << "Latest frame number: " << std::to_string(latestFrameNumber) << "\n";
	//std::cout << "Starting from index: " << std::to_string(startFromIndex) << "\n";
	//std::cout << "For a total of frames: " << std::to_string(framesDifference) << "\n";

	for (int i = 0; i < framesDifference; i++)
		writeSingleState(InputState(outputList[startFromIndex + i]));

}

// Gets the current frame number
int InputStream::getCurrentFrameNumber()
{
	return currentFrameNumber;
}

void InputStream::setCurrentFrameNumber(int targetFrameNumber)
{
	currentFrameNumber = targetFrameNumber;
}

// Gets the current frame number
int InputStream::getBufferSize()
{
	return circular_buffer.size();
}

void LocalPlayerInputStream::update(float deltaT, GameState &gs) {
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		InputState iState = (*gs.boats)[playerNum].inputStream->lastInputState;
		switch (e.type) {
			case SDL_KEYDOWN: 
				switch (e.key.keysym.sym) {
					case SDLK_w:
						iState.acc = Accelerating;
						break;
					case SDLK_s:
						iState.acc = Reversing;
						break;
					case SDLK_a:
						//Our Up vector in graphics is negative, so invert the left-right turn axis.
						iState.turn = Right;
						break;
					case SDLK_d:
						iState.turn = Left;
						break;
					case SDLK_SPACE:
						iState.fire = Firing;
						break;
					default:
						break;
				}
				lastInputState = iState;
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.sym) {
					case SDLK_w:
						iState.acc = Idling;
						break;
					case SDLK_s:
						iState.acc = Idling;
						break;
					case SDLK_a:
						iState.turn = Neutral;
						break;
					case SDLK_d:
						iState.turn = Neutral;
						break;
					case SDLK_SPACE:
						iState.fire = NotFiring;
						break;
					default:
						break;
				}
				lastInputState = iState;
				break;
			default:
				break;
		}
	}

	writeSingleState(lastInputState);
	//std::cout << "Local Player broadcast!\n";
	networkingHandler->broadcastInputStream();
}

void AIInputStream::update(float deltaTime, GameState &gs) {
	b2Body *rigidBody = (*gs.boats)[playerNum].rigidBody;
	lastInputState = ai->getCommand(*gs.boats);

	// Writes in the inputstate and broadcasts it
	writeSingleState(lastInputState);
	//std::cout << "AI Player broadcast!\n";
	networkingHandler->broadcastInputStream();
}

// GIANNI'S CHANGE
void NetworkPlayerInputStream::update(float deltaTime, GameState &gs) {
	// Here we grab the inputstate by using readSingleState on the latest frame number
	//int latestFrame = getCurrentFrameNumber() - 0 - 1;
	if (getCurrentFrameNumber() > FRAME_LAG)
		lastInputState = readSingleState(getCurrentFrameNumber() - FRAME_LAG - 1);
	else
		lastInputState = readSingleState(getCurrentFrameNumber() - 1);
	//std::cout << "In network update for player number " << std::to_string(playerNum) << " looking for frame number: " << std::to_string(latestFrame) << "\n";
	//lastInputState = readSingleState(oldestFrame);

	// Only broadcast if it is a network inputstream at host level
	if (isBroadcasting)
	{
		std::cout << "Networking Player broadcast!\n";
		networkingHandler->broadcastInputStream();
	}
}
