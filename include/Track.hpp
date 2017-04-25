#ifndef Track_h
#define Track_h

#include<time.h>
#include<iostream>
class vec2
{
public:
    float x,y;
    vec2(float x, float y)
    {
        this->x=x;
        this->y=y;
    }
    vec2(){}
};
vec2 add(vec2 a, vec2 b)
{
    return vec2(a.x+b.x,a.y+b.y);
}
vec2 sub(vec2 a, vec2 b)
{
    return vec2(a.x-b.x,a.y-b.y);
}
vec2 mul(vec2 a, float b)
{
    return vec2(a.x*b,a.y*b);
}
float norm(vec2 a)
{
    return sqrt(a.x*a.x+a.y*a.y);
}
float norm2(vec2 a)
{
	return a.x*a.x + a.y*a.y;
}
float dot(vec2 a, vec2 b)
{
    return a.x*b.x+a.y*b.y;
}
vec2 perp(vec2 a)
{
    return vec2(a.y,-a.x);
}

float dist(vec2 a, vec2 b)
{
	return norm(sub(a, b));
}
float dist2(vec2 a, vec2 b)
{
	return norm2(sub(a, b));
}
class Track
{
public:
	int N;             //Number of segments
	vec2 *l, *r, *p;
	float *c;
	Track(int N, float step, float width, float smoothness = 15)
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
		int backStep=1;
		for (int i = 1; i<N; i++)
		{
			float  d = rand() / (float)RAND_MAX*smoothness + 0.5;
			//d = 0.5;
			if (d>10)
			{
				p[i] = mul(p[i - 1], step / norm(p[i - 1]));
				r[i] = add(r[i - 1], p[i]);
				l[i] = add(l[i - 1], p[i]);
				c[i] = -dot(p[i], r[i]);
				//cout<<"("<<r[i].x<<", "<<r[i].y<<") ("<<l[i].x<<", "<<l[i].y<<") : "<<p[i].x<<"x +"<<p[i].y<<"y +"<<c[i]<<"= 0\t"<<"\n";
				//cout<<"1: "<<dist(r[i],l[i])<<endl;
			}
			else
			{
				if (rand() % 2)
				{
					r[i] = add(r[i - 1], mul(p[i - 1], step / norm(p[i - 1])));
					vec2 P = add(r[i - 1], mul(sub(r[i - 1], l[i - 1]), d));
					p[i] = sub(r[i], P);
					l[i] = add(r[i], mul(p[i], width / norm(p[i])));
					p[i] = perp(p[i]);
					p[i] = mul(p[i], norm(sub(l[i], l[i - 1])) / norm(p[i]));
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
					p[i] = mul(p[i], norm(sub(r[i], r[i - 1])) / norm(p[i]));
					c[i] = -dot(p[i], l[i]);
					if (dot(p[i], r[i - 1]) + c[i]>0)
					{
						p[i] = mul(p[i], -1);
						c[i] = -c[i];
					}
				}
				//cout<<"2: "<<dist(r[i],l[i])<<endl;
			}
			float d2=4.5*step*step;                 //4.5=1.5*1.5+1.5*1.5
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
	float getNewSegPosition(float currentSegPosition, vec2 pos)
	{
		float d;
		for (int i = (int)currentSegPosition + 1; i < N; i++)
		{
			d = dot(pos, p[i]) + c[i];
			if (d < -1)
				std::cout << "Something Wrong\n";
			if (d < 0)
				return i + d;
		}
		return N - 1 + dot(pos, p[N - 1]) + c[N - 1];
	}
};



#endif /* Track_h */
