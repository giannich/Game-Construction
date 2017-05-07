//
//  BoatTrack.h
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/12/17.
//
//

#ifndef BoatTrack_h
#define BoatTrack_h

#include "Testbed\include\Boat.hpp"
#include "Testbed\include\InputStream.hpp"
#include <math.h>
#include "Testbed\include\Track.hpp"
#include "Testbed\include\SimpleAI.hpp"

#define CIRCLE_SEGMENTS 30

class BoatTest: public Test
{
public:
    Boat *m_boat, *m_boat2, *m_boat3;
	SimpleAI *ai, *ai2, *ai3;
	Track *track;
    BoatTest()
    {
        //Zero-G
        m_world->SetGravity(b2Vec2(0.0f, 0.0f));
		//b2World *ai_world = new b2World(b2Vec2(0.0f, 0.0f));
        //Create track sides
        {
//            float innerRadius = 50.0f;
//            float outerRadius = 100.0f;
            
            {
                b2BodyDef bd;
                b2Body* ground = m_world->CreateBody(&bd);
                
//                // One more to close the loop
//                b2Vec2 inner[CIRCLE_SEGMENTS+1];
//                b2Vec2 outer[CIRCLE_SEGMENTS+1];

//                for(int i = 0; i < CIRCLE_SEGMENTS+1; ++i)
//                {
//                    float theta = i * 2 * b2_pi /CIRCLE_SEGMENTS;
//                    inner[i].Set(innerRadius * cos(theta),
//                                 innerRadius * sin(theta));
//                    outer[i].Set(outerRadius * cos(theta),
//                                 outerRadius * sin(theta));
//                }
				const int n = 1000;
                track = new Track(n,25.0f,50.0f,4);
				
                b2Vec2 left[n];
                b2Vec2 right[n];
                for(int i = 0; i < n; i++)
				{
                    left[i].Set(track->l[i].x, track->l[i].y);
                    right[i].Set(track->r[i].x, track->r[i].y);
                }
                
                b2ChainShape innerShape;
                innerShape.CreateChain(left, n);
                ground->CreateFixture(&innerShape, 0.0f);
                
                b2ChainShape outerShape;
                outerShape.CreateChain(right, n);
                ground->CreateFixture(&outerShape, 0.0f);

				ai = new SimpleAI(track,3,.7,.5,.99);
				ai2 = new SimpleAI(track,3,.8,.5,.99);
				ai3 = new SimpleAI(track,3,.8,.3,.99);
            }
        }
        
        //Create Boat
        {
            b2World& m_worldRef = *m_world;
            b2Vec2 initPos = b2Vec2(5.0f,-5.0f);
			b2Vec2 initPos2 = b2Vec2(25.0f, -5.0f);
			b2Vec2 initPos3 = b2Vec2(45.0f, -5.0f);
            m_boat = new Boat(initPos, m_worldRef, ai);
			m_boat2 = new Boat(initPos2, m_worldRef, ai2);
			m_boat3 = new Boat(initPos2, m_worldRef, ai3);
			m_boat->rigidBody->SetTransform(m_boat->rigidBody->GetPosition(), 1.57);
			m_boat2->rigidBody->SetTransform(m_boat2->rigidBody->GetPosition(), 1.57);
			m_boat3->rigidBody->SetTransform(m_boat3->rigidBody->GetPosition(), 1.57);
        }
    }
    
    void Keyboard(int key)
    {
		switch (key)
		{
			case GLFW_KEY_W:
			{
				m_boat->inputState->acc = Accelerating;
			}
			break;
			case GLFW_KEY_S:
			{
				m_boat->inputState->acc = Reversing;
			}
			break;

			case GLFW_KEY_A:
			{
				m_boat->inputState->turn = Left;
			}
			break;

			case GLFW_KEY_D:
			{
				m_boat->inputState->turn = Right;
			}
			break;
		}
    }
    
    void KeyboardUp(int key)
    {
        switch (key)
        {
            case GLFW_KEY_W:
            {
                m_boat->inputState->acc = Idling;
            }
                break;
            case GLFW_KEY_S:
            {
                m_boat->inputState->acc = Idling;
            }
                break;

                
            case GLFW_KEY_A:
            {
                m_boat->inputState->turn = Neutral;
            }
                break;
                
            case GLFW_KEY_D:
            {
                m_boat->inputState->turn = Neutral;
            }
                break;
        }
    }
    
    void Step(Settings* settings)
    {
        //Center Camera
        g_camera.m_center = m_boat->rigidBody->GetPosition();
        
        //Apply forces based on input state
        m_boat->update(settings->hz);
		m_boat2->update(settings->hz);
		m_boat3->update(settings->hz);
        Test::Step(settings);
		m_boat->segPosition = track->getNewSegPosition(m_boat->segPosition, vec2(m_boat->rigidBody->GetPosition().x, m_boat->rigidBody->GetPosition().y));
		m_boat2->segPosition = track->getNewSegPosition(m_boat2->segPosition, vec2(m_boat2->rigidBody->GetPosition().x, m_boat2->rigidBody->GetPosition().y));
		m_boat3->segPosition = track->getNewSegPosition(m_boat3->segPosition, vec2(m_boat3->rigidBody->GetPosition().x, m_boat3->rigidBody->GetPosition().y));

		std::cout << m_boat->segPosition << "\t" << m_boat2->segPosition << "\t" << m_boat3->segPosition << "\n";
    }
    
    static Test* Create()
    {
        return new BoatTest;
    }
	/*
	static void init_world(b2World *m_world, Track* track, const int n)
	{
		m_world->SetGravity(b2Vec2(0.0f, 0.0f));
		//Create track sides
		{
			//            float innerRadius = 50.0f;
			//            float outerRadius = 100.0f;

			{
				b2BodyDef bd;
				b2Body* ground = m_world->CreateBody(&bd);

				b2Vec2 left[n];
				b2Vec2 right[n];
				for (int i = 0; i < n; i++)
				{
					left[i].Set(track->l[i].x, track->l[i].y);
					right[i].Set(track->r[i].x, track->r[i].y);
				}

				b2ChainShape innerShape;
				innerShape.CreateChain(left, n);
				ground->CreateFixture(&innerShape, 0.0f);

				b2ChainShape outerShape;
				outerShape.CreateChain(right, n);
				ground->CreateFixture(&outerShape, 0.0f);

			}
		}
		SimpleAI* ai = nullptr;
		//Create Boat
		{
			b2World& m_worldRef = *m_world;
			b2Vec2 initPos = b2Vec2(0.0f, -75.0f);
			Boat *m_boat; = new Boat(initPos, m_worldRef, ai);
		}
	}*/
};

#endif /* BoatTrack_h */
