//
//  InputStream.cpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#include <stdio.h>
#include <iostream>
#include "InputStream.hpp"
#include "Box2D/Box2D.h"
#include "Boat.hpp"

void LocalPlayerInputStream::update(float deltaT, GameState &gs) {
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		InputState iState = (*gs.boats)[playerNum].inputStream->lastInputState;
		switch (e.type) {
			case SDL_KEYDOWN: 
				switch (e.key.keysym.sym) {
					case SDLK_w:
						iState.acc = Accelerating;
						break;
					case SDLK_s:
						iState.acc = Reversing;
						break;
					case SDLK_a:
						//Our Up vector in graphics is negative, so invert the left-right turn axis.
						iState.turn = Right;
						break;
					case SDLK_d:
						iState.turn = Left;
						break;
					case SDLK_SPACE:
						iState.fire = Firing;
						break;
					default:
						break;
				}
				lastInputState = iState;
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.sym) {
					case SDLK_w:
						iState.acc = Idling;
						break;
					case SDLK_s:
						iState.acc = Idling;
						break;
					case SDLK_a:
						iState.turn = Neutral;
						break;
					case SDLK_d:
						iState.turn = Neutral;
						break;
					case SDLK_SPACE:
						iState.fire = NotFiring;
						break;
					default:
						break;
				}
				lastInputState = iState;
				break;
			default:
				break;
		}
	}
}

void AIInputStream::update(float deltaTime, GameState &gs) {
	b2Body *rigidBody = (*gs.boats)[playerNum].rigidBody;
	lastInputState = ai->getCommand(*gs.boats);
}
