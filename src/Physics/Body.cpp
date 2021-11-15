//
//  PhysicsBody.cpp
//  Project Space
//
//  Created by Nathan Phipps on 9/21/18.
//  Copyright © 2018 Nathan Phipps. All rights reserved.
//

#include "NP-Engine/Physics/Body.hpp"

namespace np {

    //---------------------------------------------------------------------------
    //BodyDef
    //---------------------------------------------------------------------------

    ///
    ///PhysicsBodyDef constructor
    ///
    BodyDef::BodyDef(){
        
        bodyUserData = nullptr;
        position.Set(0.0f, 0.0f);
        angle = 0.0f;
        linearVelocity.Set(0.0f, 0.0f);
        angularVelocity = 0.0f;
        linearDamping = 0.0f;
        angularDamping = 0.0f;
        allowSleep = true;
        awake = true;
        fixedRotation = false;
        bullet = false;
        type = b2_staticBody;
        active = true;
        gravityScale = 1.0f;
        
    }

    ///
    ///PhysicsBodyDef deconstructor
    ///
    BodyDef::~BodyDef(){
        //handle pointers?
    }

    ///
    ///PhysicsBodyDef GetBodyDef method
    /// This returns the b2BodyDef pointer of this PhysicsBodyDef
    ///
    b2BodyDef* BodyDef::GetB2BodyDef(){
        b2BodyDef *bodyDef;
        bodyDef = new b2BodyDef();
        bodyDef->active = this->active;
        bodyDef->allowSleep = this->allowSleep;
        bodyDef->angle = this->angle;
        bodyDef->angularDamping = this->angularDamping;
        bodyDef->angularVelocity = this->angularVelocity;
        bodyDef->awake = this->awake;
        bodyDef->bullet = this->bullet;
        bodyDef->fixedRotation = this->fixedRotation;
        bodyDef->gravityScale = this->gravityScale;
        bodyDef->linearDamping = this->linearDamping;
        bodyDef->linearVelocity = this->linearVelocity; //not sure
        //make usage of anchor here for position
        bodyDef->position.Set(this->position.x,
                              this->position.y);
        bodyDef->type = this->type;
        bodyDef->userData = this->bodyUserData;
        return bodyDef;
    }

    ///
    ///PhysicsBodyDef GetFixtureDef method
    /// This returns the b2FixtureDef pointer of the given fixture index of this PhysicsBodyDef
    ///
    b2FixtureDef* BodyDef::GetFixtureDef(int i){
        if (this->fixtureDefs.size() < i){
            return this->fixtureDefs[i];
        }
        return nullptr;
    }

    b2FixtureDef* BodyDef::GetLastFixtureDef(){
        if (this->fixtureDefs.size() > 0){
            return this->fixtureDefs.back();
        }
        return nullptr;
    }

    void BodyDef::AddFixtureDef(b2FixtureDef *fixtureDef){
        
        //collect anchor data for future use of GetBodyDef
        //range of x and y
        if (typeid(*fixtureDef->shape) == typeid(b2CircleShape)){
    //        b2CircleShape *circleShape = (b2CircleShape*)fixtureDef->shape;
        }
        else if (typeid(*fixtureDef->shape) == typeid(b2ChainShape)){
    //        b2ChainShape *chainShape = (b2ChainShape*)fixtureDef->shape;
        }
        else if (typeid(*fixtureDef->shape) == typeid(b2EdgeShape)){
    //        b2EdgeShape *edgeShape = (b2EdgeShape*)fixtureDef->shape;
        }
        else if (typeid(*fixtureDef->shape) == typeid(b2PolygonShape)){
    //        b2PolygonShape *polygonShape = (b2PolygonShape*)fixtureDef->shape;
        }
        else {
            std::cout<<"couldn't figure out the type"<<std::endl;
        }
        
        this->fixtureDefs.push_back(fixtureDef);
    }

    void BodyDef::AddFixtureDef(b2Shape *shape, float32 density){
        b2FixtureDef *fixtureDef;
        fixtureDef = new b2FixtureDef;
        fixtureDef->shape = shape;
        fixtureDef->density = density;
        AddFixtureDef(fixtureDef);
    }

    void BodyDef::AddFixtureDef(b2Shape *shape){
        b2FixtureDef *fixtureDef;
        fixtureDef = new b2FixtureDef;
        fixtureDef->shape = shape;
        AddFixtureDef(fixtureDef);
    }

    std::vector<b2FixtureDef*> &BodyDef::GetFixtureDefs(){
        return this->fixtureDefs;
    }


    //---------------------------------------------------------------------------
    //Body
    //---------------------------------------------------------------------------

    ///
    /// Constructor
    ///
    Body::Body(b2Body *body){
        this->body = body;
    }

    ///
    /// Deconstructor
    ///
    Body::~Body(){
        //TODO-NP: handle pointers?
    }

