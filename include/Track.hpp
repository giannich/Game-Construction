#include "Box2D/Box2D.h"

#ifndef Track_h
#define Track_h

class vec2
{
public:
    float x,y;
	vec2(float x, float y);
    vec2(){}
};
vec2 add(vec2 a, vec2 b);
vec2 sub(vec2 a, vec2 b);
vec2 mul(vec2 a, float b);
float norm(vec2 a);
float norm2(vec2 a);
float dot(vec2 a, vec2 b);
vec2 perp(vec2 a);
float dist(vec2 a, vec2 b);
float dist2(vec2 a, vec2 b);
class Track
{
public:
	int N;             //Number of segments
	vec2 *l, *r, *p;
	float *c;
	Track(int N, float step, float width, float smoothness, unsigned int seed);
	float getNewSegPosition(float currentSegPosition, vec2 pos);
	void addTrackToWorld(b2World&);
};

#endif /* Track_h */
