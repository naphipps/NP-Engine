//
//  Physics.cpp
//  Project Space
//
//  Created by Nathan Phipps on 9/20/18.
//  Copyright © 2018 Nathan Phipps. All rights reserved.
//

#include "Physics.hpp"

namespace np {

    Physics &Physics::GetInstance(){
        static Physics instance;
        return instance;
    }

    Physics::Physics(){
        
        //setup world
        b2Vec2 defaultCardinalGravity(0, -9.81);
        this->world = new b2World(defaultCardinalGravity);
        
        this->SetFrameRate(this->frameRate);
        
        this->contactListener = new ContactListener;
        this->world->SetContactListener(this->contactListener);
        
    }

    Physics::~Physics(){
        //handle pointers
    }

    //---------------------------------------------------------------------------

    void Physics::SetDegubDrawer(b2Draw *drawer){
        this->drawer = drawer;
        world->SetDebugDraw(this->drawer);
    }

    void Physics::DrawDebugData(){
        if (this->drawer){
            world->DrawDebugData();
        }
    }

    void Physics::SetFrameRate(unsigned int rate){
        this->frameRate = rate;
        this->timeStep = (float32)1 / (float32)this->frameRate;
    }

    void Physics::Step(){
        if (this->enableStep){
            world->Step(timeStep, velocityIterations, positionIterations, particleIterations);
            
            this->contactListener->TriggerPostStepCollisionEvents();
            this->contactListener->ClearPostCollisionEvents();
        }
    }

    void Physics::SetEnableStep(bool enable){
        this->enableStep = enable;
    }

    Body *Physics::CreateBody(BodyDef* physicsBodyDef){
        
        b2Body* body = NULL;
        std::vector<b2FixtureDef*> fixtureDefs = physicsBodyDef->GetFixtureDefs();
        
        if (fixtureDefs.size() > 0){
            
            b2BodyDef *bodyDef = physicsBodyDef->GetB2BodyDef();
            body = world->CreateBody(bodyDef);
            
            for (int i=0; i<fixtureDefs.size(); i++){
                body->CreateFixture(fixtureDefs[i]);
            }
        }
        
        Body *physicsBody;
        physicsBody = new Body(body);
        physicsBody->SetUserData(body->GetUserData());
        body->SetUserData(physicsBody);
        
        return physicsBody;
    }

    Body *Physics::CreateBody(BodyDef *physicsBodyDef, b2Shape *shape){
        physicsBodyDef->AddFixtureDef(shape);
        return CreateBody(physicsBodyDef);
    }

    Body *Physics::CreateBody(BodyDef *physicsBodyDef, b2Shape *shape, float32 density){
        physicsBodyDef->AddFixtureDef(shape, density);
        return CreateBody(physicsBodyDef);
    }

    void Physics::SetCardinalGravity(b2Vec2 *gravity){
        this->world->SetGravity(*gravity);
    }

    ParticleSystem* Physics::CreateParticleSystem(b2ParticleSystemDef *particleSystemDef){
        
        b2ParticleSystem* b2particleSystem;
        b2particleSystem = this->world->CreateParticleSystem(particleSystemDef);
        ParticleSystem* particleSystem;
        particleSystem = new ParticleSystem(b2particleSystem);
        b2particleSystem->SetUserData(particleSystem);
        
        return particleSystem;
    }

    //---------------------------------------------------------------------------

    //during collision subscriptions
    
    void Physics::SubscribeForDuringFixtureCollisionBeginEvents(Body* body, void (*contactAction)(b2Contact*)){
        body->AddDuringFixtureCollisionBeginAction(contactAction);
        this->contactListener->SubscribeForDuringFixtureCollisionBeginEvents(body->GetB2Body());
    }
    
    void Physics::SubscribeForDuringFixtureCollisionEndEvents(Body* body, void (*contactAction)(b2Contact*)){
        body->AddDuringFixtureCollisionEndAction(contactAction);
        this->contactListener->SubscribeForDuringFixtureCollisionEndEvents(body->GetB2Body());
    }
    
    void Physics::SubscribeForDuringFixtureParticleCollisionBeginEvents(Body* body, void (*contactAction)(b2ParticleSystem*, b2ParticleBodyContact*)){
        body->AddDuringFixtureParticleCollisionBeginAction(contactAction);
        this->contactListener->SubscribeForDuringFixtureParticleCollisionBeginEvents(body->GetB2Body());
    }
    
