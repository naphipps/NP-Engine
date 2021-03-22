//
//  PhysicsContactListener.cpp
//  Project Space
//
//  Created by Nathan Phipps on 9/24/18.
//  Copyright © 2018 Nathan Phipps. All rights reserved.
//

#include "ContactListener.hpp"

namespace np {
    
    //---------------------------------------------------------------------------
    //b2ContactListener methods
    
    ///
    ///event handler for when two fixtures begin to collide
    ///
    void ContactListener::BeginContact(b2Contact *contact){
        
        b2Body* bodyA = contact->GetFixtureA()->GetBody();
        b2Body* bodyB = contact->GetFixtureB()->GetBody();
        
        //check during collision bodies
        
        if (this->duringFixtureCollisionBeginBodies.find(bodyA) != this->duringFixtureCollisionBeginBodies.end()){
            //bodyA has during collision events
            ((Body*)bodyA->GetUserData())->DuringBeginContact(contact);
        }
        
        if (this->duringFixtureCollisionBeginBodies.find(bodyB) != this->duringFixtureCollisionBeginBodies.end()){
            //bodyB has during collision events
            ((Body*)bodyB->GetUserData())->DuringBeginContact(contact);
        }
        
        //check post collision bodies
        
        if (this->postFixtureCollisionBeginBodies.find(bodyA) != this->postFixtureCollisionBeginBodies.end()){
            //bodyA has post collision events
            PostFixtureCollisionParameters postParameters;
            postParameters.body = (Body*)bodyA->GetUserData();
            postParameters.fixtureA = contact->GetFixtureA();
            postParameters.fixtureB = contact->GetFixtureB();
            this->postFixtureCollisionBeginEvents.push_back(postParameters);
        }
        
        if (this->postFixtureCollisionBeginBodies.find(bodyB) != this->postFixtureCollisionBeginBodies.end()){
            //bodyB has post collision events
            PostFixtureCollisionParameters postParameters;
            postParameters.body = (Body*)bodyB->GetUserData();
            postParameters.fixtureA = contact->GetFixtureA();
            postParameters.fixtureB = contact->GetFixtureB();
            this->postFixtureCollisionBeginEvents.push_back(postParameters);
        }
    }

    ///
    ///event handler for when to fixtures' collision ends
    ///
    void ContactListener::EndContact(b2Contact *contact){
        
        b2Body* bodyA = contact->GetFixtureA()->GetBody();
        b2Body* bodyB = contact->GetFixtureB()->GetBody();
        
        if (this->duringFixtureCollisionEndBodies.find(bodyA) != this->duringFixtureCollisionEndBodies.end()){
            //bodyA has collision events
            ((Body*)bodyA->GetUserData())->DuringEndContact(contact);
        }
        
        if (this->duringFixtureCollisionEndBodies.find(bodyB) != this->duringFixtureCollisionEndBodies.end()){
            //bodyB has collision events
            ((Body*)bodyB->GetUserData())->DuringEndContact(contact);
        }
        
        if (this->postFixtureCollisionEndBodies.find(bodyA) != this->postFixtureCollisionEndBodies.end()){
            //bodyA has post collision events
            PostFixtureCollisionParameters postParameters;
            postParameters.body = (Body*)bodyA->GetUserData();
            postParameters.fixtureA = contact->GetFixtureA();
            postParameters.fixtureB = contact->GetFixtureB();
            this->postFixtureCollisionEndEvents.push_back(postParameters);
        }
        
        if (this->postFixtureCollisionEndBodies.find(bodyB) != this->postFixtureCollisionEndBodies.end()){
            //bodyB has post collision events
            PostFixtureCollisionParameters postParameters;
            postParameters.body = (Body*)bodyB->GetUserData();
            postParameters.fixtureA = contact->GetFixtureA();
            postParameters.fixtureB = contact->GetFixtureB();
            this->postFixtureCollisionEndEvents.push_back(postParameters);
        }
        
    }

