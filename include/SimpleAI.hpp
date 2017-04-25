#pragma once
#include "Testbed\include\Track.hpp"
#include "Testbed\include\InputStream.hpp"

class SimpleAI
{
public:
	Track *track;
	SimpleAI(Track *t)
	{
		track = t;
	}
	InputState getCommand(vec2 pos, vec2 vel, float segPosition, int segAhead=5)
	{
		int target = (int)segPosition + segAhead + 1;
		if (target >= track->N)
			target = track->N - 1;
		vec2 targetP = mul(add(track->l[target], track->r[target]), .5);
		vec2 targetDir = sub(targetP, pos);
		float v = norm(vel);
		targetDir = mul(targetDir, v / norm(targetDir));
		vec2 correctionV = sub(targetDir, vel);
		float dv = norm(correctionV);

		//Converting to Input 
		InputState is;
		if (dv * 6 < v)
			is.turn = Neutral;
		else
			if (vel.x*targetDir.y + vel.y*targetDir.x > 0)
				is.turn = Left;
			else
				is.turn = Right;
		float d = dot(targetDir, vel) / (v*v);
		if (d > .5)
			is.acc = Accelerating;
		else if (d > .1)
			is.acc = Idling;
		else
			is.acc = Reversing;
		is.fire = NotFiring;
		return is;
	}
};