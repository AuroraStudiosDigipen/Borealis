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


namespace Borealis
{
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
		static void PushTransform(unsigned int bodyID, TransformComponent& transform);

		/**
   * \brief Pulls the transform of the specified body from the physics system.
   * \param bodyID The ID of the body.
   * \param transform The transform component of the body.
   */
		static void PullTransform(unsigned int bodyID, TransformComponent& transform);

		/**
   * \brief Adds a square body to the physics system.
   * \param radius The half-extent of the square body.
   * \param position The position of the square body.
   * \param rigidbody The rigid body component of the square body.
   */
		static void addSquareBody(glm::vec3 size, glm::vec3 position, RigidBodyComponent& rigidbody);

			/**
	* \brief Adds a sphere body to the physics system.
	* \param radius The ardius of the sphere body.
	* \param position The position of the sphere body.
	* \param rigidbody The rigid body component of the sphere body.
	*/
		static void addSphereBody(float radius, glm::vec3 position, RigidBodyComponent& rigidbody);

    /**
    * \brief Adds a capsule body to the physics system.
    * \param radius The radius of the capsule body.
    * \param halfHeight The half height of the capsule body.
    * \param position The position of the capsule body.
    * \param rigidbody The rigid body component of the capsule body.
    */
        static void addCapsuleBody(float radius, float halfHeight, glm::vec3 position, RigidBodyComponent& rigidbody);



        /**
        * \brief Adds a body to the physics system.
        * \param position The position of the body.
        * \param rigidbody The rigid body component of the body.
        */
        static void addBody(TransformComponent& transform, RigidBodyComponent& rigidbody, MeshFilterComponent& mesh);

		/**
   * \brief Updates the sphere values of the specified rigid body.
   * \param rigidbody The rigid body component to update.
   */
		static void UpdateSphereValues(RigidBodyComponent& rigidbody);

			/**
	* \brief Updates the box values of the specified rigid body.
	* \param rigidbody The rigid body component to update.
	*/
		static void UpdateBoxValues(RigidBodyComponent& rigidbody);
		
		static void FreeRigidBody(RigidBodyComponent& rigidbody);

		static void calculateBoundingVolume(const Model& model, TransformComponent& transform);

		static glm::vec3 calculateBoxSize(glm::vec3 minExtent, glm::vec3 maxExtent);

		static float calculateSphereRadius(glm::vec3 minExtent, glm::vec3 maxExtent);

		static std::pair<float, float> calculateCapsuleDimensions(glm::vec3 minExtent, glm::vec3 maxExtent);

	};

}

#endif // !PHYSICSSYSTEM_HPP