//
//  BoatTrack.h
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/12/17.
//
//
#include "Boat.hpp"
#include "InputStream.hpp"

#ifndef BoatTrack_h
#define BoatTrack_h


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
            // Chain shape
            {
                // Add track edges here
                b2BodyDef bd;
                bd.angle = 0.25f * b2_pi;
                b2Body* ground = m_world->CreateBody(&bd);
                
                b2Vec2 vs[8];
                for(int i = 0; i < 8; ++i)
                {
                    vs[i].Set(0.0f, i * 8.0f);
                }
                b2ChainShape shape;
                shape.CreateChain(vs, 8);
                ground->CreateFixture(&shape, 0.0f);
            }
        }
        
        //Create Boat
        {
            b2World& m_worldRef = *m_world;
            m_boat = new Boat(5.0f, m_worldRef);
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
