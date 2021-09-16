//
//  PhysicsBody.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/21/18.
//  Copyright © 2018 Nathan Phipps. All rights reserved.
//

#ifndef PhysicsBody_h
#define PhysicsBody_h

#include <algorithm>
#include <vector>
#include <iostream>
#include <Box2D/Box2D.h>
#include <typeinfo>
#include "Constants.h"

namespace np {

    //---------------------------------------------------------------------------
    //BodyDef
    //---------------------------------------------------------------------------
    
    class BodyDef {
      
    private:
        
        std::vector<b2FixtureDef*> fixtureDefs;
        
    public:
        
        BodyDef();
        ~BodyDef();
        
        //---------------------------------------------------------------------------
        
        b2BodyType type;
        b2Vec2 position;
        float32 angle;
        b2Vec2 linearVelocity;
        float32 angularVelocity;
        float32 linearDamping;
        float32 angularDamping;
        bool allowSleep;
        bool awake;
        bool fixedRotation;
        bool bullet;
        bool active;
        void* bodyUserData;
        float32 gravityScale;
        
        //---------------------------------------------------------------------------
        
        b2BodyDef *GetB2BodyDef();
        void AddFixtureDef(b2FixtureDef *fixtureDef);
        void AddFixtureDef(b2Shape *shape, float32 density);
        void AddFixtureDef(b2Shape *shape);
        b2FixtureDef *GetLastFixtureDef();
        b2FixtureDef *GetFixtureDef(int i);
        std::vector<b2FixtureDef*> &GetFixtureDefs();
        
        //TODO-NP: figure out what to do about this anchor idea
        b2Vec2 anchor;
        
    };

    //---------------------------------------------------------------------------
    //Body
    //---------------------------------------------------------------------------

    class Body{
        
    private:

        friend class Physics;
        friend class ContactListener;
        
        Body(b2Body *body);
        ~Body();
        
        b2Body *body;
        
        //---------------------------------------------------------------------------
        
        //during collision action lists
        
        std::vector<void (*)(b2Contact*)> duringFixtureCollisionBeginActions;
        std::vector<void (*)(b2Contact*)> duringFixtureCollisionEndActions;
        
        std::vector<void (*)(b2ParticleSystem*, b2ParticleBodyContact*)> duringFixtureParticleCollisionBeginActions;
        std::vector<void (*)(b2Fixture*, b2ParticleSystem*, int32)> duringFixtureParticleCollisionEndActions;
        
        //post collision action lists
        
        std::vector<void (*)(b2Fixture*, b2Fixture*)> postFixtureCollisionBeginActions;
        std::vector<void (*)(b2Fixture*, b2Fixture*)> postFixtureCollisionEndActions;
        
        std::vector<void (*)(b2Fixture*, b2ParticleSystem*, int32)> postFixtureParticleCollisionBeginActions;
        std::vector<void (*)(b2Fixture*, b2ParticleSystem*, int32)> postFixtureParticleCollisionEndActions;
        
        //---------------------------------------------------------------------------
        
        unsigned long GetDuringFixtureCollisionBeginActionCount();
        unsigned long GetDuringFixtureCollisionEndActionCount();
        unsigned long GetDuringFixtureParticleCollisionBeginActionCount();
        unsigned long GetDuringFixtureParticleCollisionEndActionCount();
        
        unsigned long GetPostFixtureCollisionBeginActionCount();
        unsigned long GetPostFixtureCollisionEndActionCount();
        unsigned long GetPostFixtureParticleCollisioBeginActionCount();
        unsigned long GetPostFixtureParticleCollisionEndActionCount();
        
        //---------------------------------------------------------------------------
        //adapter methods of b2ContactListener methods for b2Body objects
        
        //during collision methods
        
        void DuringBeginContact(b2Contact *contact);
        void DuringEndContact(b2Contact *contact);

        void DuringBeginContact(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact);
        void DuringEndContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex);

        //post collision methods
        
        void PostBeginContact(b2Fixture *fixtureA, b2Fixture *fixtureB);
        void PostEndContact(b2Fixture *fixtureA, b2Fixture *fixtureB);
        
        void PostBeginContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex);
        void PostEndContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex);
        
        //---------------------------------------------------------------------------
        
        //during collision actions
        
        void AddDuringFixtureCollisionBeginAction(void (*contactAction)(b2Contact *contact));
        void AddDuringFixtureCollisionEndAction(void (*contactAction)(b2Contact *contact));
        
        void AddDuringFixtureParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact));
        void AddDuringFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        
        //post collision actions
        
        void AddPostFixtureCollisionBeginAction(void (*contactAction)(b2Fixture *fixtureA, b2Fixture *fixtureB));
        void AddPostFixtureCollisionEndAction(void (*contactAction)(b2Fixture *fixtureA, b2Fixture *fixtureB));
        
        void AddPostFixtureParticleCollisionBeginAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        void AddPostFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        
        //---------------------------------------------------------------------------
        
        //during collision actions
        
        void RemoveDuringFixtureCollisionBeginAction(void (*contactAction)(b2Contact *contact));
        void RemoveDuringFixtureCollisionEndAction(void (*contactAction)(b2Contact *contact));
        
        void RemoveDuringFixtureParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact));
        void RemoveDuringFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        
        //post collision actions
        
        void RemovePostFixtureCollisionBeginAction(void (*contactAction)(b2Fixture *fixtureA, b2Fixture *fixtureB));
        void RemovePostFixtureCollisionEndAction(void (*contactAction)(b2Fixture *fixtureA, b2Fixture *fixtureB));
        
        void RemovePostFixtureParticleCollisionBeginAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        void RemovePostFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        
        //---------------------------------------------------------------------------
        
        void *userData;
        
    public:
        
        b2Body *GetB2Body();
        
        void SetUserData(void *data);
        void* GetUserData();
        
    };

}

#endif /* PhysicsBody_h */
