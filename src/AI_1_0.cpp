#include "AI_1_0.hpp"
#include "Boat.hpp"

AI_1_0::AI_1_0(Track *t, int my_player, int num_boats, int s, float acceleratingThreshold, float reversingThreshold, float turningThreshold)
{
	track = t;
	myPlayer = my_player;
	numBoats = num_boats;
	segAhead = s;
	accThreshold = acceleratingThreshold;
	revThreshold = reversingThreshold;
	turnThreshold = turningThreshold;
}
InputState AI_1_0::getCommand(std::vector<Boat*>* boats)
{
	vec2 pos(boats->at(myPlayer)->rigidBody->GetPosition().x, boats->at(myPlayer)->rigidBody->GetPosition().y);
	//vec2 vel(boats[myPlayer]->rigidBody->GetLinearVelocity().x, boats[myPlayer]->rigidBody->GetLinearVelocity().y);
	float ang = boats->at(myPlayer)->rigidBody->GetAngle();

	int target = boats->at(myPlayer)->segPosition + segAhead + 1;
	if (target >= track->N)
		target = track->N - 1;
	vec2 targetP = mul(add(track->l[target], track->r[target]), .5);
	vec2 targetDir = sub(targetP, pos);
	vec2 apos(cos(ang), sin(ang));
	targetDir = mul(targetDir, 1 / norm(targetDir));
	//vec2 vel1 = mul(vel, 1 / norm(vel));
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
