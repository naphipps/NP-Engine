//
//  ParticleSystem.hpp
//  Project Space
//
//  Created by Nathan Phipps on 10/5/18.
//  Copyright © 2018 Nathan Phipps. All rights reserved.
//

#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp

#include <Box2D/Box2D.h>
#include <vector>
#include <iostream>

namespace np {
    
    class ParticleSystem {
        
    private:
        
        friend class Physics;
        friend class ContactListener;
        
        void* userData;
        b2ParticleSystem* particleSystem;
        
        ParticleSystem(b2ParticleSystem* particleSystem);
        ~ParticleSystem();
        
        //---------------------------------------------------------------------------
        
        //during collision action lists
        
        std::vector<void (*)(b2ParticleSystem*, b2ParticleBodyContact*)> duringFixtureParticleCollisionBeginActions;
        std::vector<void (*)(b2Fixture*, b2ParticleSystem*, int32)> duringFixtureParticleCollisionEndActions;
        
        std::vector<void (*)(b2ParticleSystem*, b2ParticleContact*)> duringParticleCollisionBeginActions;
        std::vector<void (*)(b2ParticleSystem*, int32, int32)> duringParticleCollisionEndActions;
        
        //post collision action lists
        
        std::vector<void (*)(b2Fixture*, b2ParticleSystem*, int32)> postFixtureParticleCollisionBeginActions;
        std::vector<void (*)(b2Fixture*, b2ParticleSystem*, int32)> postFixtureParticleCollisionEndActions;
        
        std::vector<void (*)(b2ParticleSystem*, int32, int32)> postParticleCollisionBeginActions;
        std::vector<void (*)(b2ParticleSystem*, int32, int32)> postParticleCollisionEndActions;
        
        //---------------------------------------------------------------------------
        
        unsigned long GetDuringFixtureParticleCollisionBeginActionCount();
        unsigned long GetDuringFixtureParticleCollisionEndActionCount();
        unsigned long GetDuringParticleCollisionBeginActionCount();
        unsigned long GetDuringParticleCollisionEndActionCount();
        
        unsigned long GetPostFixtureParticleCollisioBeginActionCount();
        unsigned long GetPostFixtureParticleCollisionEndActionCount();
        unsigned long GetPostParticleCollisionBeginActionCount();
        unsigned long GetPostParticleCollisionEndActionCount();
        
        //---------------------------------------------------------------------------
        //adapter methods of b2ContactListener methods for b2ParticleSystem objects
        
        //during collision methods
        
        void DuringBeginContact(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact);
        void DuringEndContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex);
        
        void DuringBeginContact(b2ParticleSystem *paricleSystem, b2ParticleContact *particleContact);
        void DuringEndContact(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex);
        
        //post collision methods
        
        void PostBeginContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex);
        void PostEndContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex);
        
        void PostBeginContact(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex);
        void PostEndContact(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex);
        
        //---------------------------------------------------------------------------
        
        //during collision actions
        
        void AddDuringFixtureParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact));
        void AddDuringFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        
        void AddDuringParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, b2ParticleContact *particleContact));
        void AddDuringParticleCollisionEndAction(void (*contactAction)(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex));
        
        //post collision actions
        
        void AddPostFixtureParticleCollisionBeginAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        void AddPostFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        
        void AddPostParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex));
        void AddPostParticleCollisionEndAction(void (*contactAction)(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex));
        
        //---------------------------------------------------------------------------
        
        //during collision actions
        
        void RemoveDuringFixtureParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact));
        void RemoveDuringFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        
        void RemoveDuringParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, b2ParticleContact *particleContact));
        void RemoveDuringParticleCollisionEndAction(void (*contactAction)(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex));
        
        //post collision actions
        
        void RemovePostFixtureParticleCollisionBeginAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        void RemovePostFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex));
        
        void RemovePostParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex));
        void RemovePostParticleCollisionEndAction(void (*contactAction)(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex));
        
        //---------------------------------------------------------------------------
        
    public:
        
        b2ParticleSystem* GetB2ParticleSystem();
        
        void* GetUserData();
        void SetUserData(void* data);
        
    };
    
}

#endif /* ParticleSystem_hpp */
