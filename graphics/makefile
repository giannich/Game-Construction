CC=clang++
LIB_PATH=../OSG/lib/
INCLUDE_PATH=../OSG/include/
OPTIONS=-losgDB -losgViewer -losg -lOpenThreads -losgUtil -losgGA

graphicsTest: graphicsTest.cpp
	$(CC) -L $(LIB_PATH) -I $(INCLUDE_PATH) graphicsTest.cpp -o graphicsTest $(OPTIONS)

clean:
	rm graphicsTest