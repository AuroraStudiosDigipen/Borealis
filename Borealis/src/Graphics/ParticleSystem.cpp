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
		mRandomStartSizeBool = particleSystemComponent.randomStartSize;
		mStartSize = particleSystemComponent.startSize; //if not 3d, use .x for size
		m_3DStartRotationBool = particleSystemComponent._3DStartRotationBool; //if false, uniform rotation
		mStartRotation = particleSystemComponent.startRotation; // if not 3d, use .x for rotation
		mRandomStartColor = particleSystemComponent.randomStartColor;
		mStartColor = particleSystemComponent.startColor;
		mStartColor2 = particleSystemComponent.startColor2;
		mGravityModifer = particleSystemComponent.gravityModifer;
		mSimulationSpeed = particleSystemComponent.simulationSpeed; //speed of simulation
		mMaxParticles = particleSystemComponent.maxParticles;
		mParticlesCount = 0;
		mRateOverTime = particleSystemComponent.rateOverTime;
		mAngle = particleSystemComponent.angle;
		mRadius = particleSystemComponent.radius;
		mRadiusThickness = particleSystemComponent.radiusThickness;
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
		particle.isActive = true;

		// Get emitter world position
		glm::vec3 emitterPos = transform.GetGlobalTranslate();

		// Convert emitter rotation (Euler to Quaternion)
		glm::vec3 rotation = transform.GetGlobalRotation();
		glm::quat orientation = glm::quat(rotation);

		// Generate a random position in a 2D disc (radius = mRadius)
		float randomAngle = static_cast<float>(rand()) / RAND_MAX * glm::two_pi<float>();
		float randomRadius = mRadius * glm::sqrt(static_cast<float>(rand()) / RAND_MAX); // Even distribution in a circle

		glm::vec3 spawnOffset = glm::vec3(
			randomRadius * glm::cos(randomAngle),
			0.0f,  // Keep it on the base (Y = 0)
			randomRadius * glm::sin(randomAngle)
		);

		// Offset the particle spawn position
		particle.position = emitterPos + (orientation * spawnOffset);

		// Generate a random direction within the cone
		float coneAngle = glm::radians(mAngle);

		float u = static_cast<float>(rand()) / RAND_MAX;
		float v = static_cast<float>(rand()) / RAND_MAX;

		float theta = v * 2.0f * glm::pi<float>(); // Random around the circle
		float phi = glm::acos(1.0f - u * (1.0f - glm::cos(coneAngle))); // Spread within the cone

		glm::vec3 direction = glm::vec3(
			glm::sin(phi) * glm::cos(theta),
			glm::cos(phi), // Y-axis is up
			glm::sin(phi) * glm::sin(theta)
		);

		// Align direction with emitter rotation
		glm::vec3 finalDirection = glm::normalize(orientation * direction);

		// Apply velocity
		particle.startVelocity = startSpeed * finalDirection;

		// Random start color (if enabled)
		if (mRandomStartColor)
		{
			float colorLerp = static_cast<float>(rand()) / RAND_MAX;
			particle.startColor = glm::mix(mStartColor, mStartColor2, colorLerp);
		}
		else
		{
			particle.startColor = mStartColor;
		}

		// Random start size (if enabled)
		if (mRandomStartSizeBool)
		{
			float sizeLerp = static_cast<float>(rand()) / RAND_MAX;
			particle.startSize = glm::mix(mStartSize, mStartSize2, sizeLerp);
		}
		else
		{
			particle.startSize = mStartSize;
		}

		// Set rotation
		particle.startRotation = glm::quat(mStartRotation);

		// Reset life counter
		particle.life = 0.0f;
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
		mRandomStartSizeBool = particleSystemComponent.randomStartSize;
		mStartSize = particleSystemComponent.startSize; //if not 3d, use .x for size
		mStartSize2 = particleSystemComponent.startSize2;
		m_3DStartRotationBool = particleSystemComponent._3DStartRotationBool; //if false, uniform rotation
		mStartRotation = particleSystemComponent.startRotation; // if not 3d, use .x for rotation
		mRandomStartColor = particleSystemComponent.randomStartColor;
		mStartColor = particleSystemComponent.startColor;
		mStartColor2 = particleSystemComponent.startColor2;
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
		mRadius = particleSystemComponent.radius;
		mRadiusThickness = particleSystemComponent.radiusThickness;

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

