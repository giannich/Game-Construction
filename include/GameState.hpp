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
    std::vector<Boat> boats;
public:
    GameState();
};

#endif /* GameState_hpp */
