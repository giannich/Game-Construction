CC = g++
INC = include
LIB_PATH = lib
LIB_FLAGS = -std=c++11 -losgDB -losgViewer -losg -lOpenThreads -losgUtil -losgGA -lpthread $(LIB_PATH)/Box2D/libBox2D.a 
CC_FLAGS = 
EXECUTABLE = gtest

all: $(EXECUTABLE)

$(EXECUTABLE): src/*.cpp
	$(CC) src/*.cpp $(CC_FLAGS) -I $(INC) -L $(LIB_PATH) -F $(LIB_PATH) `sdl2-config --cflags --libs` $(LIB_FLAGS) -o $(EXECUTABLE)

graphics: src/Graphics.cpp
	$(CC) src/Graphics.cpp $(CC_FLAGS) -I $(INC) -L $(LIB_PATH) $(LIB_FLAGS) -o graphics

clean:
	rm $(EXECUTABLE)
