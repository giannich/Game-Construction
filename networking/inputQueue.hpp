#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define N 50

using std::cout;

typedef enum E_Code = {E_Success, E_Failure} E_Code;

class InputQueue
{
public:

      // Constructor
      InputQueue()
      {
         inQueue = new boost::circular_buffer(N);
         lastFrame = 0;
      }

      // Destructor
      ~InputQueue()
      {
         free(inQueue);
      }

      // Gets a specific frame InputState
      E_Code getStateForFrame(int frameNum, InputState &inputAddr);

      // Inserts a bunch of inputStates
      void writeInputQueue(InputState inputArray[N]);

      // Reads a bunch of inputStates
      void recentInputStates(InputState &outputAddr[]);

private:

      int lastFrame;
      boost::circular_buffer inQueue;
};