    void Physics::SubscribeForDuringFixtureParticleCollisionEndEvents(Body* body, void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32)){
        body->AddDuringFixtureParticleCollisionEndAction(contactAction);
        this->contactListener->SubscribeForDuringFixtureParticleCollisionEndEvents(body->GetB2Body());
    }
    
    void Physics::SubscribeForDuringFixtureParticleCollisionBeginEvents(ParticleSystem* particleSystem, void (*contactAction)(b2ParticleSystem*, b2ParticleBodyContact*)){
        particleSystem->AddDuringFixtureParticleCollisionBeginAction(contactAction);
        this->contactListener->SubscribeForDuringFixtureParticleCollisionBeginEvents(particleSystem->GetB2ParticleSystem());
    }
    
    void Physics::SubscribeForDuringFixtureParticleCollisionEndEvents(ParticleSystem* particleSystem, void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32)){
        particleSystem->AddDuringFixtureParticleCollisionEndAction(contactAction);
        this->contactListener->SubscribeForDuringFixtureParticleCollisionEndEvents(particleSystem->GetB2ParticleSystem());
    }
    
    void Physics::SubscribeForDuringParticleCollisionBeginEvents(ParticleSystem* particleSystem, void (*contactAction)(b2ParticleSystem*, b2ParticleContact*)){
        particleSystem->AddDuringParticleCollisionBeginAction(contactAction);
        this->contactListener->SubscribeForDuringParticleCollisionBeginEvents(particleSystem->GetB2ParticleSystem());
    }
    
    void Physics::SubscribeForDuringParticleCollisionEndEvents(ParticleSystem* particleSystem, void (*contactAction)(b2ParticleSystem*, int32, int32)){
        particleSystem->AddDuringParticleCollisionEndAction(contactAction);
        this->contactListener->SubscribeForDuringParticleCollisionEndEvents(particleSystem->GetB2ParticleSystem());
    }
    
    //post collision subscriptions
    
    void Physics::SubscribeForPostFixtureCollisionBeginEvents(Body* body, void (*contactAction)(b2Fixture*, b2Fixture*)){
        body->AddPostFixtureCollisionBeginAction(contactAction);
        this->contactListener->SubscribeForPostFixtureCollisionBeginEvents(body->GetB2Body());
    }
    
    void Physics::SubscribeForPostFixtureCollisionEndEvents(Body* body, void (*contactAction)(b2Fixture*, b2Fixture*)){
        body->AddPostFixtureCollisionEndAction(contactAction);
        this->contactListener->SubscribeForPostFixtureCollisionEndEvents(body->GetB2Body());
    }
    
    void Physics::SubscribeForPostFixtureParticleCollisionBeginEvents(Body* body, void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32)){
        body->AddPostFixtureParticleCollisionBeginAction(contactAction);
        this->contactListener->SubscribeForPostFixtureParticleCollisionBeginEvents(body->GetB2Body());
    }
    
    void Physics::SubscribeForPostFixtureParticleCollisionEndEvents(Body* body, void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32)){
        body->AddPostFixtureParticleCollisionEndAction(contactAction);
        this->contactListener->SubscribeForPostFixtureParticleCollisionEndEvents(body->GetB2Body());
    }
    
    void Physics::SubscribeForPostFixtureParticleCollisionBeginEvents(ParticleSystem* particleSystem, void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32)){
        particleSystem->AddPostFixtureParticleCollisionBeginAction(contactAction);
        this->contactListener->SubscribeForPostFixtureParticleCollisionBeginEvents(particleSystem->GetB2ParticleSystem());
    }
    
    void Physics::SubscribeForPostFixtureParticleCollisionEndEvents(ParticleSystem* particleSystem, void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32)){
        particleSystem->AddPostFixtureParticleCollisionEndAction(contactAction);
        this->contactListener->SubscribeForPostFixtureParticleCollisionEndEvents(particleSystem->GetB2ParticleSystem());
    }
    
    void Physics::SubscribeForPostParticleCollisionBeginEvents(ParticleSystem* particleSystem, void (*contactAction)(b2ParticleSystem*, int32, int32)){
        particleSystem->AddPostParticleCollisionBeginAction(contactAction);
        this->contactListener->SubscribeForPostParticleCollisionBeginEvents(particleSystem->GetB2ParticleSystem());
    }
    
    void Physics::SubscribeForPostParticleCollisionEndEvents(ParticleSystem* particleSystem, void (*contactAction)(b2ParticleSystem*, int32, int32)){
        particleSystem->AddPostParticleCollisionEndAction(contactAction);
        this->contactListener->SubscribeForPostParticleCollisionEndEvents(particleSystem->GetB2ParticleSystem());
    }

    //---------------------------------------------------------------------------
    
    //during collision unsubscriptions
    
    void Physics::UnsubscribeForDuringFixtureCollisionBeginEvents(Body* body){
        this->contactListener->UnsubscribeForDuringFixtureCollisionBeginEvents(body->GetB2Body());
        body->duringFixtureCollisionBeginActions.clear();
    }

    void Physics::UnsubscribeForDuringFixtureCollisionBeginEvents(Body* body, void(*contactAction)(b2Contact*)){
        ((Body*)body->GetUserData())->RemoveDuringFixtureCollisionBeginAction(contactAction);
        
        if (((Body*)body->GetUserData())->GetDuringFixtureCollisionBeginActionCount() == 0){
            this->UnsubscribeForDuringFixtureCollisionBeginEvents(body);
        }
    }
    
    //post collision unsubscriptions
    
    

}
