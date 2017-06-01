#include "AI_1_3.hpp"
#include "Boat.hpp"

AI_1_3::AI_1_3(Track *t, int my_player, int num_boats, float width, float acceleratingThreshold, float reversingThreshold)
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
InputState AI_1_3::getCommand(std::vector<Boat>& boats,std::vector<Soul>& souls)
{
	vec2 pos(boats[myPlayer].rigidBody->GetPosition().x, boats[myPlayer].rigidBody->GetPosition().y);
	vec2 vel(boats[myPlayer].rigidBody->GetLinearVelocity().x, boats[myPlayer].rigidBody->GetLinearVelocity().y);
	float ang = boats[myPlayer].rigidBody->GetAngle();
	float seg = boats[myPlayer].segPosition;
	
	int I = N-1;
	float timeToCrash = std::numeric_limits<float>::infinity();
	for(int i = seg + 1; i < N; i++)
	{
		if(cross_z(sub(L[i],pos),vel)>0 || cross_z(sub(R[i],pos),vel)<0)
		{
			I = i;
			if(I==seg+1 && dot(vel,track->p[I])<0)
			{
				timeToCrash = std::numeric_limits<float>::infinity();//does not matter
			}
			else if(cross_z(sub(L[i],pos),vel)>0)
			{
				vec2 p = perp(sub(L[i],L[i-1]));
				timeToCrash = dot(sub(pos,L[i-1]),p)/dot(vel,p);
				if(timeToCrash<0)
					timeToCrash=-timeToCrash;
				//distToCrash = 
			}
			else
			{
				vec2 p = perp(sub(R[i],R[i-1]));
				timeToCrash = dot(sub(pos,R[i-1]),p)/dot(vel,p);
				if(timeToCrash<0)
					timeToCrash=-timeToCrash;
				//distToCrash = 
			}
			break;
		}
	}
	//if(timeToCrash)
	vec2 aim;// = mul(add(R[I],L[I]),.5);
	
	if(I>seg+1)
		if(dot(sub(L[I],pos),vel)/norm(sub(L[I],pos)) > dot(sub(R[I],pos),vel)/norm(sub(R[I],pos)))
			aim = R[I];
		else
			aim = L[I];
	else
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
	if (d1 > 0.999)
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
	std::printf("Time To Crash = %f\n", timeToCrash);

	//std::cout << d1 << "\t" << ang*180/3.14 <<" " << is.turn << " "<<apos.x*targetDir.y + apos.y*targetDir.x<<"\n";
	//std::cout << targetDir.x << "\t" << targetDir.y << "\t" << apos.x << "\t" << apos.y << "\n";
	return is;
}