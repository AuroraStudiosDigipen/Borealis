/******************************************************************************
/*!
\file       Components.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       November 03, 2024
\brief      Defines the component structs and classes

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Scene/SceneManager.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Components.hpp>
#include <Core/Utils.hpp>

#include <glm/gtc/noise.hpp>

namespace Borealis
{
	glm::mat4 TransformComponent::GetGlobalTransform()
	{
		std::stack<UUID> parentList;
		UUID currID = ParentID;
		while (currID != 0)
		{
			parentList.push(currID); // CBA
			currID = SceneManager::GetActiveScene()->GetEntityByUUID(currID).GetComponent<TransformComponent>().ParentID;
		}
		glm::mat4 globalTransform(1.f);

		while (parentList.size() > 0)
		{
			auto& tc = SceneManager::GetActiveScene()->GetEntityByUUID(parentList.top()).GetComponent<TransformComponent>();
			glm::mat4 localTransform = tc.GetTransform();
			globalTransform = globalTransform * localTransform;
			parentList.pop();
		}
		return globalTransform * GetTransform();
	}

	glm::vec3 TransformComponent::GetGlobalTranslate()
	{
		glm::vec3 globalTranslate(0.f);
		glm::vec3 globalRotation(0.0f);
		glm::vec3 globalScale(1.0f);
		glm::mat4 matrix = GetGlobalTransform();
		Math::MatrixDecomposition(&matrix, &globalTranslate, &globalRotation, &globalScale);
		return globalTranslate;
	}

	glm::vec3 TransformComponent::GetGlobalRotation()
	{
		glm::vec3 globalTranslate(0.f);
		glm::vec3 globalRotation(0.0f);
		glm::vec3 globalScale(1.0f);
		glm::mat4 matrix = GetGlobalTransform();
		Math::MatrixDecomposition(&matrix, &globalTranslate, &globalRotation, &globalScale);
		return globalRotation;
	}

	glm::vec3 TransformComponent::GetGlobalScale()
	{
		glm::vec3 globalTranslate(0.f);
		glm::vec3 globalRotation(0.0f);
		glm::vec3 globalScale(1.0f);
		glm::mat4 matrix = GetGlobalTransform();
		Math::MatrixDecomposition(&matrix, &globalTranslate, &globalRotation, &globalScale);
		return globalScale;
	}

	void TransformComponent::GetGlobalTransformComp(glm::vec3* translate, glm::vec3* rotate, glm::vec3* scale)
	{
		glm::mat4 matrix = GetGlobalTransform();
		Math::MatrixDecomposition(&matrix, translate, rotate, scale);
	}

	void TransformComponent::SetGlobalTransform(glm::mat4 transform)
	{
		if (ParentID != 0)
		{
			Entity parent = SceneManager::GetActiveScene()->GetEntityByUUID(ParentID);
			glm::mat4 parentInverse = glm::inverse(parent.GetComponent<TransformComponent>().GetGlobalTransform());
			glm::mat4 childRelativeTransform = parentInverse * transform;
			Math::MatrixDecomposition(&childRelativeTransform, &Translate, &Rotation, &Scale);
		}
		else
		{
			Math::MatrixDecomposition(&transform , &Translate, &Rotation, &Scale);
		}
	}

	 void TransformComponent::SetParent(Entity entity, Entity parent)
	{
		auto globalMat = GetGlobalTransform();
		ResetParent(entity);
		ParentID = parent.GetUUID();
		SetGlobalTransform(globalMat);
		parent.GetComponent<TransformComponent>().ChildrenID.push_back(entity.GetUUID());
	}

	 void TransformComponent::ResetParent(Entity entity)
	{
		auto mat = GetGlobalTransform();
		Math::MatrixDecomposition(&mat, &Translate, &Rotation, &Scale);

		if (ParentID != 0)
		{
			auto parent = SceneManager::GetActiveScene()->GetEntityByUUID(ParentID);
			auto& parentTC = parent.GetComponent<TransformComponent>();
			auto pos = std::find(parentTC.ChildrenID.begin(), parentTC.ChildrenID.end(), entity.GetUUID());
			parentTC.ChildrenID.erase(pos);
		}

		ParentID = 0;
	}

	 int TransformComponent::GetHierarchyLayer(Entity entity)
	 {
		 

		 if (ParentID == 0)
			 return entity.GetComponent<TagComponent>().mHierarchyLayer;
		 else
		 {
			 auto parent = SceneManager::GetActiveScene()->GetEntityByUUID(ParentID);
			 auto& parentTC = parent.GetComponent<TransformComponent>();
			auto pos = std::find(parentTC.ChildrenID.begin(), parentTC.ChildrenID.end(), entity.GetUUID());
			return (int)std::distance(parentTC.ChildrenID.begin(), pos) + 1;
		 }
	 }

	 void ButtonComponent::onClick()
	 {
		 if (onClickEntity == 0) return;
		auto entity = SceneManager::GetActiveScene()->GetEntityByUUID(onClickEntity);
		if (!entity.HasComponent<ScriptComponent>())
			 return;

		auto& sc = entity.GetComponent<ScriptComponent>();
		auto iter = sc.mScripts.find(onClickClass);
		if (iter == sc.mScripts.end())
			 return; // script class not found

		auto scriptInstance = iter->second;
		auto scriptClass = scriptInstance->GetScriptClass();
		auto method = scriptClass->GetMethod(onClickFunctionName, 0);
		if (method)
			 scriptClass->InvokeMethod(scriptInstance->GetInstance(), method, nullptr);
	 }

	 void ButtonComponent::onRelease()
	 {
		 if (onReleaseEntity == 0) return;
		 auto entity = SceneManager::GetActiveScene()->GetEntityByUUID(onReleaseEntity);
		 if (!entity.HasComponent<ScriptComponent>()) return;

		 auto& sc = entity.GetComponent<ScriptComponent>();
		 auto iter = sc.mScripts.find(onReleaseClass);
		 if (iter == sc.mScripts.end()) return;

		 auto scriptInstance = iter->second;
		 auto scriptClass = scriptInstance->GetScriptClass();
		 auto method = scriptClass->GetMethod(onReleaseFunctionName, 0);
		 if (method)
			 scriptClass->InvokeMethod(scriptInstance->GetInstance(), method, nullptr);
	 }

	 void ButtonComponent::onHover()
	 {
		 if (onHoverEntity == 0) return;
		 auto entity = SceneManager::GetActiveScene()->GetEntityByUUID(onHoverEntity);
		 if (!entity.HasComponent<ScriptComponent>()) return;

		 auto& sc = entity.GetComponent<ScriptComponent>();
		 auto iter = sc.mScripts.find(onHoverClass);
		 if (iter == sc.mScripts.end()) return;

		 auto scriptInstance = iter->second;
		 auto scriptClass = scriptInstance->GetScriptClass();
		 auto method = scriptClass->GetMethod(onHoverFunctionName, 0);
		 if (method)
			 scriptClass->InvokeMethod(scriptInstance->GetInstance(), method, nullptr);
	 }

	 void ParticleSystemComponent::Init()
	 {
		 mParticlesCount = 0;
		 Accumulator = 0.f;
		 Timer = 0.f;
		 mParticles.resize(maxParticles);
		 mDeadParticles.resize(maxParticles);
	 }

	 void ParticleSystemComponent::Start()
	 {
		 if (isActive)
			 return;

		 Timer = 0.f;
		 Accumulator = 0.f;
		 mDeadParticlesCount = 0;
		 mDeadParticles.clear();

		 isActive = true;
	 }

	 void ParticleSystemComponent::Stop()
	 {
		 if (!isActive)
			 return;

		 isActive = false;

		 for (auto& particle : mParticles)
		 {
			 particle.isActive = false;
		 }
		 mParticles.resize(maxParticles);
		 mDeadParticles.resize(maxParticles);
		 mParticlesCount = 0;
		 mDeadParticlesCount = 0;
	 }

	 void ParticleSystemComponent::Update(TransformComponent& transform, float dt)
	 {
		 if (!isActive)
			 return;

		 glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
		 Timer += dt;

		 bool durationExpired = !looping && Timer >= duration;

		 if (!durationExpired)
		 {
			 Accumulator += dt * rateOverTime;
			 if (mParticlesCount > 100000000)
			 {
				 mParticlesCount = 0;
			 }
			 while (mParticlesCount < maxParticles && Accumulator >= 1.f)
			 {
				 Accumulator -= 1.f;
				 if (mDeadParticlesCount > 0)
				 {
					 SpawnParticle(transform, *mDeadParticles[mDeadParticlesCount - 1]);
					 mDeadParticlesCount--;
					 mParticlesCount++;
				 }
				 else
				 {
					 SpawnParticle(transform, mParticles[mParticlesCount]);
					 mParticlesCount++;
				 }
			 }
		 }

		 for (Particle& particle : mParticles)
		 {
			 if (!particle.isActive) continue;
			 particle.life += dt;

			 if (particle.life >= startLifeTime)
			 {
				 particle.isActive = false;
				 mDeadParticles[mDeadParticlesCount] = &particle;
				 mDeadParticlesCount++;
				 mParticlesCount--;
				 continue;
			 }
			 glm::vec3 effectiveGravity = gravity * gravityModifer;
			 particle.startVelocity += effectiveGravity * dt;

			 if (useNoise) {
				 // Compute noise input based on position and time
				 glm::vec3 noiseInput = particle.position * noiseFrequency;
				 noiseInput += glm::vec3(Timer * noiseScrollSpeed);

				 // Sample noise for each axis with offsets to decorrelate
				 glm::vec3 noise;
				 noise.x = glm::perlin(noiseInput + glm::vec3(0.0, 0.0, 0.0));
				 noise.y = glm::perlin(noiseInput + glm::vec3(100.0, 0.0, 0.0));
				 noise.z = glm::perlin(noiseInput + glm::vec3(200.0, 0.0, 0.0));

				 // Calculate acceleration and update velocity
				 glm::vec3 acceleration = noise * noiseStrength;
				 particle.startVelocity += acceleration * dt;
			 }

			 particle.position += particle.startVelocity * dt;

			 if (endColorBool)
			 {
				 float t = particle.life / startLifeTime; // Normalized lifetime (0 to 1)
				 particle.currentColor = particle.startColor + t * (endColor - particle.startColor);
			 }
		 }

		 if (durationExpired && mParticlesCount == 0)
		 {
			 isActive = false;
		 }
	 }

	 std::vector<Particle> const& ParticleSystemComponent::GetParticles()
	 {
		 return mParticles;
	 }

	 uint32_t ParticleSystemComponent::GetParticlesCount()
	 {
		 return mParticlesCount;
	 }

	 Ref<Texture2D> ParticleSystemComponent::GetDefaultParticleTexture()
	 {
		 if (!mDefaultParticle)
		 {
			 mDefaultParticle = Texture2D::GetDefaultTexture();
		 }
		 return mDefaultParticle;
	 }

	 ParticleSystemComponent::ParticleSystemComponent()
	 {
		 Init();
	 }

	 float RandomRange(float min, float max) {
		 return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	 }

	 void ParticleSystemComponent::SpawnParticle(TransformComponent& transform, Particle& particle)
	 {
		 particle.isActive = true;

		 // Get emitter world position
		 glm::vec3 emitterPos = transform.GetGlobalTranslate();

		 // Convert emitter rotation (Euler to Quaternion)
		 glm::vec3 rotation = transform.GetGlobalRotation();
		 rotation = glm::radians(rotation);
		 glm::quat orientation = glm::quat(rotation);

		 if (emitterShape == EmitterShape::Cone)
		 {
			 float randomAngle = static_cast<float>(rand()) / RAND_MAX * glm::two_pi<float>();
			 float randomRadius = radius * glm::sqrt(static_cast<float>(rand()) / RAND_MAX);
			 glm::vec3 spawnOffset = glm::vec3(
				 randomRadius * glm::cos(randomAngle),
				 0.0f,
				 randomRadius * glm::sin(randomAngle)
			 );
			 particle.position = emitterPos + (orientation * spawnOffset);

			 float coneAngle = glm::radians(angle);
			 float u = static_cast<float>(rand()) / RAND_MAX;
			 float v = static_cast<float>(rand()) / RAND_MAX;
			 float theta = v * 2.0f * glm::pi<float>();
			 float phi = glm::acos(1.0f - u * (1.0f - glm::cos(coneAngle)));
			 glm::vec3 direction = glm::vec3(
				 glm::sin(phi) * glm::cos(theta),
				 glm::cos(phi),
				 glm::sin(phi) * glm::sin(theta)
			 );
			 glm::vec3 finalDirection = glm::normalize(orientation * direction);

			 particle.startVelocity = startSpeed * finalDirection;
		 }
		 else if (emitterShape == EmitterShape::Quad)
		 {
			 float randX = static_cast<float>(rand()) / RAND_MAX;
			 float randZ = static_cast<float>(rand()) / RAND_MAX;
			 float halfWidth = transform.GetGlobalScale().x * 0.5f;
			 float halfHeight = transform.GetGlobalScale().y * 0.5f;

			 glm::vec3 spawnOffset = glm::vec3(
				 (randX - 0.5f) * transform.GetGlobalScale().x,
				 0.0f,                    
				 (randZ - 0.5f) * transform.GetGlobalScale().y
			 );

			 particle.position = emitterPos + (orientation * spawnOffset);

			 glm::vec3 quadNormal = orientation * glm::vec3(0.0f, 1.0f, 0.0f); 

			 glm::vec3 finalDirection = glm::normalize(quadNormal);

			 particle.startVelocity = startSpeed * finalDirection;
		 }
		 else if (emitterShape == EmitterShape::Box)
		 {
			 glm::vec3 boxScale = transform.GetGlobalScale();
			 float halfWidth = boxScale.x * 0.5f;
			 float halfHeight = boxScale.y * 0.5f;
			 float halfDepth = boxScale.z * 0.5f;

			 float randX = static_cast<float>(rand()) / RAND_MAX;
			 float randY = static_cast<float>(rand()) / RAND_MAX;
			 float randZ = static_cast<float>(rand()) / RAND_MAX;

			 glm::vec3 spawnOffset = glm::vec3(
				 (randX - 0.5f) * boxScale.x,
				 (randY - 0.5f) * boxScale.y,
				 (randZ - 0.5f) * boxScale.z 
			 );

			 particle.position = emitterPos + (orientation * spawnOffset);

			 float theta = static_cast<float>(rand()) / RAND_MAX * glm::two_pi<float>();
			 float phi = glm::acos(1.0f - 2.0f * static_cast<float>(rand()) / RAND_MAX);

			 glm::vec3 direction = glm::vec3(
				 glm::sin(phi) * glm::cos(theta),
				 glm::sin(phi) * glm::sin(theta),
				 glm::cos(phi)
			 );

			 glm::vec3 finalDirection = glm::normalize(orientation * direction);
			 particle.startVelocity = startSpeed * finalDirection;
		 }

		 if (randomStartColor)
		 {
			 float colorLerp = static_cast<float>(rand()) / RAND_MAX;
			 particle.startColor = glm::mix(startColor, startColor2, colorLerp);
		 }
		 else
		 {
			 particle.startColor = startColor;
		 }

		 particle.currentColor = startColor;

		 if (randomStartSize)
		 {
			 float sizeLerp = static_cast<float>(rand()) / RAND_MAX;
			 particle.startSize = glm::mix(startSize, startSize2, sizeLerp);
		 }
		 else
		 {
			 particle.startSize = startSize;
		 }

		 if (randomStartRotation)
		 {
			 float sizeLerp = static_cast<float>(rand()) / RAND_MAX;
			 particle.startRotation = glm::mix(glm::quat(glm::radians(startRotation)), glm::quat(glm::radians(startRotation2)), sizeLerp);
		 }
		 else
		 {
			 particle.startRotation = glm::quat(glm::radians(startRotation));
		 }

		 particle.life = 0.0f;
	 }

}