    ///
    /// Gets the b2Body
    ///
    b2Body* Body::GetB2Body(){
        return this->body;
    }

    ///
    /// Gets the user data
    ///
    void* Body::GetUserData(){
        return this->userData;
    }
    
    ///
    /// Sets the user data
    ///
    void Body::SetUserData(void *data){
        this->userData = data;
    }
    
    //---------------------------------------------------------------------------

    unsigned long Body::GetDuringFixtureCollisionBeginActionCount(){
        return this->duringFixtureCollisionBeginActions.size();
    }

    unsigned long Body::GetDuringFixtureCollisionEndActionCount(){
        return this->duringFixtureCollisionEndActions.size();
    }

    unsigned long Body::GetDuringFixtureParticleCollisionBeginActionCount(){
        return this->duringFixtureParticleCollisionBeginActions.size();
    }

    unsigned long Body::GetDuringFixtureParticleCollisionEndActionCount(){
        return this->duringFixtureParticleCollisionEndActions.size();
    }

    unsigned long Body::GetPostFixtureCollisionBeginActionCount(){
        return this->postFixtureCollisionBeginActions.size();
    }

    unsigned long Body::GetPostFixtureCollisionEndActionCount(){
        return this->postFixtureCollisionEndActions.size();
    }

    unsigned long Body::GetPostFixtureParticleCollisioBeginActionCount(){
        return this->postFixtureParticleCollisionBeginActions.size();
    }

    unsigned long Body::GetPostFixtureParticleCollisionEndActionCount(){
        return this->postFixtureParticleCollisionEndActions.size();
    }

    //---------------------------------------------------------------------------
    //adapter methods of b2ContactListener methods for b2Body objects
    
    //during collision methods
    
    void Body::DuringBeginContact(b2Contact *contact){
        for (int i=0; i<this->duringFixtureCollisionBeginActions.size(); i++){
            this->duringFixtureCollisionBeginActions[i](contact);
        }
    }

    void Body::DuringEndContact(b2Contact *contact){
        for (int i=0; i<this->duringFixtureCollisionEndActions.size(); i++){
            this->duringFixtureCollisionEndActions[i](contact);
        }
    }

    void Body::DuringBeginContact(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact){
        for (int i=0; i<this->duringFixtureParticleCollisionBeginActions.size(); i++){
            this->duringFixtureParticleCollisionBeginActions[i](particleSystem, particleBodyContact);
        }
    }