    ///
    ///event handler for when a fixture collides with a particle
    ///
    void ContactListener::BeginContact(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact){
        
        //body
        
        b2Body* b2body = particleBodyContact->body;
        
        if (this->duringFixtureParticleCollisionBeginBodies.find(b2body) !=
            this->duringFixtureParticleCollisionBeginBodies.end()){
            //b2body has collision events
            ((Body*)b2body->GetUserData())->DuringBeginContact(particleSystem, particleBodyContact);
        }
        
        if (this->postFixtureParticleCollisionBeginBodies.find(b2body) !=
            this->postFixtureParticleCollisionBeginBodies.end()){
            //b2body has post collision events
            PostFixtureParticleCollisionParameters postParameters;
            postParameters.body = (Body*)b2body->GetUserData();
            postParameters.fixture = particleBodyContact->fixture;
            postParameters.particleSystem = (ParticleSystem*)particleSystem->GetUserData();
            postParameters.particleIndex = particleBodyContact->index;
            this->postFixtureParticleCollisionBeginEvents.push_back(postParameters);
        }
        
        //particle system
        
        if (this->duringFixtureParticleCollisionBeginSystems.find(particleSystem) !=
            this->duringFixtureParticleCollisionBeginSystems.end()){
            //particle system has collision events
            ((ParticleSystem*)particleSystem->GetUserData())->DuringBeginContact(particleSystem, particleBodyContact);
        }
        
        if (this->postFixtureParticleCollisionBeginSystems.find(particleSystem) !=
            this->postFixtureParticleCollisionBeginSystems.end()){
            //particle system has post collision events
            PostFixtureParticleCollisionParameters postParameters;
            postParameters.body = (Body*)b2body->GetUserData();
            postParameters.fixture = particleBodyContact->fixture;
            postParameters.particleSystem = (ParticleSystem*)particleSystem->GetUserData();
            postParameters.particleIndex = particleBodyContact->index;
            this->postFixtureParticleCollisionBeginEvents.push_back(postParameters);
        }
        
    }

