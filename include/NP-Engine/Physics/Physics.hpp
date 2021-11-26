//
//  Physics.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/20/18.
//  Copyright © 2018 Nathan Phipps. All rights reserved.
//

#ifndef Physics_hpp
#define Physics_hpp

#include <Box2D/Box2D.h>
#include <iostream>
#include "Body.hpp"
#include "ContactListener.hpp"
#include "Constants.h"

namespace np
{

	class Physics
	{
	private:
		b2World* world;
		b2Draw* drawer = nullptr;
		unsigned int frameRate = 60;
		float32 timeStep;
		int32 velocityIterations = 8;
		int32 positionIterations = 3;
		int32 particleIterations = 1;
		bool enableStep = true;
		ContactListener* contactListener;

		Physics();
		~Physics();

	public:
		static Physics& GetInstance();

		inline b2World* GetWorld()
		{
			return world;
		}

		void Start();
		void Pause();

		void SetFrameRate(unsigned int rate);
		void SetVelocityIterations(int32 iterations);
		void SetPositionIterations(int32 iterations);
		void SetParticleIterations(int32 iterations);

		void SetEnableStep(bool enable = true);
		void Step();

		void EnableDebugDraw(bool enable);
		bool IsDebugDrawEnabled();
		void DrawDebugData();
		void SetDegubDrawer(b2Draw* drawer);

		void SetCardinalGravity(b2Vec2* gravity);
		b2Vec2 GetCardinalGravity();

		void SetRadialGravity(b2Vec2* gravityCenter, float radius);
		b2Vec2 GetRadialGravityCenter();
		float GetRadialGravityRadius();

		//---------------------------------------------------------------------------

		ParticleSystem* CreateParticleSystem(b2ParticleSystemDef* particleSystemDef);
		bool RemoveParticleSystem();

		Body* CreateBody(BodyDef* bodyDef);
		Body* CreateBody(BodyDef* bodyDef, b2Shape* shape);
		Body* CreateBody(BodyDef* bodyDef, b2Shape* shape, float32 density);
		Body* CreateBody(BodyDef* bodyDef, b2FixtureDef* fixtureDef);
		bool RemoveBody();

		//---------------------------------------------------------------------------

		// subscribe for during collision events

		void SubscribeForDuringFixtureCollisionBeginEvents(Body* body, void (*contactAction)(b2Contact*));
		void SubscribeForDuringFixtureCollisionEndEvents(Body* body, void (*contactAction)(b2Contact*));
		void SubscribeForDuringFixtureParticleCollisionBeginEvents(Body* body,
																   void (*contactAction)(b2ParticleSystem*,
																						 b2ParticleBodyContact*));
		void SubscribeForDuringFixtureParticleCollisionEndEvents(Body* body,
																 void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32));