    void Body::DuringEndContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex){
        for (int i=0; i<this->duringFixtureParticleCollisionEndActions.size(); i++){
            this->duringFixtureParticleCollisionEndActions[i](fixture, particleSystem, particleIndex);
        }
    }

    //post collision methods

    void Body::PostBeginContact(b2Fixture *fixtureA, b2Fixture *fixtureB){
        std::vector<void(*)(b2Fixture*, b2Fixture*)>::iterator iterator = this->postFixtureCollisionBeginActions.begin();
        for (; iterator != this->postFixtureCollisionBeginActions.end(); iterator++){
            (*iterator)(fixtureA, fixtureB);
        }
    }

    void Body::PostEndContact(b2Fixture *fixtureA, b2Fixture *fixtureB){
        std::vector<void(*)(b2Fixture*, b2Fixture*)>::iterator iterator = this->postFixtureCollisionEndActions.begin();
        for (; iterator != this->postFixtureCollisionEndActions.end(); iterator++){
            (*iterator)(fixtureA, fixtureB);
        }
    }

    void Body::PostBeginContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex){
        std::vector<void (*)(b2Fixture*, b2ParticleSystem*, int32)>::iterator iterator =
        this->postFixtureParticleCollisionBeginActions.begin();
        for (; iterator != this->postFixtureParticleCollisionBeginActions.end(); iterator++){
            (*iterator)(fixture, particleSystem, particleIndex);
        }
    }

    void Body::PostEndContact(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex){
        std::vector<void (*)(b2Fixture*, b2ParticleSystem*, int32)>::iterator iterator =
        this->postFixtureParticleCollisionEndActions.begin();
        for (; iterator != this->postFixtureParticleCollisionEndActions.end(); iterator++){
            (*iterator)(fixture, particleSystem, particleIndex);
        }
    }

    //---------------------------------------------------------------------------

    //during collision actions

    void Body::AddDuringFixtureCollisionBeginAction(void (*contactAction)(b2Contact *contact)){
        this->duringFixtureCollisionBeginActions.push_back(contactAction);
    }

    void Body::AddDuringFixtureCollisionEndAction(void (*contactAction)(b2Contact *contact)){
        this->duringFixtureCollisionEndActions.push_back(contactAction);
    }

    void Body::AddDuringFixtureParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact)){
        this->duringFixtureParticleCollisionBeginActions.push_back(contactAction);
    }

    void Body::AddDuringFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex)){
        this->duringFixtureParticleCollisionEndActions.push_back(contactAction);
    }

    //post collision actions

    void Body::AddPostFixtureCollisionBeginAction(void (*contactAction)(b2Fixture *fixtureA, b2Fixture *fixtureB)){
        this->postFixtureCollisionBeginActions.push_back(contactAction);
    }

    void Body::AddPostFixtureCollisionEndAction(void (*contactAction)(b2Fixture *fixtureA, b2Fixture *fixtureB)){
        this->postFixtureCollisionEndActions.push_back(contactAction);
    }

    void Body::AddPostFixtureParticleCollisionBeginAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex)){
        this->postFixtureParticleCollisionBeginActions.push_back(contactAction);
    }

    void Body::AddPostFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex)){
        this->postFixtureParticleCollisionEndActions.push_back(contactAction);
    }
    
    //---------------------------------------------------------------------------

    //during collision actions

    void Body::RemoveDuringFixtureCollisionBeginAction(void (*contactAction)(b2Contact *contact)){
        std::vector<void (*)(b2Contact*)>::iterator iterator;
        iterator = find(this->duringFixtureCollisionBeginActions.begin(),
                        this->duringFixtureCollisionBeginActions.end(),
                        contactAction);
        
        if (iterator != this->duringFixtureCollisionBeginActions.end()){
            this->duringFixtureCollisionBeginActions.erase(iterator);
        }
    }

    void Body::RemoveDuringFixtureCollisionEndAction(void (*contactAction)(b2Contact *contact)){
        std::vector<void (*)(b2Contact*)>::iterator iterator;
        iterator = find(this->duringFixtureCollisionEndActions.begin(),
                        this->duringFixtureCollisionEndActions.end(),
                        contactAction);
        
        if (iterator != this->duringFixtureCollisionEndActions.end()){
            this->duringFixtureCollisionEndActions.erase(iterator);
        }
    }

    void Body::RemoveDuringFixtureParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact)){
        std::vector<void (*)(b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact)>::iterator iterator;
        iterator = find(this->duringFixtureParticleCollisionBeginActions.begin(),
                        this->duringFixtureParticleCollisionBeginActions.end(),
                        contactAction);
        
        if (iterator != this->duringFixtureParticleCollisionBeginActions.end()){
            this->duringFixtureParticleCollisionBeginActions.erase(iterator);
        }
    }

    void Body::RemoveDuringFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex)){
        std::vector<void (*)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex)>::iterator iterator;
        iterator = find(this->duringFixtureParticleCollisionEndActions.begin(),
                        this->duringFixtureParticleCollisionEndActions.end(),
                        contactAction);
        
        if (iterator != this->duringFixtureParticleCollisionEndActions.end()){
            this->duringFixtureParticleCollisionEndActions.erase(iterator);
        }
    }

    //post collision actions

    void Body::RemovePostFixtureCollisionBeginAction(void (*contactAction)(b2Fixture *fixtureA, b2Fixture *fixtureB)){
        std::vector<void (*)(b2Fixture *fixtureA, b2Fixture *fixtureB)>::iterator iterator;
        iterator = find(this->postFixtureCollisionBeginActions.begin(),
                        this->postFixtureCollisionBeginActions.end(),
                        contactAction);
        
        if (iterator != this->postFixtureCollisionBeginActions.end()){
            this->postFixtureCollisionBeginActions.erase(iterator);
        }
    }

    void Body::RemovePostFixtureCollisionEndAction(void (*contactAction)(b2Fixture *fixtureA, b2Fixture *fixtureB)){
        std::vector<void (*)(b2Fixture *fixtureA, b2Fixture *fixtureB)>::iterator iterator;
        iterator = find(this->postFixtureCollisionEndActions.begin(),
                        this->postFixtureCollisionEndActions.end(),
                        contactAction);
        
        if (iterator != this->postFixtureCollisionEndActions.end()){
            this->postFixtureCollisionEndActions.erase(iterator);
        }
    }

    void Body::RemovePostFixtureParticleCollisionBeginAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex)){
        std::vector<void (*)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex)>::iterator iterator;
        iterator = find(this->postFixtureParticleCollisionBeginActions.begin(),
                        this->postFixtureParticleCollisionBeginActions.end(),
                        contactAction);
        
        if (iterator != this->postFixtureParticleCollisionBeginActions.end()){
            this->postFixtureParticleCollisionBeginActions.erase(iterator);
        }
    }

    void Body::RemovePostFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex)){
        std::vector<void (*)(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex)>::iterator iterator;
        iterator = find(this->postFixtureParticleCollisionEndActions.begin(),
                        this->postFixtureParticleCollisionEndActions.end(),
                        contactAction);
        
        if (iterator != this->postFixtureParticleCollisionEndActions.end()){
            this->postFixtureParticleCollisionEndActions.erase(iterator);
        }
    }

}
