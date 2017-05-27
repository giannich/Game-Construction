
#include "Track.hpp"
#include<time.h>
#include<iostream>
vec2::vec2(float x, float y)
{
	this->x = x;
	this->y = y;
}
vec2 add(vec2 a, vec2 b)
{
	return vec2(a.x + b.x, a.y + b.y);
}
vec2 sub(vec2 a, vec2 b)
{
	return vec2(a.x - b.x, a.y - b.y);
}
vec2 mul(vec2 a, float b)
{
	return vec2(a.x*b, a.y*b);
}
float norm(vec2 a)
{
	return sqrt(a.x*a.x + a.y*a.y);
}
float norm2(vec2 a)
{
	return a.x*a.x + a.y*a.y;
}
float dot(vec2 a, vec2 b)
{
	return a.x*b.x + a.y*b.y;
}
vec2 perp(vec2 a)
{
	return vec2(a.y, -a.x);
}

float dist(vec2 a, vec2 b)
{
	return norm(sub(a, b));
}
float dist2(vec2 a, vec2 b)
{
	return norm2(sub(a, b));
}

Track::Track(int N, float step, float width, float smoothness = 15)
{
	this->N = N;
	l = new vec2[N];
	r = new vec2[N];
	p = new vec2[N];
	c = new float[N];
	srand(time(NULL));
	l[0] = { 0,0 };
	r[0] = { width,0 };
	p[0] = { 0,1 };
	c[0] = 0;
	int backStep = 1;
	for (int i = 1; i<N; i++)
	{
		float  d = rand() / (float)RAND_MAX*smoothness + 0.5;
		//d = 0.5;
		/*
		if (d>50)
		{
			p[i] = mul(p[i - 1], step / norm(p[i - 1]));
			r[i] = add(r[i - 1], p[i]);
			l[i] = add(l[i - 1], p[i]);
			c[i] = -dot(p[i], r[i]);
			//cout<<"("<<r[i].x<<", "<<r[i].y<<") ("<<l[i].x<<", "<<l[i].y<<") : "<<p[i].x<<"x +"<<p[i].y<<"y +"<<c[i]<<"= 0\t"<<"\n";
			//cout<<"1: "<<dist(r[i],l[i])<<endl;
		}
		else
		{*/
			if (rand() % 2)
			{
				r[i] = add(r[i - 1], mul(p[i - 1], step / norm(p[i - 1])));
				vec2 P = add(r[i - 1], mul(sub(r[i - 1], l[i - 1]), d));
				p[i] = sub(r[i], P);
				l[i] = add(r[i], mul(p[i], width / norm(p[i])));
				p[i] = perp(p[i]);
				p[i] = mul(p[i], 1/norm(sub(l[i], l[i - 1])) / norm(p[i]));
				c[i] = -dot(p[i], r[i]);
				if (dot(p[i], r[i - 1]) + c[i]>0)
				{
					p[i] = mul(p[i], -1);
					c[i] = -c[i];
				}
				//cout<<"("<<r[i].x<<", "<<r[i].y<<") ("<<l[i].x<<", "<<l[i].y<<") : "<<p[i].x<<"x +"<<p[i].y<<"y +"<<c[i]<<"= 0\t"<<"("<<P.x<<", "<<P.y<<")"<<"\n";
			}
			else
			{
				l[i] = add(l[i - 1], mul(p[i - 1], step / norm(p[i - 1])));
				vec2 P = add(l[i - 1], mul(sub(l[i - 1], r[i - 1]), d));
				p[i] = sub(l[i], P);
				r[i] = add(l[i], mul(p[i], width / norm(p[i])));
				p[i] = perp(p[i]);
				p[i] = mul(p[i], 1/norm(sub(r[i], r[i - 1])) / norm(p[i]));
				c[i] = -dot(p[i], l[i]);
				if (dot(p[i], r[i - 1]) + c[i]>0)
				{
					p[i] = mul(p[i], -1);
					c[i] = -c[i];
				}
			}
			//std::cout << "2: " << dot(r[i-1], p[i]) + c[i] << std::endl;
		//}
		float d2 = 4.5*step*step;                 //4.5=1.5*1.5+1.5*1.5
		for (int j = 0; j < i - 5; j++)
		{
			if (dist2(l[i], l[j]) <= d2)
			{
				i -= backStep;
				backStep++;
				if (i < 0)
				{
					backStep = 1;
					i = 0;
				}
				break;
			}
			if (dist2(l[i], r[j]) <= d2)
			{
				i -= backStep;
				backStep++;
				if (i < 0)
				{
					backStep = 1;
					i = 0;
				}
				break;
			}
			if (dist2(r[i], l[j]) <= d2)
			{
				i -= backStep;
				backStep++;
				if (i < 0)
				{
					backStep = 1;
					i = 0;
				}
				break;
			}
			if (dist2(r[i], r[j]) <= d2)
			{
				i -= backStep;
				backStep++;
				if (i < 0)
				{
					backStep = 1;
					i = 0;
				}
				break;
			}
		}
	}
}
float Track::getNewSegPosition(float currentSegPosition, vec2 pos)
{
	float d;
	//std::cout << currentSegPosition << "\n";
	if (currentSegPosition < 0)
		currentSegPosition = -1.5;
	for (int i = (int)currentSegPosition + 1; i < N; i++)
	{
		
		d = dot(pos, p[i]) + c[i];
		//std::cout << i << "\t" << i + d << "\t" << pos.x << ", " << pos.y << "\t" << p[i].x << ", " << p[i].y << ", " << c[i] << "\n";
		//if (d < -1 && currentSegPosition >= 0)
			//std::cout << "Something Wrong\n";
		if (d < 0)
		{
			
			return (i == 0) ? -.5 : (i + d);
		}
	}
	return N - 1 + dot(pos, p[N - 1]) + c[N - 1];
}
vec2* Track::getInitialSoulPositions(int numSouls)
{
	vec2 *sp = new vec2[numSouls];
	srand(time(NULL));
	for (int i = 0; i < numSouls; i++)
	{
		int seg = rand() % (N - 5) + 5;
		float p1 = rand() / (float)RAND_MAX;
		float p2 = rand() / (float)RAND_MAX;
		sp[i] = add(add(mul(l[seg] , p1 * p2), mul(l[seg - 1], (1 - p1) * p2)), add(mul(r[seg] , p1*(1 - p2)),mul(r[seg - 1], (1 - p1)*(1 - p2))));
	}
	return sp;
}

