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
#include "Boat.hpp"
#include "Soul.hpp"

GameState::GameState(Track& track) {
	m_track = &track;
	boats = new std::vector<Boat*>();
	souls = new std::vector<Soul*>();
}

void GameState::addPlayer(Boat* boat) {
	boats->push_back(boat);
}

void GameState::addSoul(Soul* soul) {
	souls->push_back(soul);
}

void GameState::update(float deltaTime) {
	for(auto it = boats->begin(); it != boats->end(); ++it) {
		(*it)->update(deltaTime, *this);
		(*it)->segPosition = m_track->getNewSegPosition((*it)->segPosition, vec2((*it)->rigidBody->GetPosition().x, (*it)->rigidBody->GetPosition().y));
	}
	for(auto it = souls->begin(); it != souls->end(); ++it) {
		(*it)->update(deltaTime);
	}
}

void GameState::printDebugInfo() {
	boost::format fmt = boost::format(" %3.2f | ");
	for(auto it = boats->begin(); it != boats->end(); ++it) {
		//std::cout << fmt % it->rigidBody->GetPosition().x << it->rigidBody->GetPosition().y;
		//std::cout << fmt % it->segPosition;
	}
	std::cout << std::endl;
}