		void SubscribeForDuringFixtureParticleCollisionBeginEvents(ParticleSystem* particleSystem,
																   void (*contactAction)(b2ParticleSystem*,
																						 b2ParticleBodyContact*));
		void SubscribeForDuringFixtureParticleCollisionEndEvents(ParticleSystem* particleSystem,
																 void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32));
		void SubscribeForDuringParticleCollisionBeginEvents(ParticleSystem* particleSystem,
															void (*contactAction)(b2ParticleSystem*, b2ParticleContact*));
		void SubscribeForDuringParticleCollisionEndEvents(ParticleSystem* particleSystem,
														  void (*contactAction)(b2ParticleSystem*, int32, int32));

		// subscribe for post collision events

		void SubscribeForPostFixtureCollisionBeginEvents(Body* body, void (*contactAction)(b2Fixture*, b2Fixture*));
		void SubscribeForPostFixtureCollisionEndEvents(Body* body, void (*contactAction)(b2Fixture*, b2Fixture*));
		void SubscribeForPostFixtureParticleCollisionBeginEvents(Body* body,
																 void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32));
		void SubscribeForPostFixtureParticleCollisionEndEvents(Body* body,
															   void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32));

		void SubscribeForPostFixtureParticleCollisionBeginEvents(ParticleSystem* particleSystem,
																 void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32));
		void SubscribeForPostFixtureParticleCollisionEndEvents(ParticleSystem* particleSystem,
															   void (*contactAction)(b2Fixture*, b2ParticleSystem*, int32));
		void SubscribeForPostParticleCollisionBeginEvents(ParticleSystem* particleSystem,
														  void (*contactAction)(b2ParticleSystem*, int32, int32));
		void SubscribeForPostParticleCollisionEndEvents(ParticleSystem* particleSystem,
														void (*contactAction)(b2ParticleSystem*, int32, int32));

		//---------------------------------------------------------------------------

		// unsubscribe for during collision events

		//        void UnsubscribeForPostFixtureCollisionBeginEvents(Body* body);
		//        void UnsubscribeForPostFixtureCollisionEndEvents(Body* body);
		//        void UnsubscribeForPostFixtureParticleCollisionBeginEvents(Body* body);
		//        void UnsubscribeForPostFixtureParticleCollisionEndEvents(Body* body);

		void UnsubscribeForDuringFixtureCollisionBeginEvents(Body* body);
		void UnsubscribeForDuringFixtureCollisionEndEvents(Body* body);
		void UnsubscribeForDuringFixtureParticleCollisionBeginEvents(Body* body);
		void UnsubscribeForDuringFixtureParticleCollisionEndEvents(Body* body);

		//        void UnsubscribeForPostFixtureParticleCollisionBeginEvents(ParticleSystem* particleSystem);
		//        void UnsubscribeForPostFixtureParticleCollisionEndEvents(ParticleSystem* particleSystem);
		//        void UnsubscribeForPostParticleCollisionBeginEvents(ParticleSystem* particleSystem);
		//        void UnsubscribeForPostParticleCollisionEndEvents(ParticleSystem* particleSystem);

		void UnsubscribeForDuringFixtureParticleCollisionBeginEvents(ParticleSystem* particleSystem);
		void UnsubscribeForDuringFixtureParticleCollisionEndEvents(ParticleSystem* particleSystem);
		void UnsubscribeForDuringParticleCollisionBeginEvents(ParticleSystem* particleSystem);
		void UnsubscribeForDuringParticleCollisionEndEvents(ParticleSystem* particleSystem);

		// unsubscribe for post collision events

		void UnsubscribeForDuringFixtureCollisionBeginEvents(Body* body, void (*contactAction)(b2Contact* contact));
		//        void UnsubscribeForDuringFixtureCollisionEndEvents(Body* body);
		//        void UnsubscribeForDuringFixtureParticleCollisionBeginEvents(Body* body);
		//        void UnsubscribeForDuringFixtureParticleCollisionEndEvents(Body* body);

		void UnsubscribeForPostFixtureCollisionBeginEvents(Body* body);
		void UnsubscribeForPostFixtureCollisionEndEvents(Body* body);
		void UnsubscribeForPostFixtureParticleCollisionBeginEvents(Body* body);
		void UnsubscribeForPostFixtureParticleCollisionEndEvents(Body* body);

		//        void UnsubscribeForDuringFixtureParticleCollisionBeginEvents(ParticleSystem* particleSystem);
		//        void UnsubscribeForDuringFixtureParticleCollisionEndEvents(ParticleSystem* particleSystem);
		//        void UnsubscribeForDuringParticleCollisionBeginEvents(ParticleSystem* particleSystem);
		//        void UnsubscribeForDuringParticleCollisionEndEvents(ParticleSystem* particleSystem);

		void UnsubscribeForPostFixtureParticleCollisionBeginEvents(ParticleSystem* particleSystem);
		void UnsubscribeForPostFixtureParticleCollisionEndEvents(ParticleSystem* particleSystem);
		void UnsubscribeForPostParticleCollisionBeginEvents(ParticleSystem* particleSystem);
		void UnsubscribeForPostParticleCollisionEndEvents(ParticleSystem* particleSystem);
	};

} // namespace np

#endif /* Physics_hpp */
