//
//  BoatTrack.h
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/12/17.
//
//

#ifndef BoatTrack_h
#define BoatTrack_h

class BoatTest: public Test
{
public:
    
    BoatTest()
    {
        // Read Test Spec from File
        
        
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
        
        //Create boat
        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set(0.0f, 0.0f);
            m_body = m_world->CreateBody(&bd);
            
            b2PolygonShape shape;
            b2Vec2 vertices[6];
            vertices[0].Set(-6.0f, 0.0f);
            vertices[1].Set(-4.0f, 4.0f);
            vertices[2].Set(4.0f, 4.0f);
            vertices[3].Set(6.0f, 0.0f);
            vertices[4].Set(4.0f, -4.0f);
            vertices[5].Set(-4.0f, -4.0f);
            shape.Set(vertices,6);
            
            b2FixtureDef fd;
            fd.shape = &shape;
            fd.density = 1.0f;
            m_body->SetLinearDamping(1.0f);
            m_body->SetAngularDamping(1.0f);
            m_body->CreateFixture(&fd);
        }
        
        
    }
    
    void Keyboard(int key)
    {
        switch (key)
        {
            case GLFW_KEY_W:
            {
                b2Vec2 f = m_body->GetWorldVector(b2Vec2(20000.0f, 0.0f));
                b2Vec2 p = m_body->GetWorldPoint(b2Vec2(0.0f, 0.0f));
                m_body->ApplyForce(f, p, true);
            }
                break;
                
            case GLFW_KEY_A:
            {
                m_body->ApplyTorque(50.0f, true);
            }
                break;
                
            case GLFW_KEY_D:
            {
                m_body->ApplyTorque(-50.0f, true);
            }
                break;
        }
    }
    
    static Test* Create()
    {
        return new BoatTest;
    }
    
    b2Body* m_body;
};

#endif /* BoatTrack_h */
