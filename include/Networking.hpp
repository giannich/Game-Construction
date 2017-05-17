#ifndef Networking_hpp
#define Networking_hpp

void error(const char *msg);
void sendDatagram(void *msgObject, size_t objLen, std::string destIPAddress, int destPortNum);
void receiveDatagram(void *buffer, size_t bufferSize, int receivePortNum);

#endif /* Networking_hpp */
