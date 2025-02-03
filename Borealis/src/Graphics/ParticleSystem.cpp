/******************************************************************************
/*!
\file       ParticleSystem.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       January 24 2025
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>

#include <Graphics/ParticleSystem.hpp>
#include <Assets/AssetManager.hpp>
#include <Graphics/Texture.hpp>

#include <Scene/Components.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/euler_angles.hpp>


namespace Borealis
{
	void ParticleSystem::Init(ParticleSystemComponent& particleSystemComponent)
	{
		mDuration = particleSystemComponent.duration; //lifecycle //min 0.05
		mSpawnAccumulator = 0.f;
		mLooping = particleSystemComponent.looping;
		mStartDelay = particleSystemComponent.startDelay;
		mStartLifeTime = particleSystemComponent.startLifeTime;
		mStartSpeed = particleSystemComponent.startSpeed; //speed of particles
		m_3DStartSizeBool = particleSystemComponent._3DStartSizeBool; //if false, uniform scale
		mStartSize = particleSystemComponent.startSize; //if not 3d, use .x for size
		m_3DStartRotationBool = particleSystemComponent._3DStartRotationBool; //if false, uniform rotation
		mStartRotation = particleSystemComponent.startRotation; // if not 3d, use .x for rotation
		mStartColor = particleSystemComponent.startColor;
		mGravityModifer = particleSystemComponent.gravityModifer;
		mSimulationSpeed = particleSystemComponent.simulationSpeed; //speed of simulation
		mMaxParticles = particleSystemComponent.maxParticles;
		mParticlesCount = 0;
		mRateOverTime = particleSystemComponent.rateOverTime;
		mAngle = particleSystemComponent.angle;
		mTimer = 0.f;

		mParticles.resize(mMaxParticles);
		mDeadParticles.resize(mMaxParticles);
	}

	void ParticleSystem::Update(ParticleSystemComponent& particleSystemComponent, TransformComponent & transfrom, float dt)
	{
		SyncWithEditor(particleSystemComponent);
		mTimer += dt;

		if (mTimer >= mDuration)
		{
			if (mLooping)
				mTimer = 0.f;
			else
				return;
		}

		mSpawnAccumulator += dt * mRateOverTime;
		while (mParticlesCount < mMaxParticles && mSpawnAccumulator >= 1.f)
		{
			mSpawnAccumulator -= 1.f;
			if (mDeadParticlesCount > 0)
			{
				SpawnParticle(transfrom, *mDeadParticles[mDeadParticlesCount - 1], mStartSpeed);
				mDeadParticlesCount--;
				mParticlesCount++;
			}
			else
			{
				SpawnParticle(transfrom, mParticles[mParticlesCount], mStartSpeed);
				mParticlesCount++;
			}
		}

		for (Particle& particle : mParticles)
		{
			if (!particle.isActive) continue;
			particle.life += dt;

			if (particle.life >= mStartLifeTime)
			{
				particle.isActive = false;
				mDeadParticles[mDeadParticlesCount] = &particle;
				mDeadParticlesCount++;
				mParticlesCount--;
				continue;
			}

			particle.position += particle.startVelocity * dt;
		}
	}

	std::vector<Particle> const& ParticleSystem::GetParticles()
	{
		return mParticles;
	}

	uint32_t ParticleSystem::GetParticlesCount()
	{
		return mParticlesCount;
	}

	Ref<Texture2D> ParticleSystem::GetDefaultParticleTexture()
	{
		if (!mDefaultParticle)
		{
			mDefaultParticle = Texture2D::GetDefaultTexture();
		}
		return mDefaultParticle;
	}

	float RandomRange(float min, float max) {
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}

	void ParticleSystem::SpawnParticle(TransformComponent & transform, Particle& particle, float startSpeed)
	{
		//particle.isActive = true;
		//particle.position = transform.GetGlobalTranslate();

		//// Get the forward direction of the transform
		//glm::vec3 forward = transform.GetGlobalRotation(); // Assuming you have a method to get the forward vector

		//// Convert the angle to radians
		//float angleInRadians = glm::radians(mAngle);

		//// Generate a random direction within the cone defined by the angle
		//glm::vec3 randomDirection = glm::sphericalRand(25.0f); // Random direction on the unit sphere
		//glm::vec3 coneDirection = glm::mix(forward, randomDirection, angleInRadians);

		//// Normalize the cone direction to ensure it's a unit vector
		//coneDirection = glm::normalize(coneDirection);

		//// Set the initial velocity of the particle
		//particle.startVelocity = startSpeed * coneDirection;

		//particle.startColor = mStartColor;
		//particle.startSize = mStartSize;
		//particle.startRotation = glm::quat(mStartRotation);

		//particle.life = 0.f;

		particle.isActive = true;
		particle.position = transform.GetGlobalTranslate();

		// Get the Euler angles (pitch, yaw, roll) from the transform
		glm::vec3 eulerAngles = transform.GetGlobalRotation(); // Assuming this returns Euler angles

		// Convert Euler angles to a rotation matrix
		glm::mat4 rotationMatrix = glm::eulerAngleXYZ(glm::radians(eulerAngles.x), glm::radians(eulerAngles.y), glm::radians(eulerAngles.z));

		// Get the forward direction from the rotation matrix (unit vector along the Z-axis)
		glm::vec3 forwardDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));

		// Convert the angle to radians
		float angleInRadians = glm::radians(mAngle);

		// Randomize the direction within the cone
		float randomAngleX = RandomRange(-angleInRadians, angleInRadians);
		float randomAngleY = RandomRange(-angleInRadians, angleInRadians);

		// Apply the random deviation to the forward direction (randomize within the cone)
		glm::vec3 randomDirection = glm::vec3(
			glm::sin(randomAngleX),  // X component within the cone
			glm::sin(randomAngleY),  // Y component within the cone
			glm::cos(randomAngleX)   // Z component for the cone's depth
		);

		// Combine the base direction with the random deviation
		glm::vec3 finalDirection = glm::normalize(forwardDirection + randomDirection);

		// Set the particle's velocity, scaled by the startSpeed
		particle.startVelocity = startSpeed * finalDirection;

		// Set other properties
		particle.startColor = mStartColor;
		particle.startSize = mStartSize;
		particle.startRotation = glm::quat(mStartRotation);

		particle.life = 0.f;
	}

	void ParticleSystem::SyncWithEditor(ParticleSystemComponent& particleSystemComponent)
	{
		if (!particleSystemComponent.isEdited) return;

		mDuration = particleSystemComponent.duration; //lifecycle //min 0.05
		mLooping = particleSystemComponent.looping;
		mStartDelay = particleSystemComponent.startDelay;
		mStartLifeTime = particleSystemComponent.startLifeTime;
		mStartSpeed = particleSystemComponent.startSpeed; //speed of particles
		m_3DStartSizeBool = particleSystemComponent._3DStartSizeBool; //if false, uniform scale
		mStartSize = particleSystemComponent.startSize; //if not 3d, use .x for size
		m_3DStartRotationBool = particleSystemComponent._3DStartRotationBool; //if false, uniform rotation
		mStartRotation = particleSystemComponent.startRotation; // if not 3d, use .x for rotation
		mStartColor = particleSystemComponent.startColor;
		mGravityModifer = particleSystemComponent.gravityModifer;
		mSimulationSpeed = particleSystemComponent.simulationSpeed; //speed of simulation
		if(mMaxParticles != particleSystemComponent.maxParticles)
		{
			mMaxParticles = particleSystemComponent.maxParticles;
			mParticles.clear();
			mDeadParticles.clear();
			mParticles.resize(mMaxParticles);
			mDeadParticles.resize(mMaxParticles);
			mParticlesCount = 0;
			mDeadParticlesCount = 0;

		}
		mRateOverTime = particleSystemComponent.rateOverTime;
		mAngle = particleSystemComponent.angle;

		UpdateParticles(mStartSize, mStartRotation, mStartColor);
	}

	void ParticleSystem::UpdateParticles(glm::vec3 size, glm::vec3 rotation, glm::vec4 color)
	{
		for (Particle& particle : mParticles)
		{
			if (!particle.isActive) continue;

			particle.startSize = size;
			particle.startRotation = rotation;
			particle.startColor = color;
		}
	}
}