    ///
    ///event handler for when a fixture and particle collision ends
    ///
    void ContactListener::EndContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex){
        
        //body
        
        b2Body* b2body = fixture->GetBody();
        
        if (this->duringFixtureParticleCollisionEndBodies.find(b2body) !=
            this->duringFixtureParticleCollisionEndBodies.end()){
            //b2body has collision events
            ((Body*)b2body->GetUserData())->DuringEndContact(fixture, particleSystem, particleIndex);
        }
        
        if (this->postFixtureParticleCollisionEndBodies.find(b2body) !=
            this->postFixtureParticleCollisionEndBodies.end()){
            //b2body has post collision events
            PostFixtureParticleCollisionParameters postParameters;
            postParameters.body = (Body*)b2body->GetUserData();
            postParameters.fixture = fixture;
            postParameters.particleSystem = (ParticleSystem*)particleSystem->GetUserData();
            postParameters.particleIndex = particleIndex;
            this->postFixtureParticleCollisionEndEvents.push_back(postParameters);
        }
        
        //particle system
        
        if (this->duringFixtureParticleCollisionEndSystems.find(particleSystem) !=
            this->duringFixtureParticleCollisionEndSystems.end()){
            //particle system has collision events
            ((ParticleSystem*)particleSystem->GetUserData())->DuringEndContact(fixture, particleSystem, particleIndex);
        }
        
        if (this->postFixtureParticleCollisionEndSystems.find(particleSystem) !=
            this->postFixtureParticleCollisionEndSystems.end()){
            //particle system has post collision events
            PostFixtureParticleCollisionParameters postParameters;
            postParameters.body = (Body*)b2body->GetUserData();
            postParameters.fixture = fixture;
            postParameters.particleSystem = (ParticleSystem*)particleSystem->GetUserData();
            postParameters.particleIndex = particleIndex;
            this->postFixtureParticleCollisionEndEvents.push_back(postParameters);
        }
        
    }

    ///
    ///event handler for when two particles collide
    ///
    void ContactListener::BeginContact(b2ParticleSystem *particleSystem, b2ParticleContact *particleContact){
        
        if (this->duringParticleCollisionBeginSystems.find(particleSystem) !=
            this->duringParticleCollisionBeginSystems.end()){
            //particle system has collision events
            ((ParticleSystem*)particleSystem->GetUserData())->DuringBeginContact(particleSystem, particleContact);
        }
        
        if (this->postParticleCollisionBeginSystems.find(particleSystem) !=
            this->postParticleCollisionBeginSystems.end()){
            //particle system has post collision events
            PostParticleCollisionParameters postParameters;
            postParameters.particleSystem = (ParticleSystem*)particleSystem->GetUserData();
            postParameters.firstParticleIndex = particleContact->GetIndexA();
            postParameters.secondParticleIndex = particleContact->GetIndexB();
            this->postParticleCollisionBeginEvents.push_back(postParameters);
        }
        
    }

    ///
    ///event handler for when two particles' collision ends
    ///
    void ContactListener::EndContact(b2ParticleSystem *particleSystem, int32 firstParticleIndex, int32 secondParticleIndex){
        
        if (this->duringParticleCollisionEndSystems.find(particleSystem) !=
            this->duringParticleCollisionEndSystems.end()){
            //particle system has collision events
            ((ParticleSystem*)particleSystem->GetUserData())->DuringEndContact(particleSystem, firstParticleIndex, secondParticleIndex);
        }
        
        if (this->postParticleCollisionEndSystems.find(particleSystem) !=
            this->postParticleCollisionEndSystems.end()){
            //particle system has post collision events
            PostParticleCollisionParameters postParameters;
            postParameters.particleSystem = (ParticleSystem*)particleSystem->GetUserData();
            postParameters.firstParticleIndex = firstParticleIndex;
            postParameters.secondParticleIndex = secondParticleIndex;
            this->postParticleCollisionEndEvents.push_back(postParameters);
        }
        
    }
    
    ///
    ///event handler for before two fixtures start to collide
    ///
    void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold){
        //loop through list of contact actions
    }
    
    ///
    ///event handler for after two fixtures collided
    ///
    void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* contactImpulse){
        //loop through list of contact actions
    }

    //---------------------------------------------------------------------------
    //subscription methods for during events
    
    void ContactListener::SubscribeForDuringFixtureCollisionBeginEvents(b2Body* body){
        this->duringFixtureCollisionBeginBodies.insert(body);
    }

    void ContactListener::SubscribeForDuringFixtureCollisionEndEvents(b2Body* body){
        this->duringFixtureCollisionEndBodies.insert(body);
    }
    
    void ContactListener::SubscribeForDuringFixtureParticleCollisionBeginEvents(b2Body* body){
        this->duringFixtureParticleCollisionBeginBodies.insert(body);
    }
    
    void ContactListener::SubscribeForDuringFixtureParticleCollisionEndEvents(b2Body* body){
        this->duringFixtureParticleCollisionEndBodies.insert(body);
    }
    
    void ContactListener::SubscribeForDuringFixtureParticleCollisionBeginEvents(b2ParticleSystem* particleSystem){
        this->duringFixtureParticleCollisionBeginSystems.insert(particleSystem);
    }
    
    void ContactListener::SubscribeForDuringFixtureParticleCollisionEndEvents(b2ParticleSystem* particleSystem){
        this->duringFixtureParticleCollisionEndSystems.insert(particleSystem);
    }
    
    void ContactListener::SubscribeForDuringParticleCollisionBeginEvents(b2ParticleSystem* particleSystem){
        this->duringParticleCollisionBeginSystems.insert(particleSystem);
    }
    
    void ContactListener::SubscribeForDuringParticleCollisionEndEvents(b2ParticleSystem* particleSystem){
        this->duringParticleCollisionEndSystems.insert(particleSystem);
    }
    
    //unsubscription methods for during events
    
    void ContactListener::UnsubscribeForDuringFixtureCollisionBeginEvents(b2Body* body){
        this->duringFixtureCollisionBeginBodies.erase(body);
    }

    void ContactListener::UnsubscribeForDuringFixtureCollisionEndEvents(b2Body* body){
        this->duringFixtureCollisionEndBodies.erase(body);
    }
    
    void ContactListener::UnsubscribeForDuringFixtureParticleCollisionBeginEvents(b2Body* body){
        this->duringFixtureParticleCollisionBeginBodies.erase(body);
    }

    void ContactListener::UnsubscribeForDuringFixtureParticleCollisionEndEvents(b2Body* body){
        this->duringFixtureParticleCollisionEndBodies.erase(body);
    }
    
    void ContactListener::UnsubscribeForDuringFixtureParticleCollisionBeginEvents(b2ParticleSystem* particleSystem){
        this->duringFixtureParticleCollisionBeginSystems.erase(particleSystem);
    }
    
    void ContactListener::UnsubscribeForDuringFixtureParticleCollisionEndEvents(b2ParticleSystem* particleSystem){
        this->duringFixtureParticleCollisionEndSystems.erase(particleSystem);
    }
    
    void ContactListener::UnsubscribeForDuringParticleCollisionBeginEvents(b2ParticleSystem* particleSystem){
        this->duringParticleCollisionBeginSystems.erase(particleSystem);
    }
    
    void ContactListener::UnsubscribeForDuringParticleCollisionEndEvents(b2ParticleSystem* particleSystem){
        this->duringParticleCollisionEndSystems.erase(particleSystem);
    }

    //---------------------------------------------------------------------------
    //subscription methods for post events
    
    void ContactListener::SubscribeForPostFixtureCollisionBeginEvents(b2Body *body){
        this->postFixtureCollisionBeginBodies.insert(body);
    }
    
    void ContactListener::SubscribeForPostFixtureCollisionEndEvents(b2Body* body){
        this->postFixtureCollisionEndBodies.insert(body);
    }
    
    void ContactListener::SubscribeForPostFixtureParticleCollisionBeginEvents(b2Body* body){
        this->postFixtureParticleCollisionBeginBodies.insert(body);
    }
    
    void ContactListener::SubscribeForPostFixtureParticleCollisionEndEvents(b2Body* body){
        this->postFixtureParticleCollisionEndBodies.insert(body);
    }
    
    void ContactListener::SubscribeForPostFixtureParticleCollisionBeginEvents(b2ParticleSystem* particleSystem){
        this->postFixtureParticleCollisionBeginSystems.insert(particleSystem);
    }
    
    void ContactListener::SubscribeForPostFixtureParticleCollisionEndEvents(b2ParticleSystem* particleSystem){
        this->postFixtureParticleCollisionEndSystems.insert(particleSystem);
    }
    
    void ContactListener::SubscribeForPostParticleCollisionBeginEvents(b2ParticleSystem* particleSystem){
        this->postParticleCollisionBeginSystems.insert(particleSystem);
    }
    
    void ContactListener::SubscribeForPostParticleCollisionEndEvents(b2ParticleSystem* particleSystem){
        this->postParticleCollisionEndSystems.insert(particleSystem);
    }
    
    //unsubscription methods for post events
    
    void ContactListener::UnsubscribeForPostFixtureCollisionBeginEvents(b2Body* body){
        this->postFixtureCollisionBeginBodies.erase(body);
    }
    
    void ContactListener::UnsubscribeForPostFixtureCollisionEndEvents(b2Body* body){
        this->postFixtureCollisionEndBodies.erase(body);
    }
    
    void ContactListener::UnsubscribeForPostFixtureParticleCollisionBeginEvents(b2Body* body){
        this->postFixtureParticleCollisionBeginBodies.erase(body);
    }
    
    void ContactListener::UnsubscribeForPostFixtureParticleCollisionEndEvents(b2Body* body){
        this->postFixtureParticleCollisionEndBodies.erase(body);
    }
    
    void ContactListener::UnsubscribeForPostFixtureParticleCollisionBeginEvents(b2ParticleSystem* particleSystem){
        this->postFixtureParticleCollisionBeginSystems.erase(particleSystem);
    }
    
    void ContactListener::UnsubscribeForPostFixtureParticleCollisionEndEvents(b2ParticleSystem* particleSystem){
        this->postFixtureParticleCollisionEndSystems.erase(particleSystem);
    }
    
    void ContactListener::UnsubscribeForPostParticleCollisionBeginEvents(b2ParticleSystem* particleSystem){
        this->postParticleCollisionBeginSystems.erase(particleSystem);
    }
    
    void ContactListener::UnsubscribeForPostParticleCollisionEndEvents(b2ParticleSystem* particleSystem){
        this->postParticleCollisionEndSystems.erase(particleSystem);
    }
    
    //---------------------------------------------------------------------------
    //private methods for friend classes
    
    ///
    ///Triggers all the post step collision events
    ///
    void ContactListener::TriggerPostStepCollisionEvents(){
        
        //trigger post fixture events
        for (std::vector<PostFixtureCollisionParameters>::iterator iterator =
             this->postFixtureCollisionBeginEvents.begin();
             iterator != this->postFixtureCollisionBeginEvents.end(); iterator++){
            if (this->postFixtureCollisionBeginBodies.find(iterator->body->GetB2Body()) !=
                this->postFixtureCollisionBeginBodies.end()){
                iterator->body->PostBeginContact(iterator->fixtureA, iterator->fixtureB);
            }
        }
        
        for (std::vector<PostFixtureCollisionParameters>::iterator iterator =
             this->postFixtureCollisionEndEvents.begin();
             iterator != this->postFixtureCollisionEndEvents.end(); iterator++){
            if (this->postFixtureCollisionEndBodies.find(iterator->body->GetB2Body()) !=
                this->postFixtureCollisionEndBodies.end()){
                iterator->body->PostEndContact(iterator->fixtureA, iterator->fixtureB);
            }
        }
        
        //trigger post fixture particle events
        for (std::vector<PostFixtureParticleCollisionParameters>::iterator iterator =
             this->postFixtureParticleCollisionBeginEvents.begin();
             iterator != this->postFixtureParticleCollisionBeginEvents.end(); iterator++){
            if (this->postFixtureParticleCollisionBeginBodies.find(iterator->body->GetB2Body()) !=
                this->postFixtureParticleCollisionBeginBodies.end()){
                b2ParticleSystem* particleSystem = iterator->particleSystem->GetB2ParticleSystem();
                iterator->body->PostBeginContact(iterator->fixture, particleSystem, iterator->particleIndex);
            }
        }
        
        for (std::vector<PostFixtureParticleCollisionParameters>::iterator iterator =
             this->postFixtureParticleCollisionEndEvents.begin();
             iterator != this->postFixtureParticleCollisionEndEvents.end(); iterator++){
            if (this->postFixtureParticleCollisionEndBodies.find(iterator->body->GetB2Body()) !=
                this->postFixtureParticleCollisionEndBodies.end()){
                b2ParticleSystem* particleSystem = iterator->particleSystem->GetB2ParticleSystem();
                iterator->body->PostEndContact(iterator->fixture, particleSystem, iterator->particleIndex);
            }
        }
        
        for (std::vector<PostFixtureParticleCollisionParameters>::iterator iterator =
             this->postFixtureParticleCollisionBeginEvents.begin();
             iterator != this->postFixtureParticleCollisionBeginEvents.end(); iterator++){
            if (this->postFixtureParticleCollisionBeginSystems.find(iterator->particleSystem->GetB2ParticleSystem()) !=
                this->postFixtureParticleCollisionBeginSystems.end()){
                b2ParticleSystem* particleSystem = iterator->particleSystem->GetB2ParticleSystem();
                iterator->particleSystem->PostBeginContact(iterator->fixture, particleSystem, iterator->particleIndex);
            }
        }
        
        for (std::vector<PostFixtureParticleCollisionParameters>::iterator iterator =
             this->postFixtureParticleCollisionEndEvents.begin();
             iterator != this->postFixtureParticleCollisionEndEvents.end(); iterator++){
            if (this->postFixtureParticleCollisionEndSystems.find(iterator->particleSystem->GetB2ParticleSystem()) !=
                this->postFixtureParticleCollisionEndSystems.end()){
                b2ParticleSystem* particleSystem = iterator->particleSystem->GetB2ParticleSystem();
                iterator->particleSystem->PostEndContact(iterator->fixture, particleSystem, iterator->particleIndex);
            }
        }
        
        //trigger post particle events
        for (std::vector<PostParticleCollisionParameters>::iterator iterator =
             this->postParticleCollisionBeginEvents.begin();
             iterator != this->postParticleCollisionBeginEvents.begin(); iterator++){
            if (this->postParticleCollisionBeginSystems.find(iterator->particleSystem->GetB2ParticleSystem()) !=
                this->postParticleCollisionBeginSystems.end()){
                b2ParticleSystem* particleSystem = iterator->particleSystem->GetB2ParticleSystem();
                iterator->particleSystem->PostBeginContact(particleSystem, iterator->firstParticleIndex,
                                                           iterator->secondParticleIndex);
            }
        }
        
        for (std::vector<PostParticleCollisionParameters>::iterator iterator =
             this->postParticleCollisionEndEvents.begin();
             iterator != this->postParticleCollisionEndEvents.begin(); iterator++){
            if (this->postParticleCollisionEndSystems.find(iterator->particleSystem->GetB2ParticleSystem()) !=
                this->postParticleCollisionEndSystems.end()){
                b2ParticleSystem* particleSystem = iterator->particleSystem->GetB2ParticleSystem();
                iterator->particleSystem->PostEndContact(particleSystem, iterator->firstParticleIndex,
                                                           iterator->secondParticleIndex);
            }
        }
        
    }
    
    ///
    ///Clears the post collision event vectors
    ///
    void ContactListener::ClearPostCollisionEvents(){
        this->postFixtureCollisionBeginEvents.clear();
        this->postFixtureCollisionEndEvents.clear();
        this->postFixtureParticleCollisionBeginEvents.clear();
        this->postFixtureParticleCollisionEndEvents.clear();
        this->postParticleCollisionBeginEvents.clear();
        this->postParticleCollisionEndEvents.clear();
    }
    
}
