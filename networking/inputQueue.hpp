#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using std::cout;

class InputQueue
{
public:

   // Constructor
      InputQueue(int InputQueueSize, int networkPlayer)
         : queueSize(InputQueueSize)
         , playerID(networkPlayer)
      {
         //cout << "In Constructor\n";

         queueBuffer = (InputState *) malloc(sizeof(int) * queueSize);
         readBuffer = (InputState *) malloc(sizeof(int) * queueSize);

         nextPointer = 0;
      }

      // Destructor
      ~InputQueue()
      {
         //cout << "In Destructor\n";
         free(readBuffer);
         free(queueBuffer);
      }

      // Returns player id
      int playerNum()
      {
         return playerID;
      }

      // Returns the frame number
      int frameNum()
      {
         return nextPointer;
      }

      // Pushes a bunch of input states
      void pushBlob(InputState *inputArray, int numItems)
      {
         //cout << "In pushBlob\n";

         // nextPointer is incremented ad infinitum, this is pretty much the frame number
         for (int i = 0; i < numItems; i++)
         {
            queueBuffer[nextPointer % queueSize] = inputArray[i];
            nextPointer++;
         }
      }

      // Reads a bunch of inputStates
      InputState *readBlob(int numItems)
      {
         //cout << "In readBlob\n";

         int tempPointer;

         // Will only return up to the original queue size elements
         if (numItems > queueSize)
            numItems = queueSize;

         // If there are at least queueSize elements, just starts from nextPointer
         // Otherwise starts from 0
         if (nextPointer > queueSize)
            tempPointer = nextPointer % queueSize;
         else
            tempPointer = 0;

         // Appends the each inputState object into the readBuffer
         for (int i = 0; i < numItems; i++)
            readBuffer[i] = queueBuffer[tempPointer++ % queueSize];

         // Finally returns the readBuffer
         return readBuffer;
      }

private:

      int queueSize;
      int nextPointer;
      int playerID;
      InputState *readBuffer;
      InputState *queueBuffer;
};