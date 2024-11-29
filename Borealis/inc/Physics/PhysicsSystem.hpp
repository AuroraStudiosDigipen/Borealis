/******************************************************************************/
/*!
\file		PhysicsSystem.hpp
\author 	Benjamin Lee Zhi Yuan
\par    	email: benjaminzhiyuan.lee\@digipen.edu
\date   	September 28, 2024
\brief		Declares the PhysicsSystem class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef PHYSICSSYSTEM_HPP
#define PHYSICSSYSTEM_HPP

#include <glm/glm.hpp>
#include <Scene/Components.hpp>
#include <Core/BitSet32.hpp>


namespace Borealis
{

	struct RaycastHit
	{
		uint64_t ID;
		uint32_t colliderID;
		float distance;
		glm::vec3 normal;
		glm::vec3 point;
	};

	using CollisionPair = std::pair<UUID, UUID>;

	class Entity;
	class PhysicsSystem
	{
	public:
		/**
   * \brief Initializes the physics system.
   */
		static void Init();

		/**
   * \brief Updates the physics system.
   * \param dt The time step for the update.
   */
		static void Update(float dt);

		/**
   * \brief Frees the resources used by the physics system.
   */
		static void Free();

		/**
   * \brief Pushes the transform of the specified body to the physics system.
   * \param bodyID The ID of the body.
   * \param transform The transform component of the body.
   */
		static void PushTransform(ColliderComponent& collider, TransformComponent& transform, RigidBodyComponent* rigidbody, Entity entity);

		/**
   * \brief Pulls the transform of the specified body from the physics system.
   * \param bodyID The ID of the body.
   * \param transform The transform component of the body.
   */
		static void PullTransform(ColliderComponent& collider, TransformComponent& transform, Entity& entity);

        /**
        * \brief Adds a body to the physics system.
        * \param position The position of the body.
        * \param rigidbody The rigid body component of the body.
        */
        static void addBody(TransformComponent& transform, RigidBodyComponent* rigidbody, ColliderComponent& collider, UUID entityID);
		
		static void EndScene();

		static void FreeRigidBody(ColliderComponent& collider);

		static std::pair<glm::vec3, glm::vec3> calculateBoundingVolume(const Model& model);

		static glm::vec3 calculateBoxSize(glm::vec3 minExtent, glm::vec3 maxExtent);

		static float calculateSphereRadius(glm::vec3 boundingVolume);

		static std::pair<float, float> calculateCapsuleDimensions(glm::vec3 boundingVolume);

		static void AddForce(unsigned int bodyID, glm::vec3 force);

		static void AddTorque(unsigned int bodyID, glm::vec3 torque);

		static void AddImpulse(unsigned int bodyID, glm::vec3 impulse);

		static glm::vec3 GetLinearVelocity(unsigned int bodyID);
		static glm::vec3 GetAngularVelocity(unsigned int bodyID);
		static void SetLinearVelocity(unsigned int bodyID, glm::vec3 velocity);
		static void SetAngularVelocity(unsigned int bodyID, glm::vec3 velocity);
		static glm::vec3 GetPosition(unsigned int bodyID);
		static void SetPosition(unsigned int bodyID, glm::vec3 position);
		static glm::vec3 GetRotation(unsigned int bodyID);
		static void SetRotation(unsigned int bodyID, glm::vec3 rotation);

		static UUID BodyIDToUUID(unsigned int bodyID);
		static bool BodyIDToIsSensor(unsigned int bodyID);

		static std::queue<CollisionPair>& GetCollisionEnterQueue();
		static std::queue<CollisionPair>& GetCollisionPersistQueue();
		static std::queue<CollisionPair>& GetCollisionExitQueue();

		static std::queue<CollisionPair>& GetTriggerEnterQueue();
		static std::queue<CollisionPair>& GetTriggerPersistQueue();
		static std::queue<CollisionPair>& GetTriggerExitQueue();

		static void DrawDebug();

		static void move(ColliderComponent& rigidbody, glm::vec3 motion);

		static void addCharacter(CharacterControlComponent& character, TransformComponent& transform, ColliderComponent& collider, UUID entityID);

		static void FreeCharacter(CharacterControlComponent& character);

		static void HandleInput(float inDeltaTime, CharacterControlComponent& controllerComp);

		static void PrePhysicsUpdate(float dt, void* Character);

		static void PushCharacterTransform(CharacterControlComponent& character, glm::vec3 position, glm::vec3 rotation);

		static void PullCharacterTransform(CharacterControlComponent& character, glm::vec3& position, glm::vec3& rotation);

		static bool RayCast(glm::vec3 origin, glm::vec3 direction, float maxDistance, Bitset32 LayerMask);

		static bool RayCast(glm::vec3 origin, glm::vec3 direction, RaycastHit* hitInfo, float maxDistance, Bitset32 LayerMask);

		static std::vector<RaycastHit> RayCastAll(glm::vec3 origin, glm::vec3 direction, float maxDistance, Bitset32 LayerMask);

		static bool IsCharacterOnGround(void* Character);

	};

}

#endif // !PHYSICSSYSTEM_HPP