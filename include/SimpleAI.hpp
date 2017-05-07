#ifndef Simple_AI_hpp

#define Simple_AI_hpp

#include "Track.hpp"
#include "InputStream.hpp"
#include <iostream>
#include <cmath>

class SimpleAI
{
public:
	Track *track;
	int segAhead;
	float accThreshold;
	float revThreshold;
	float turnThreshold;
	SimpleAI(Track *t, int s = 5, float acceleratingThreshold=0.8, float reversingThreshold=0.5, float turningThreshold=.99)
	{
		track = t;
		segAhead = s;
		accThreshold = acceleratingThreshold;
		revThreshold = reversingThreshold;
		turnThreshold = turningThreshold;
	}
	InputState getCommand(vec2 pos, vec2 vel, float ang, float segPosition)
	{
		//std::cout << segPosition << "\t";
		int target = (int)segPosition + segAhead + 1;
		if (target >= track->N)
			target = track->N - 1;
		vec2 targetP = mul(add(track->l[target], track->r[target]), .5);
		vec2 targetDir = sub(targetP, pos);
		vec2 apos(cos(ang), sin(ang));
		targetDir = mul(targetDir, 1 / norm(targetDir));
		vec2 vel1 = mul(vel, 1 / norm(vel));
		//std::cout << targetDir.x << ", " << targetDir.y << std::endl;
		float d1 = dot(targetDir, apos);
		//Converting to Input 
		InputState is;
		if (d1 > turnThreshold)
			is.turn = Neutral;
		else
			if (apos.x*targetDir.y - apos.y*targetDir.x > 0)
				is.turn = Left;
			else
				is.turn = Right;
		//float d = dot(targetDir, vel) / (v*v);
		if (d1 > accThreshold)
			is.acc = Accelerating;
		else if (d1 > revThreshold)
			is.acc = Idling;
		else
			is.acc = Reversing;
		is.fire = NotFiring;

		//std::cout << d1 << "\t" << ang*180/3.14 <<" " << is.turn << " "<<apos.x*targetDir.y + apos.y*targetDir.x<<"\n";
		//std::cout << targetDir.x << "\t" << targetDir.y << "\t" << apos.x << "\t" << apos.y << "\n";
		return is;
	}
};
#endif
