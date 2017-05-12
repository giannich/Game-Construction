CC = g++
INC = include
LIB_PATH = lib
LIB_FLAGS = -std=c++11 -losgDB -losgViewer -losg -lOpenThreads -losgUtil -losgGA $(LIB_PATH)/libBox2d.a
CC_FLAGS = -rpath .
EXECUTABLE = gtest

all: $(EXECUTABLE)

$(EXECUTABLE): src/GraphicsTest.cpp src/Boat.cpp src/Track.cpp src/GameState.cpp
	$(CC) src/GraphicsTest.cpp src/Boat.cpp src/Track.cpp src/GameState.cpp $(CC_FLAGS) -I $(INC) -L $(LIB_PATH) $(LIB_FLAGS) -o $(EXECUTABLE)

graphics: src/Graphics.cpp src/Track.cpp
	$(CC) src/Graphics.cpp src/Track.cpp $(CC_FLAGS) -I $(INC) -L $(LIB_PATH) $(LIB_FLAGS) -o graphics

clean:
	rm $(EXECUTABLE) graphics
