CC = g++
INC = ../../OpenSceneGraph/include
LIB_PATH = ../../OpenSceneGraph/lib
LIB_FLAGS = -losgDB -losgViewer -losg -lOpenThreads -losgUtil
EXECUTABLE = gtest

all: $(EXECUTABLE)

$(EXECUTABLE): GraphicsTest.cpp
	$(CC) GraphicsTest.cpp -I $(INC) -L $(LIB_PATH) $(LIB_FLAGS) -o $(EXECUTABLE)

clean:
	rm $(EXECUTABLE)
