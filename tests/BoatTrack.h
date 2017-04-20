//
//  BoatTrack.h
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/12/17.
//
//
#include "Boat.hpp"
#include "InputStream.hpp"
#include <math.h>

#ifndef BoatTrack_h
#define BoatTrack_h
#define CIRCLE_SEGMENTS 30

class BoatTest: public Test
{
public:
    Boat *m_boat;
    
    BoatTest()
    {
        //Zero-G
        m_world->SetGravity(b2Vec2(0.0f, 0.0f));
        
        //Create track sides
        {
            float innerRadius = 50.0f;
            float outerRadius = 100.0f;
            
            {
                b2BodyDef bd;
                b2Body* ground = m_world->CreateBody(&bd);
                
                // One more to close the loop
                b2Vec2 inner[CIRCLE_SEGMENTS+1];
                b2Vec2 outer[CIRCLE_SEGMENTS+1];
                
                for(int i = 0; i < CIRCLE_SEGMENTS+1; ++i)
                {
                    float theta = i * 2 * b2_pi /CIRCLE_SEGMENTS;
                    inner[i].Set(innerRadius * cos(theta),
                                 innerRadius * sin(theta));
                    outer[i].Set(outerRadius * cos(theta),
                                 outerRadius * sin(theta));
                }
                                 
                b2ChainShape innerShape;
                innerShape.CreateChain(inner, CIRCLE_SEGMENTS+1);
                ground->CreateFixture(&innerShape, 0.0f);
                
                b2ChainShape outerShape;
                outerShape.CreateChain(outer, CIRCLE_SEGMENTS+1);
                ground->CreateFixture(&outerShape, 0.0f);
            }
        }
        
        //Create Boat
        {
            b2World& m_worldRef = *m_world;
            b2Vec2 initPos = b2Vec2(0.0f,-75.0f);
            m_boat = new Boat(initPos, m_worldRef);
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
        
        Test::Step(settings);
    }
    
    static Test* Create()
    {
        return new BoatTest;
    }
};

#endif /* BoatTrack_h */
