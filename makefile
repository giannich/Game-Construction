CC = g++
INC = include
LIB_PATH = lib
LIB_FLAGS = -losgDB -losgViewer -losg -lOpenThreads -losgUtil $(LIB_PATH)/libBox2d.a
EXECUTABLE = gtest

all: $(EXECUTABLE)

$(EXECUTABLE): src/GraphicsTest.cpp
	$(CC) src/GraphicsTest.cpp -I $(INC) -L $(LIB_PATH) $(LIB_FLAGS) -o $(EXECUTABLE)

clean:
	rm $(EXECUTABLE)
