//
//  PhysicsContactListener.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/24/18.
//  Copyright © 2018 Nathan Phipps. All rights reserved.
//

#ifndef PhysicsContactListener_hpp
#define PhysicsContactListener_hpp

#include <set>
#include "Body.hpp"
#include "ParticleSystem.hpp"

#include "NP-Engine/Vendor/Box2dInclude.hpp"

namespace np
{

	class ContactListener : b2ContactListener
	{
	private:
		friend class Physics;

		//---------------------------------------------------------------------------
		// post step collision event parameters

		struct PostFixtureCollisionParameters
		{
			Body* body;
			b2Fixture* fixtureA;
			b2Fixture* fixtureB;
		};

		struct PostFixtureParticleCollisionParameters
		{
			Body* body;
			b2Fixture* fixture;
			ParticleSystem* particleSystem;
			int particleIndex;
		};

		struct PostParticleCollisionParameters
		{
			ParticleSystem* particleSystem;
			int firstParticleIndex;
			int secondParticleIndex;
		};

		//---------------------------------------------------------------------------
		// body and particle system event registrations represented by sets

		std::set<b2Body*> duringFixtureCollisionBeginBodies;
		std::set<b2Body*> duringFixtureCollisionEndBodies;

		std::set<b2Body*> duringFixtureParticleCollisionBeginBodies;
		std::set<b2Body*> duringFixtureParticleCollisionEndBodies;

		std::set<b2ParticleSystem*> duringFixtureParticleCollisionBeginSystems;
		std::set<b2ParticleSystem*> duringFixtureParticleCollisionEndSystems;

		std::set<b2ParticleSystem*> duringParticleCollisionBeginSystems;
		std::set<b2ParticleSystem*> duringParticleCollisionEndSystems;

		std::set<b2Body*> postFixtureCollisionBeginBodies;
		std::set<b2Body*> postFixtureCollisionEndBodies;

		std::set<b2Body*> postFixtureParticleCollisionBeginBodies;
		std::set<b2Body*> postFixtureParticleCollisionEndBodies;

		std::set<b2ParticleSystem*> postFixtureParticleCollisionBeginSystems;
		std::set<b2ParticleSystem*> postFixtureParticleCollisionEndSystems;

		std::set<b2ParticleSystem*> postParticleCollisionBeginSystems;
		std::set<b2ParticleSystem*> postParticleCollisionEndSystems;

		//---------------------------------------------------------------------------
		// post event vectors

		std::vector<PostFixtureCollisionParameters> postFixtureCollisionBeginEvents;
		std::vector<PostFixtureCollisionParameters> postFixtureCollisionEndEvents;

		std::vector<PostFixtureParticleCollisionParameters> postFixtureParticleCollisionBeginEvents;
		std::vector<PostFixtureParticleCollisionParameters> postFixtureParticleCollisionEndEvents;

		std::vector<PostParticleCollisionParameters> postParticleCollisionBeginEvents;
		std::vector<PostParticleCollisionParameters> postParticleCollisionEndEvents;

		//---------------------------------------------------------------------------
		// subscribe and unsubscribe for during collision events

		void SubscribeForDuringFixtureCollisionBeginEvents(b2Body* body);
		void SubscribeForDuringFixtureCollisionEndEvents(b2Body* body);
		void SubscribeForDuringFixtureParticleCollisionBeginEvents(b2Body* body);
		void SubscribeForDuringFixtureParticleCollisionEndEvents(b2Body* body);

		void SubscribeForDuringFixtureParticleCollisionBeginEvents(b2ParticleSystem* particleSystem);
		void SubscribeForDuringFixtureParticleCollisionEndEvents(b2ParticleSystem* particleSystem);
		void SubscribeForDuringParticleCollisionBeginEvents(b2ParticleSystem* particleSystem);
		void SubscribeForDuringParticleCollisionEndEvents(b2ParticleSystem* particleSystem);

		void UnsubscribeForDuringFixtureCollisionBeginEvents(b2Body* body);
		void UnsubscribeForDuringFixtureCollisionEndEvents(b2Body* body);
		void UnsubscribeForDuringFixtureParticleCollisionBeginEvents(b2Body* body);
		void UnsubscribeForDuringFixtureParticleCollisionEndEvents(b2Body* body);

		void UnsubscribeForDuringFixtureParticleCollisionBeginEvents(b2ParticleSystem* particleSystem);
		void UnsubscribeForDuringFixtureParticleCollisionEndEvents(b2ParticleSystem* particleSystem);
		void UnsubscribeForDuringParticleCollisionBeginEvents(b2ParticleSystem* particleSystem);
		void UnsubscribeForDuringParticleCollisionEndEvents(b2ParticleSystem* particleSystem);

		// subscribe and unsubscribe for post collision events

		void SubscribeForPostFixtureCollisionBeginEvents(b2Body* body);
		void SubscribeForPostFixtureCollisionEndEvents(b2Body* body);
		void SubscribeForPostFixtureParticleCollisionBeginEvents(b2Body* body);
		void SubscribeForPostFixtureParticleCollisionEndEvents(b2Body* body);

		void SubscribeForPostFixtureParticleCollisionBeginEvents(b2ParticleSystem* particleSystem);
		void SubscribeForPostFixtureParticleCollisionEndEvents(b2ParticleSystem* particleSystem);
		void SubscribeForPostParticleCollisionBeginEvents(b2ParticleSystem* particleSystem);
		void SubscribeForPostParticleCollisionEndEvents(b2ParticleSystem* particleSystem);

		void UnsubscribeForPostFixtureCollisionBeginEvents(b2Body* body);
		void UnsubscribeForPostFixtureCollisionEndEvents(b2Body* body);
		void UnsubscribeForPostFixtureParticleCollisionBeginEvents(b2Body* body);
		void UnsubscribeForPostFixtureParticleCollisionEndEvents(b2Body* body);

		void UnsubscribeForPostFixtureParticleCollisionBeginEvents(b2ParticleSystem* particleSystem);
		void UnsubscribeForPostFixtureParticleCollisionEndEvents(b2ParticleSystem* particleSystem);
		void UnsubscribeForPostParticleCollisionBeginEvents(b2ParticleSystem* particleSystem);
		void UnsubscribeForPostParticleCollisionEndEvents(b2ParticleSystem* particleSystem);

		//---------------------------------------------------------------------------
		// b2ContactListener methods

		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);

		void BeginContact(b2ParticleSystem* particleSystem, b2ParticleBodyContact* particleBodyContact);
		void EndContact(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex);

		void BeginContact(b2ParticleSystem* particleSystem, b2ParticleContact* particleContact);
		void EndContact(b2ParticleSystem* particleSystem, int32 firstParticleIndex, int32 secondParticleIndex);

		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
		void PostSolve(b2Contact* contact, const b2ContactImpulse* contactImpulse);

		//---------------------------------------------------------------------------
		// private methods for friend class to use

		void TriggerPostStepCollisionEvents();
		void ClearPostCollisionEvents();
	};

} // namespace np

#endif /* PhysicsContactListener_hpp */
