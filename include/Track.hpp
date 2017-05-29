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
float cross_z(vec2 a, vec2 b);

class Map
{
public:
	bool** grid;
	int x_n, y_n;
	float x_min, x_max, y_min, y_max;
	float x_cell, y_cell;
	Map(bool** grid, int x_n, int y_n, float x_min, float x_max, float y_min, float y_max, float x_cell, float y_cell);
};

class Track
{
public:
	int N;             //Number of segments
	vec2 *l, *r, *p;
	float *c;
	Track(int N, float step, float width, float smoothness, unsigned int seed);
	float getNewSegPosition(float currentSegPosition, vec2 pos);
	vec2* getInitialSoulPositions(int);
	vec2* getInitialBoatPositions(int, float, float);
	float* getInitialSegPositions(int, float, float);
	void addTrackToWorld(b2World&);
	Map getMap(float x_cell, float y_cell, float pad);
};

#endif /* Track_h */
