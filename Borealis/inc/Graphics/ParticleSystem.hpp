/******************************************************************************
/*!
\file       ParticleSystem.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       January 24 2025
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef ParticleSystem_HPP
#define ParticleSystem_HPP

#include <Core/Core.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


namespace Borealis
{
	class Texture2D;
	struct Particle
	{
		bool		isActive = false;
		float		life = 0.f;
		glm::vec3	position = glm::vec3{5.f};
		glm::vec3	startVelocity = glm::vec3{5.f};
		glm::vec3	startSize = glm::vec3{ 1.f }; //if not 3d, use .x for size
		glm::quat	startRotation = glm::vec3{ 0.f }; // if not 3d, use .x for rotation
		glm::vec4	startColor = glm::vec4{ 1.f }; 
		glm::vec4	currentColor = glm::vec4{ 1.f }; 
	};

	struct ParticleSystemComponent;
	struct TransformComponent;

	class ParticleSystem
	{
	public:
		void Init(ParticleSystemComponent & particleSystemComponent);
		void Update(ParticleSystemComponent& particleSystemComponent, TransformComponent & transfrom, float dt);
		std::vector<Particle> const& GetParticles();
		uint32_t GetParticlesCount();

		static Ref<Texture2D> GetDefaultParticleTexture();

	private:
		void SpawnParticle(TransformComponent & transfrom, Particle& particle, float startSpeed);

		void SyncWithEditor(ParticleSystemComponent& particleSystemComponent);
		void UpdateParticles(glm::vec3 size, glm::vec3 rotation, glm::vec4 color);

	private:
		float		mDuration = 5.f; //lifecycle //min 0.05
		float		mTimer = 0.f; //lifecycle //min 0.05
		float		mSpawnAccumulator = 0.f;
		bool		mLooping = true;
		float		mStartDelay = 0.f;
		float		mStartLifeTime = 5.f;
		float		mStartSpeed = 5.f; //speed of particles
		bool		m_3DStartSizeBool = false; //if false, uniform scale
		bool		mRandomStartSizeBool = false; 
		glm::vec3	mStartSize = glm::vec3{ 1.f }; //if not 3d, use .x for size
		glm::vec3	mStartSize2 = glm::vec3{ 1.f }; //if not 3d, use .x for size
		bool		m_3DStartRotationBool = false; //if false, uniform rotation
		glm::vec3	mStartRotation = glm::vec3{ 0.f }; // if not 3d, use .x for rotation
		bool		mRandomStartColor = false;
		glm::vec4	mStartColor = glm::vec4{ 1.f };
		glm::vec4	mStartColor2 = glm::vec4{ 1.f };
		bool		mEndColorBool = false;
		glm::vec4	mEndColor = glm::vec4{ 1.f };
		float		mGravityModifer = 0.f;
		float		mSimulationSpeed = 1.f; //speed of simulation
		uint32_t	mMaxParticles = 1000;
		uint32_t	mParticlesCount = 0;
		float		mRateOverTime = 10.f;
		float		mAngle = 25.f;
		float		mRadius = 1.f;
		float		mRadiusThickness = 1.f;

		std::vector<Particle> mParticles;

		uint32_t mDeadParticlesCount = 0;
		std::vector<Particle*> mDeadParticles;

		inline static Ref<Texture2D> mDefaultParticle = nullptr;
	};
}

#endif
