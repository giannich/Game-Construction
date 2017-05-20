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
						std::cout << "w";
						iState.acc = Accelerating;
						break;
					case SDLK_s:
						std::cout << "s";
						iState.acc = Reversing;
						break;
					case SDLK_a:
						std::cout << "a";
						iState.turn = Left;
						break;
					case SDLK_d:
						std::cout << "d";
						iState.turn = Right;
						break;
					case SDLK_SPACE:
						std::cout << "_";
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
						std::cout << "w";
						iState.acc = Idling;
						break;
					case SDLK_s:
						std::cout << "s";
						iState.acc = Idling;
						break;
					case SDLK_a:
						std::cout << "a";
						iState.turn = Neutral;
						break;
					case SDLK_d:
						std::cout << "d";
						iState.turn = Neutral;
						break;
					case SDLK_SPACE:
						std::cout << "_";
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
	lastInputState = ai->getCommand(vec2(rigidBody->GetPosition().x, rigidBody->GetPosition().y), vec2(rigidBody->GetLinearVelocity().x, rigidBody->GetLinearVelocity().y), rigidBody->GetAngle(), (*gs.boats)[playerNum].segPosition);
}
