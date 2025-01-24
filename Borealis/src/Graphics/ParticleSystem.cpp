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
				SpawnParticle(transfrom, *mDeadParticles[mDeadParticlesCount - 1]);
				mDeadParticlesCount--;
				mParticlesCount++;
			}
			else
			{
				SpawnParticle(transfrom, mParticles[mParticlesCount]);
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

	void ParticleSystem::SpawnParticle(TransformComponent & transfrom, Particle& particle)
	{
		particle.isActive = true;
		particle.position = transfrom.GetGlobalTranslate();

		particle.startVelocity = transfrom.GetGlobalRotation() +
			glm::vec3(RandomRange(-mAngle, mAngle),
					  RandomRange(-mAngle, mAngle),
					  RandomRange(-mAngle, mAngle));

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
			mParticles.resize(mMaxParticles);
			mDeadParticles.resize(mMaxParticles);
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

