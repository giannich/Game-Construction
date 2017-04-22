//
//  trackmaker.hpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/20/17.
//
//

#ifndef trackmaker_h
#define trackmaker_h
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
    return (vec2){a.x+b.x,a.y+b.y};
}
vec2 sub(vec2 a, vec2 b)
{
    return (vec2){a.x-b.x,a.y-b.y};
}
vec2 mul(vec2 a, float b)
{
    return (vec2){a.x*b,a.y*b};
}
float norm(vec2 a)
{
    return sqrt(a.x*a.x+a.y*a.y);
}
float dot(vec2 a, vec2 b)
{
    return a.x*b.x+a.y*b.y;
}
vec2 perp(vec2 a)
{
    return (vec2){a.y,-a.x};
}

class Track
{
public:
    int N;
    vec2 *l, *r, *p;
    float *c;
    Track(int N, float step, float width)
    {
        this->N = N;
        l = new vec2[N];
        r = new vec2[N];
        p = new vec2[N];
        c = new float[N];
        srand(time(NULL));
        l[0]={0,0};
        r[0]={width,0};
        p[0]={0,1};
        c[0]=0;
        for(int i=1; i<N; i++)
        {
            float  dist = rand()%100+3;
            
            if(dist>60)
            {
                p[i] = mul(p[i-1],step/norm(p[i-1]));
                r[i] = add(r[i-1],p[i]);
                l[i] = add(l[i-1],p[i]);
                c[i] = -dot(p[i],r[i]);
                //cout<<"("<<r[i].x<<", "<<r[i].y<<") ("<<l[i].x<<", "<<l[i].y<<") : "<<p[i].x<<"x +"<<p[i].y<<"y +"<<c[i]<<"= 0\t"<<"\n";
            }
            else
            {
                if(rand()%2)
                {
                    r[i] = add(r[i-1], mul(p[i-1],step/norm(p[i-1])));
                    vec2 P = add(r[i-1], mul(sub(r[i-1],l[i-1]),dist));
                    p[i] = sub(r[i],P);
                    l[i] = add(r[i],mul(p[i],1/dist));
                    p[i] = perp(p[i]);
                    p[i] = mul(p[i],norm(sub(l[i],l[i-1]))/norm(p[i]));
                    c[i] = -dot(p[i],r[i]);
                    if(dot(p[i],r[i-1])+c[i]>0)
                    {
                        p[i]=mul(p[i],-1);
                        c[i]=-c[i];
                    }
                    //cout<<"("<<r[i].x<<", "<<r[i].y<<") ("<<l[i].x<<", "<<l[i].y<<") : "<<p[i].x<<"x +"<<p[i].y<<"y +"<<c[i]<<"= 0\t"<<"("<<P.x<<", "<<P.y<<")"<<"\n";
                }
                else
                {
                    l[i] = add(l[i-1], mul(p[i-1],step/norm(p[i-1])));
                    vec2 P = add(l[i-1], mul(sub(l[i-1],r[i-1]),dist));
                    p[i] = sub(l[i],P);
                    r[i] = add(l[i],mul(p[i],1/dist));
                    p[i] = perp(p[i]);
                    p[i] = mul(p[i],norm(sub(r[i],r[i-1]))/norm(p[i]));
                    c[i] = -dot(p[i],l[i]);
                    if(dot(p[i],r[i-1])+c[i]>0)
                    {
                        p[i]=mul(p[i],-1);
                        c[i]=-c[i];
                    }
                }
            }
        }
    }
};

#endif /* trackmaker_h */
