//
//  GameState.hpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#ifndef GameState_hpp
#define GameState_hpp

#include <stdio.h>
#include "Box2D/Box2D.h"
#include "Track.hpp"
#include <vector>

class Boat;

class GameState {
public:
    Track *m_track;
    std::vector<Boat> *boats;
    GameState(Track&);
    void addPlayer(Boat&);
    void update(float);
    void printDebugInfo();
};

#endif /* GameState_hpp */
