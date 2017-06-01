CC = clang++
LIB_PATH = ../lib
INCLUDE_PATH = ../include
OPTIONS = -losgDB -losgViewer -losg -lOpenThreads -losgUtil -losgGA

graphicsTest: graphicsTest.cpp
	$(CC) -L $(LIB_PATH) -I $(INCLUDE_PATH) graphicsTest.cpp -o graphicsTest $(OPTIONS)

clean:
	rm graphicsTest