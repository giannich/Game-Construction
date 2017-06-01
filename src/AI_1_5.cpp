#include "AI_1_5.hpp"
#include "Boat.hpp"

AI_1_5::AI_1_5(Track *t, int my_player, int num_boats, float width, float acceleratingThreshold, float reversingThreshold)
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
InputState AI_1_5::getCommand(std::vector<Boat>& boats)
{
	vec2 pos(boats[myPlayer].rigidBody->GetPosition().x, boats[myPlayer].rigidBody->GetPosition().y);
	vec2 vel(boats[myPlayer].rigidBody->GetLinearVelocity().x, boats[myPlayer].rigidBody->GetLinearVelocity().y);
	float ang = boats[myPlayer].rigidBody->GetAngle();
	float seg = boats[myPlayer].segPosition;
	vec2 apos(cos(ang), sin(ang));
	
	int nextPlayer;
	float nextPlayerSeg = N;
	for(int i = 0; i < boats.size(); i++)
	{
		if(boats[i].segPosition > seg)
			if(boats[i].segPosition < nextPlayerSeg)
			{
				nextPlayer = i;
				nextPlayerSeg = boats[i].segPosition;
			}
	}
	

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
		aim = mul(add(R[I],L[I]),.5);

	if(nextPlayerSeg < I)
	{
		vec2 myPoints[3];
		myPoints[0] = add(pos, apos);
		myPoints[1] = pos;
		myPoints[2] = sub(pos, apos);



		vec2 oppPos(boats[nextPlayer].rigidBody->GetPosition().x, boats[nextPlayer].rigidBody->GetPosition().y); 
		vec2 oppVel(boats[nextPlayer].rigidBody->GetLinearVelocity().x, boats[nextPlayer].rigidBody->GetLinearVelocity().y); 
		float oppAng = boats[nextPlayer].rigidBody->GetAngle();
		vec2 oppApos(cos(oppAng), sin(oppAng));

		vec2 vRel = sub(vel, oppVel);

		vec2 oppPoints[3];
		oppPoints[0] = add(oppPos, oppApos);
		oppPoints[1] = oppPos;
		oppPoints[2] = sub(oppPos, oppApos);

		float timeToCrash2 = std::numeric_limits<float>::infinity();
		float radius = 0.5;
		for(int i = 0; i < 3; i++)
			for(int j = 0; j < 3; j++)
			{
				vec2 pRel = sub(oppPoints[i], myPoints[j]);
				float a = dot(vRel,vRel);
				float b = -2*(dot(vRel,pRel));
				float c = dot(pRel,pRel) - 4*radius*radius;
				if(b*b - 4*a*c < 0 || a==0)
					continue;

				float t1 = (-b + sqrt(b*b - 4*a*c))/(2*a);
				float t2 = (-b - sqrt(b*b - 4*a*c))/(2*a);
				if(t1>0 && t1 < timeToCrash2)
					timeToCrash2 = t1;
				if(t2>0 && t2 < timeToCrash2)
					timeToCrash2 = t2;

			}
		if(timeToCrash2 < timeToCrash)
		{
			vec2 oppPos2 = add(oppPos, mul(oppVel,timeToCrash2));
			oppPoints[0] = add(oppPos2, oppApos);
			oppPoints[1] = sub(oppPos2, oppApos);
			vec2 points[4];
			vec2 perp1 = perp(sub(oppPoints[0],pos));
			perp1 = mul(perp1, 3*radius/norm(perp1));
			points[0] = add(oppPoints[0],perp1);
			points[1] = sub(oppPoints[0],perp1);
			perp1 = perp(sub(oppPoints[1],pos));
			perp1 = mul(perp1, 3*radius/norm(perp1));
			points[2] = add(oppPoints[1],perp1);
			points[3] = sub(oppPoints[1],perp1);
			float max=-std::numeric_limits<float>::infinity(),min=std::numeric_limits<float>::infinity();
			int Imax=0, Imin = 0;
			for(int i = 0; i<4; i++)
			{
				float z = cross_z(sub(points[i],pos),vel);
				if(z>max)
				{
					Imax = i;
					max = z;
				}
				if(z<min)
				{
					Imin = i;
					min = z;
				}

			}
			I = N;
			int mark = Imax;
			for(int i = seg + 1; i < N; i++)
			{
				if(cross_z(sub(L[i],pos),sub(points[Imax],pos))>0 || cross_z(sub(R[i],pos),sub(points[Imax],pos))<0)
				{
					I = i;
					break;
				}
			}
			for(int i = seg + 1; i < N; i++)
			{
				if(cross_z(sub(L[i],pos),sub(points[Imin],pos))>0 || cross_z(sub(R[i],pos),sub(points[Imin],pos))<0)
				{
					break;
				}
				else if(i>I)
				{
					mark = Imin;
					break;
				}
			}
			aim = points[mark];
		}
		//std::cout << "timeToCrash2 = " << timeToCrash2<<std::endl;



	}

	vec2 targetDir = sub(aim, pos);
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
	//std::printf("Time To Crash = %f\n", timeToCrash);

	//std::cout << d1 << "\t" << ang*180/3.14 <<" " << is.turn << " "<<apos.x*targetDir.y + apos.y*targetDir.x<<"\n";
	//std::cout << targetDir.x << "\t" << targetDir.y << "\t" << apos.x << "\t" << apos.y << "\n";
	return is;
}