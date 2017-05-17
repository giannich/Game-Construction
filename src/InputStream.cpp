//
//  InputStream.cpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#include <iostream>
#include "InputStream.hpp"

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

// Constructor
InputStream::InputStream()
{
      currentFrameNumber = 0;
}

// Reads a single InputState
void InputStream::readSingleState(int targetFrameNumber, InputState &inputAddr)
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
    		std::cout << "Physics engine is asking for a frame that is too old!\n";
    	else if (errorNum == -1)
    		std::cout << "Physics engine is asking for a frame that is too new!\n";
    	else
    		std::cout << "This error should not exist!\n";
  	}

  	// This is when the physics engine is asking for a frame that is present in the InputStream
  	inputAddr = circular_buffer.at(circular_buffer.size() - (currentFrameNumber - targetFrameNumber));
}

// Writes a single InputState
void InputStream::writeSingleState(InputState newInputState)
{
    circular_buffer.push_back(newInputState);
    currentFrameNumber++;
}

// Reads a bunch of InputStates
void InputStream::readAllInputStates(char *outputList)
{
	// Filles the outputList with all the elements from the buffer in order
	for (int i = 0; i < circular_buffer.size(); i++)
		outputList[i] = circular_buffer.at(i).toChar();

	// Debug info
	std::cout << "currentFrameNumber: " << std::to_string(currentFrameNumber) << "\n";

	// Encode
	outputList[MAX_FRAMES + 0] = (char) (currentFrameNumber);
	outputList[MAX_FRAMES + 1] = (char) (currentFrameNumber >> 8);
	outputList[MAX_FRAMES + 2] = (char) (currentFrameNumber >> 16);
	outputList[MAX_FRAMES + 3] = (char) (currentFrameNumber >> 24);
}

// Reads a bunch of InputStates
void InputStream::writeAllInputStates(char *outputList)
{
	// Decode
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













