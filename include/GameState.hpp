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
#include "Box2d/Box2d.h"
#include "Boat.hpp"
#include <vector>

class GameState {
    Track *m_track;
    std::vector<Boat> *boats;
public:
    GameState(Track&);
    void addPlayer(Boat&);
    void update(float);
    void printDebugInfo();
};

#endif /* GameState_hpp */