vec2* Track::getInitialBoatPositions(int numBoats, float width_offset, float legnth_offset)
{
	vec2 *bp = new vec2[numBoats];
	int boats_per_row = r[0].x / width_offset;
	int num_full_rows = numBoats / boats_per_row;
	int boats_in_last_row = numBoats % boats_per_row;
	int mark = 1;
	for (int i = 0; i < boats_in_last_row; i++)
	{
		float p1 = ((float)(2 * i + 1)) / (2 * boats_in_last_row);
		bp[i] = add(mul(l[1], p1), mul(r[1], (1 - p1)));
		mark = 2;
	}
	for (int i = 0; i < numBoats - boats_in_last_row; i++)
	{
		float p1 = ((float)(2 * (i % boats_per_row) + 1)) / (2 * boats_per_row);
		int row = mark + i / boats_per_row;
		bp[i + boats_in_last_row] = add(mul(l[row], p1), mul(r[row], (1 - p1)));
	}
	return bp;
}

float* Track::getInitialSegPositions(int numBoats, float width_offset, float legnth_offset)
{
	float *seg = new float[numBoats];
	int boats_per_row = r[0].x / width_offset;
	int num_full_rows = numBoats / boats_per_row;
	int boats_in_last_row = numBoats % boats_per_row;
	int mark = 1;
	for (int i = 0; i < boats_in_last_row; i++)
	{
		seg[i] = 0;
		mark = 2;
	}
	for (int i = 0; i < numBoats - boats_in_last_row; i++)
	{
		int row = mark + i / boats_per_row;
		seg[i + boats_in_last_row] = row;
	}
	return seg;
}

void Track::addTrackToWorld(b2World &b2WorldRef) {
	b2BodyDef bd;
	b2Body *ground = b2WorldRef.CreateBody(&bd);

	b2Vec2 *left = new b2Vec2[N];
	b2Vec2 *right = new b2Vec2[N];
	for(int i = 0; i < N; i++)
	{
	    left[i].Set(l[i].x, l[i].y);
	    right[i].Set(r[i].x, r[i].y);
	}
	
	b2ChainShape innerShape;
	innerShape.CreateChain(left, N);
	ground->CreateFixture(&innerShape, 0.0f);
	
	b2ChainShape outerShape;
	outerShape.CreateChain(right, N);
	ground->CreateFixture(&outerShape, 0.0f);
}

