//
//  ParticleSystem.cpp
//  Project Space
//
//  Created by Nathan Phipps on 10/5/18.
//  Copyright © 2018 Nathan Phipps. All rights reserved.
//

#include "NP-Engine/Physics/ParticleSystem.hpp"

namespace np
{

	///
	/// constructor
	///
	ParticleSystem::ParticleSystem(b2ParticleSystem* particleSystem)
	{
		this->particleSystem = particleSystem;
	}

	///
	/// deconstructor
	///
	ParticleSystem::~ParticleSystem()
	{
		// TODO-NP: handle pointers?
	}

	///
	/// gets the b2ParticleSystem object
	///
	b2ParticleSystem* ParticleSystem::GetB2ParticleSystem()
	{
		return this->particleSystem;
	}

	///
	/// gets the user data
	///
	void* ParticleSystem::GetUserData()
	{
		return this->userData;
	}

	///
	/// sets the user data
	///
	void ParticleSystem::SetUserData(void* data)
	{
		this->userData = data;
	}

	//---------------------------------------------------------------------------

	void ParticleSystem::DuringBeginContact(b2ParticleSystem* particleSystem, b2ParticleBodyContact* particleBodyContact)
	{
		std::cout << "hello from inside during begin contact for particle system and body" << std::endl;
	}

	void ParticleSystem::DuringEndContact(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex)
	{
		std::cout << "hello from inside during end contact for particle system and body" << std::endl;
	}

	void ParticleSystem::DuringBeginContact(b2ParticleSystem* paricleSystem, b2ParticleContact* particleContact)
	{
		std::cout << "hello from inside during begin contact for particle system" << std::endl;
	}

	void ParticleSystem::DuringEndContact(b2ParticleSystem* particleSystem, int32 firstParticleIndex, int32 secondParticleIndex)
	{
		std::cout << "hello from inside during end contact for particle system" << std::endl;
	}

	void ParticleSystem::PostBeginContact(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex)
	{
		std::cout << "hello from inside post begin contact for particle system and body" << std::endl;
	}

	void ParticleSystem::PostEndContact(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex)
	{
		std::cout << "hello from inside post end contact for particle system and body" << std::endl;
	}

	void ParticleSystem::PostBeginContact(b2ParticleSystem* particleSystem, int32 firstParticleIndex, int32 secondParticleIndex)
	{
		std::cout << "hello from inside post begin contact for particle system" << std::endl;
	}

	void ParticleSystem::PostEndContact(b2ParticleSystem* particleSystem, int32 firstParticleIndex, int32 secondParticleIndex)
	{
		std::cout << "hello from inside post end contact for particle system" << std::endl;
	}

	//---------------------------------------------------------------------------

	// during collision actions

	void ParticleSystem::AddDuringFixtureParticleCollisionBeginAction(
		void (*contactAction)(b2ParticleSystem* particleSystem, b2ParticleBodyContact* particleBodyContact))
	{
		this->duringFixtureParticleCollisionBeginActions.push_back(contactAction);
	}

	void ParticleSystem::AddDuringFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture* fixture,
																						  b2ParticleSystem* particleSystem,
																						  int32 particleIndex))
	{
		this->duringFixtureParticleCollisionEndActions.push_back(contactAction);
	}

	void ParticleSystem::AddDuringParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem* particleSystem,
																					 b2ParticleContact* particleContact))
	{
		this->duringParticleCollisionBeginActions.push_back(contactAction);
	}

	void ParticleSystem::AddDuringParticleCollisionEndAction(void (*contactAction)(b2ParticleSystem* particleSystem,
																				   int32 firstParticleIndex,
																				   int32 secondParticleIndex))
	{
		this->duringParticleCollisionEndActions.push_back(contactAction);
	}

	// post collision actions

	void ParticleSystem::AddPostFixtureParticleCollisionBeginAction(void (*contactAction)(b2Fixture* fixture,
																						  b2ParticleSystem* particleSystem,
																						  int32 particleIndex))
	{
		this->postFixtureParticleCollisionBeginActions.push_back(contactAction);
	}

	void ParticleSystem::AddPostFixtureParticleCollisionEndAction(void (*contactAction)(b2Fixture* fixture,
																						b2ParticleSystem* particleSystem,
																						int32 particleIndex))
	{
		this->postFixtureParticleCollisionEndActions.push_back(contactAction);
	}

	void ParticleSystem::AddPostParticleCollisionBeginAction(void (*contactAction)(b2ParticleSystem* particleSystem,
																				   int32 firstParticleIndex,
																				   int32 secondParticleIndex))
	{
		this->postParticleCollisionBeginActions.push_back(contactAction);
	}

	void ParticleSystem::AddPostParticleCollisionEndAction(void (*contactAction)(b2ParticleSystem* particleSystem,
																				 int32 firstParticleIndex,
																				 int32 secondParticleIndex))
	{
		this->postParticleCollisionEndActions.push_back(contactAction);
	}

} // namespace np
