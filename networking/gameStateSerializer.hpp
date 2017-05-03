#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// Encodes a GameState and sends it through UDP
void encodeGameState(GameState gameState, std::string destIPAddress, int destPortNum);

// Receives a message from UDP and decodes it into a GameState
void decodeGameState(GameState &gameState, int receivePortNum);