//
//  GameState.cpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#include "GameState.hpp"
#include <iostream>
#include <boost/format.hpp>

GameState::GameState(Track& track) {
	m_track = &track;
	boats = new std::vector<Boat>();
}

void GameState::addPlayer(Boat& boat) {
	boats->push_back(boat);
}

void GameState::update(float deltaTime) {
	for(auto it = boats->begin(); it != boats->end(); ++it) {
		it->update(deltaTime);
		it->segPosition = m_track->getNewSegPosition(it->segPosition, vec2(it->rigidBody->GetPosition().x, it->rigidBody->GetPosition().y));
	}
}

void GameState::printDebugInfo() {
	boost::format fmt = boost::format(" %3.2f | ");
	for(auto it = boats->begin(); it != boats->end(); ++it) {
		std::cout << fmt % it->segPosition;
	}
	std::cout << std::endl;
}

