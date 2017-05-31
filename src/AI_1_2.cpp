#include "AI_1_2.hpp"
#include "Boat.hpp"

AI_1_2::AI_1_2(Track *t, int my_player, int num_boats, float width, float acceleratingThreshold, float reversingThreshold)
{
	track = t;
	myPlayer = my_player;
	numBoats = num_boats;
	L = new vec2[t->N];
	R = new vec2[t->N];
	N = t->N;
	for(int i = 0; i < N; i++)
	{

		vec2 diff = sub(t->r[i],t->l[i]);
		diff = mul(diff, width/2/norm(diff));
		L[i] = add(t->l[i],diff);
		R[i] = sub(t->r[i],diff);
	}
	accThreshold = acceleratingThreshold;
	revThreshold = reversingThreshold;
}
InputState AI_1_2::getCommand(std::vector<Boat>& boats)
{
	vec2 pos(boats[myPlayer].rigidBody->GetPosition().x, boats[myPlayer].rigidBody->GetPosition().y);
	vec2 vel(boats[myPlayer].rigidBody->GetLinearVelocity().x, boats[myPlayer].rigidBody->GetLinearVelocity().y);
	float ang = boats[myPlayer].rigidBody->GetAngle();
	float seg = boats[myPlayer].segPosition;
	
	int I = N-1;
	//float distToCrash = std::numeric_limits<float>::infinity();
	for(int i = seg + 1; i < N; i++)
	{
		if(cross_z(sub(L[i],pos),vel)>0 || cross_z(sub(R[i],pos),vel)<0)
		{
			I = i;
			/*if(cross_z(sub(L[i],pos),vel)>0)
			{
				if(i == seg + 1)
				{
					//distToCrash = 
				}
				//distToCrash = 
			}*/
			break;
		}
	}

	vec2 aim;
	if(dot(sub(L[I],pos),vel)/norm(sub(L[I],pos)) > dot(sub(R[I],pos),vel)/norm(sub(R[I],pos)))
		aim = L[I];
	else
		aim = R[I];

	vec2 targetDir = sub(aim, pos);
	vec2 apos(cos(ang), sin(ang));
	targetDir = mul(targetDir, 1 / norm(targetDir));
	//vec2 vel1 = mul(vel, 1 / norm(vel));
	//std::cout << targetDir.x << ", " << targetDir.y << std::endl;
	float d1 = dot(targetDir, apos);
	//Converting to Input 
	InputState is;
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