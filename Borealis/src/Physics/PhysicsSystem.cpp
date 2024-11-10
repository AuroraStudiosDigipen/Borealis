/******************************************************************************/
/*!
\file		PhysicsSystem.cpp
\author 	Benjamin Lee Zhi Yuan
\par    	email: benjaminzhiyuan.lee\@digipen.edu
\date   	September 28, 2024
\brief		Defines the PhysicsSystem class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "BorealisPCH.hpp"

// STL includes
#include <iostream>
#include <cstdarg>
#include <thread>
#include <chrono>

#include <Physics/PhysicsSystem.hpp>
#include <Core/Utils.hpp>
#include <Jolt/Jolt.h>
#include <Scene/Entity.hpp>
#include <Scene/SceneManager.hpp>


// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterBase.h>


JPH_SUPPRESS_WARNINGS

using namespace std;
using namespace JPH;
using namespace JPH::literals;

// Callback for traces, connect this to your own trace function if you have one
static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	cout << buffer << endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint inLine)
{
	// Print to the TTY
	cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << endl;

	// Breakpoint
	return true;
};

#endif // JPH_ENABLE_ASSERTS

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
namespace Layers
{
	static constexpr ObjectLayer NON_MOVING = 0;
	static constexpr ObjectLayer MOVING = 1;
	static constexpr ObjectLayer NUM_LAYERS = 2;
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
	virtual bool					ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual uint					GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual BroadPhaseLayer			GetBroadPhaseLayer(ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool				ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};


// An example activation listener
class MyBodyActivationListener : public BodyActivationListener
{
public:
	virtual void		OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		//cout << "A body got activated" << endl;
	}

	virtual void		OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		//cout << "A body went to sleep" << endl;
	}
};

namespace Borealis
{
	// An example contact listener
	class MyContactListener : public ContactListener
	{
	public:
		// See: ContactListener
		virtual ValidateResult	OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
		{
			//cout << "Contact validate callback" << endl;

			// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
			return ValidateResult::AcceptAllContactsForThisBodyPair;
		}

		virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;

		virtual void OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;

		virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override;
	};

	struct PhysicsSystemData
	{
		JPH::PhysicsSystem* mSystem;
		JPH::TempAllocatorImpl* temp_allocator;
		JPH::JobSystemThreadPool* job_system;
		JPH::BodyInterface* body_interface;
		BPLayerInterfaceImpl* broad_phase_layer_interface;
		ObjectVsBroadPhaseLayerFilterImpl* object_vs_broadphase_layer_filter;
		ObjectLayerPairFilterImpl* object_vs_object_layer_filter;
		MyContactListener* contact_listener;
		MyBodyActivationListener* body_activation_listener;
		std::queue<CollisionPair> onCollisionPairAddedQueue;
		std::queue<CollisionPair> onCollisionPairRemovedQueue;
		std::queue<CollisionPair> onCollisionPairPersistedQueue;
	};

	static PhysicsSystemData sData;
	static unordered_map<unsigned int, Borealis::UUID> bodyIDMapUUID;

	void MyContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
	{
		sData.onCollisionPairAddedQueue.push({ PhysicsSystem::BodyIDToUUID(inBody1.GetID().GetIndexAndSequenceNumber()), PhysicsSystem::BodyIDToUUID(inBody2.GetID().GetIndexAndSequenceNumber()) });
	}

	void MyContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
	{
		sData.onCollisionPairPersistedQueue.push({ PhysicsSystem::BodyIDToUUID(inBody1.GetID().GetIndexAndSequenceNumber()), PhysicsSystem::BodyIDToUUID(inBody2.GetID().GetIndexAndSequenceNumber()) });
	}

	void MyContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
	{
		sData.onCollisionPairRemovedQueue.push({ PhysicsSystem::BodyIDToUUID(inSubShapePair.GetBody1ID().GetIndexAndSequenceNumber()), PhysicsSystem::BodyIDToUUID(inSubShapePair.GetBody2ID().GetIndexAndSequenceNumber())});
	}

	std::queue<CollisionPair>& PhysicsSystem::GetCollisionEnterQueue() {return sData.onCollisionPairAddedQueue; }
	std::queue<CollisionPair>& PhysicsSystem::GetCollisionPersistQueue() { return sData.onCollisionPairPersistedQueue; }
	std::queue<CollisionPair>& PhysicsSystem::GetCollisionExitQueue() { return sData.onCollisionPairRemovedQueue; }

void PhysicsSystem::Init()
{
	sData.broad_phase_layer_interface = new BPLayerInterfaceImpl();
	sData.object_vs_broadphase_layer_filter = new ObjectVsBroadPhaseLayerFilterImpl();
	sData.object_vs_object_layer_filter = new ObjectLayerPairFilterImpl();
	sData.contact_listener = new MyContactListener();
	sData.body_activation_listener = new MyBodyActivationListener();
	// Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
	// This needs to be done before any other Jolt function is called.
	RegisterDefaultAllocator();

	// Install trace and assert callbacks
	Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

		// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
		// It is not directly used in this example but still required.
	Factory::sInstance = new Factory();

	// Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
	// If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
	// If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
	RegisterTypes();

	// We need a temp allocator for temporary allocations during the physics update. We're
	// pre-allocating 10 MB to avoid having to do allocations during the physics update.
	// B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
	// If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
	// malloc / free.
	sData.temp_allocator = new TempAllocatorImpl(10 * 1024 * 1024);
	sData.job_system = new JobSystemThreadPool(2048, 8, thread::hardware_concurrency() - 1);

	// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
	const uint cMaxBodies = 1024;

	// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
	const uint cNumBodyMutexes = 0;

	// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
	// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
	// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
	const uint cMaxBodyPairs = 1024;

	// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
	// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
	const uint cMaxContactConstraints = 1024;

	// Now we can create the actual physics system.
	sData.mSystem = new JPH::PhysicsSystem();
	sData.mSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *sData.broad_phase_layer_interface, *sData.object_vs_broadphase_layer_filter, *sData.object_vs_object_layer_filter);
	
	// A body activation listener gets notified when bodies activate and go to sleep
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	
	sData.mSystem->SetBodyActivationListener(sData.body_activation_listener);

	// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	sData.mSystem->SetContactListener(sData.contact_listener);

	// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
	// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
	sData.body_interface = &sData.mSystem->GetBodyInterface();

	// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
	// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
	// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
	sData.mSystem->OptimizeBroadPhase();
}

	void PhysicsSystem::PushTransform(RigidBodyComponent& rigidbody, TransformComponent& transform, Entity entity)
	{
		auto entityTransform = TransformComponent::GetGlobalTransform(entity);
		auto modelCenter = rigidbody.modelCenter;
		auto actualCenterVec4 = entityTransform * glm::vec4(modelCenter, 1.0f);
		auto actualCenter = glm::vec3(actualCenterVec4.x, actualCenterVec4.y, actualCenterVec4.z);
		// Convert position (glm::vec3 to Jolt's RVec3)
		JPH::RVec3 newPosition = JPH::RVec3(actualCenter.x, actualCenter.y, actualCenter.z);

		// Convert Euler angles (vec3) to quaternion (quat)
		glm::quat rotation = glm::quat(glm::radians(transform.Rotation));  // Assuming Rotation is in degrees

		// Convert glm::quat to Jolt's Quat (JPH::Quat)
		JPH::Quat newRotation = JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w);

		// Set position and rotation in the physics system
		sData.body_interface->SetPosition((BodyID)rigidbody.bodyID, newPosition, EActivation::Activate);
		sData.body_interface->SetRotation((BodyID)rigidbody.bodyID, newRotation, EActivation::Activate);
	}

	void PhysicsSystem::PullTransform(RigidBodyComponent& rigidbody, TransformComponent& transform, Entity& entity)
	{
		// Get position from the physics system (JPH::RVec3 to glm::vec3)
		JPH::RVec3 newPosition = sData.body_interface->GetPosition((BodyID)rigidbody.bodyID);

		transform.Translate = glm::vec3(newPosition.GetX(), newPosition.GetY(), newPosition.GetZ()) - rigidbody.modelCenter;
		
		// Get rotation from the physics system (JPH::Quat to glm::quat)
		JPH::Quat newRotation = sData.body_interface->GetRotation((BodyID)rigidbody.bodyID);
		glm::quat rotation = glm::quat(newRotation.GetW(), newRotation.GetX(), newRotation.GetY(), newRotation.GetZ());

		// Convert quaternion to Euler angles (quat to vec3) in degrees
		transform.Rotation = glm::degrees(glm::eulerAngles(rotation));  // Euler angles in degrees


		//TODO
		/*if (transform.ParentID != 0)
		{
			auto parentEntity = SceneManager::GetEntityByUUID(transform.ParentID);
			auto parentTransform = TransformComponent::GetGlobalTransform(parentEntity);
			auto localTransform = glm::inverse(parentTransform) * glm::vec4(transform.transform);
		}*/
	}

	void PhysicsSystem::Update(float dt)
	{
		sData.mSystem->Update(dt, 1, sData.temp_allocator, sData.job_system);
	}

	void PhysicsSystem::Free()
	{
		delete sData.body_activation_listener;
		delete sData.broad_phase_layer_interface;
		delete sData.object_vs_broadphase_layer_filter;
		delete sData.object_vs_object_layer_filter;
		delete sData.contact_listener;
		delete sData.temp_allocator;
		delete sData.job_system;
		delete sData.mSystem;

		delete Factory::sInstance;
	}

	UUID PhysicsSystem::BodyIDToUUID(unsigned int bodyID)
	{
		return bodyIDMapUUID[bodyID];
	}

	void PhysicsSystem::calculateBoundingVolume(const Model& model, TransformComponent& transform, RigidBodyComponent& rigidbody)
	{
		glm::vec3 minExtent{}, maxExtent{};

		for (const auto& mesh : model.mMeshes)
		{
			for (const auto& vertex : mesh.GetVertices())
			{
				// Update min and max extents for each axis
				minExtent.x = std::min(minExtent.x, vertex.Position.x);
				minExtent.y = std::min(minExtent.y, vertex.Position.y);
				minExtent.z = std::min(minExtent.z, vertex.Position.z);
														   
				maxExtent.x = std::max(maxExtent.x, vertex.Position.x);
				maxExtent.y = std::max(maxExtent.y, vertex.Position.y);
				maxExtent.z = std::max(maxExtent.z, vertex.Position.z);
			}
		}
		minExtent *= transform.Scale;
		maxExtent *= transform.Scale;

		glm::vec3 boundingVolumeCenter = (minExtent + maxExtent) * 0.5f;
		rigidbody.modelCenter = boundingVolumeCenter;

		transform.offset = transform.Translate - boundingVolumeCenter;

		transform.minExtent = minExtent;
		transform.maxExtent = maxExtent;

		/*cout << "Min Extent: " << minExtent.x << ", " << minExtent.y << ", " << minExtent.z << endl;
		cout << "Max Extent: " << maxExtent.x << ", " << maxExtent.y << ", " << maxExtent.z << endl;*/
	}

	glm::vec3 PhysicsSystem::calculateBoxSize(glm::vec3 minExtent, glm::vec3 maxExtent)
	{
		// Calculate the size of the box
		return maxExtent - minExtent;
	}

	float PhysicsSystem::calculateSphereRadius(glm::vec3 minExtent, glm::vec3 maxExtent)
	{
		glm::vec3 dimensions = maxExtent - minExtent;
		return (glm::length(dimensions) * 0.5f);
	}

	std::pair<float, float> PhysicsSystem::calculateCapsuleDimensions(glm::vec3 minExtent, glm::vec3 maxExtent)
	{
		// Calculate the extents of the bounding box
		glm::vec3 extent = maxExtent - minExtent;

		// Radius is half of the smallest width in the X or Z dimensions
		float radius = 0.5f * std::min(extent.x, extent.z);

		// Half-height is half of the height (Y dimension), minus the radius
		float halfHeight = 0.5f * extent.y - radius;

		return { radius, halfHeight };
	}

	void PhysicsSystem::move(RigidBodyComponent& rigidbody, glm::vec3 motion)
	{
		// Get the current position of the body
		JPH::RVec3 position = sData.body_interface->GetPosition((BodyID)rigidbody.bodyID);

		// Add the motion to the position
		position += JPH::RVec3(motion.x, motion.y, motion.z);

		// Set the new position
		sData.body_interface->SetPosition((BodyID)rigidbody.bodyID, position, EActivation::Activate);
	}

	void PhysicsSystem::addBody(TransformComponent& transform, RigidBodyComponent& rigidbody, MeshFilterComponent& mesh, UUID entityID) {
		ShapeRefC shape;
		ShapeSettings::ShapeResult shape_result;

		bodyIDMapUUID[rigidbody.bodyID] = entityID;

		calculateBoundingVolume(*mesh.Model, transform, rigidbody);

		switch (rigidbody.shape) {
		case RigidBodyType::Box: {
			// Create box shape settings
			glm::vec3 size = calculateBoxSize(transform.minExtent, transform.maxExtent) * 0.5f;
			rigidbody.size = size;
			BoxShapeSettings box_shape_settings(Vec3(size.x, size.y, size.z));
			box_shape_settings.SetEmbedded();
			shape_result = box_shape_settings.Create();
			shape = shape_result.Get();
			break;
		}
		case RigidBodyType::Sphere: {
			// Create sphere shape settings
			float radius = calculateSphereRadius(transform.minExtent,transform.maxExtent);
			SphereShapeSettings sphere_shape_settings(radius);
			sphere_shape_settings.SetEmbedded();
			shape_result = sphere_shape_settings.Create();
			shape = shape_result.Get();
			break;
		}
		case RigidBodyType::Capsule: {
			// Create capsule shape settings
			auto [radius, halfHeight] = calculateCapsuleDimensions(transform.minExtent, transform.maxExtent);
			CapsuleShapeSettings capsule_shape_settings(radius, halfHeight);
			capsule_shape_settings.SetEmbedded();
			shape_result = capsule_shape_settings.Create();
			shape = shape_result.Get();
			break;
		}
		default:
			// Handle error for unsupported shape type
			return;
		}

		if (!shape) {
			// Handle error (e.g., failed to create shape)
			return;
		}

		// Create the settings for the body itself, including other properties like restitution and friction
		BodyCreationSettings body_settings(shape, RVec3(transform.Translate.x, transform.Translate.y, transform.Translate.z), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);

		if (rigidbody.movement == MovementType::Dynamic) {
			body_settings.mMotionType = EMotionType::Dynamic;
			body_settings.mObjectLayer = Layers::MOVING;
		}

		if (rigidbody.movement == MovementType::Kinematic)
		{
			body_settings.mMotionType = EMotionType::Kinematic;
			body_settings.mObjectLayer = Layers::MOVING;
		}

		body_settings.mFriction = rigidbody.friction;
		body_settings.mRestitution = rigidbody.bounciness;

		// Create the actual rigid body
		Body* body = sData.body_interface->CreateBody(body_settings); // Handle nullptr in a real scenario
		if (!body) {
			// Handle error (e.g., failed to create body)
			return;
		}

		// Add it to the world
		sData.body_interface->AddBody(body->GetID(), EActivation::Activate);

		// Store the BodyID in the RigidBodyComponent
		rigidbody.bodyID = body->GetID().GetIndexAndSequenceNumber();
	}

	void PhysicsSystem::addSquareBody(glm::vec3 size, glm::vec3 position, RigidBodyComponent& rigidbody) {

		// Create the settings for the collision volume (the shape).
		BoxShapeSettings box_shape_settings(Vec3(size.x, size.y, size.z)); // Use radius as half extents

		// Mark it as embedded (prevent it from being freed when reference count goes to 0)
		box_shape_settings.SetEmbedded();

		// Create the shape
		ShapeSettings::ShapeResult box_shape_result = box_shape_settings.Create();
		ShapeRefC box_shape = box_shape_result.Get(); // Check for errors in a real-world scenario

		// Create the settings for the body itself. Note that here you can also set other properties like restitution/friction.
		BodyCreationSettings box_settings(box_shape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
		box_settings.mFriction = rigidbody.friction;
		box_settings.mRestitution = rigidbody.bounciness;
		if(rigidbody.dynamicBody)
		{
			box_settings.mMotionType = EMotionType::Dynamic;
			box_settings.mObjectLayer = Layers::MOVING;
		}
		// Create the actual rigid body
		Body* box = sData.body_interface->CreateBody(box_settings); // Make sure to handle potential nullptr errors

		// Add it to the world
		sData.body_interface->AddBody(box->GetID(), EActivation::Activate);

		// Store the BodyID in the RigidBodyComponent
		rigidbody.bodyID = box->GetID().GetIndexAndSequenceNumber();
	}


	void PhysicsSystem::addSphereBody(float radius, glm::vec3 position, RigidBodyComponent& rigidbody)
	{
		// Create the settings for the collision volume (the shape).
		SphereShapeSettings sphere_shape_settings(radius); // Use radius as half extents

		// Mark it as embedded (prevent it from being freed when reference count goes to 0)
		sphere_shape_settings.SetEmbedded();

		// Create the shape
		ShapeSettings::ShapeResult sphere_shape_result = sphere_shape_settings.Create();
		ShapeRefC sphere_shape = sphere_shape_result.Get(); // Check for errors in a real-world scenario

		// Create the settings for the body itself. Note that here you can also set other properties like restitution/friction.
		BodyCreationSettings sphere_settings(sphere_shape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
		sphere_settings.mFriction = rigidbody.friction;
		sphere_settings.mRestitution = rigidbody.bounciness;
		if (rigidbody.dynamicBody)
		{
			sphere_settings.mMotionType = EMotionType::Dynamic;
			sphere_settings.mObjectLayer = Layers::MOVING;
		}

		// Create the actual rigid body
		Body* sphere = sData.body_interface->CreateBody(sphere_settings); // Make sure to handle potential nullptr errors

		// Add it to the world
		sData.body_interface->AddBody(sphere->GetID(), EActivation::Activate);

		// Store the BodyID in the RigidBodyComponent
		rigidbody.bodyID = sphere->GetID().GetIndexAndSequenceNumber();
	}

	void PhysicsSystem::addCapsuleBody(float radius, float halfHeight, glm::vec3 position, RigidBodyComponent& rigidbody)
	{
		// Create the settings for the collision volume (the shape).
		CapsuleShapeSettings capsule_shape_settings(radius, halfHeight); // Use radius as half extents

		// Mark it as embedded (prevent it from being freed when reference count goes to 0)
		capsule_shape_settings.SetEmbedded();

		// Create the shape
		ShapeSettings::ShapeResult capsule_shape_result = capsule_shape_settings.Create();
		ShapeRefC capsule_shape = capsule_shape_result.Get(); // Check for errors in a real-world scenario

		// Create the settings for the body itself. Note that here you can also set other properties like restitution/friction.
		BodyCreationSettings capsule_settings(capsule_shape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
		if (rigidbody.dynamicBody)
		{
			capsule_settings.mMotionType = EMotionType::Dynamic;
			capsule_settings.mObjectLayer = Layers::MOVING;
		}
		// Create the actual rigid body
		Body* capsule = sData.body_interface->CreateBody(capsule_settings); // Make sure to handle potential nullptr errors

		// Add it to the world
		sData.body_interface->AddBody(capsule->GetID(), EActivation::Activate);

		// Store the BodyID in the RigidBodyComponent
		rigidbody.bodyID = capsule->GetID().GetIndexAndSequenceNumber();
	}

	void PhysicsSystem::UpdateSphereValues(RigidBodyComponent& rigidbody)
	{
		// Create the settings for the collision volume (the shape).
		SphereShapeSettings sphere_shape_settings(rigidbody.radius);
		sphere_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

		// Create the shape
		ShapeSettings::ShapeResult sphere_shape_result = sphere_shape_settings.Create();
		ShapeRefC sphere_shape = sphere_shape_result.Get(); // We don't expect an error here, but you can check sphere_shape_result for HasError() / GetError()

		// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
		sData.body_interface->SetShape(JPH::BodyID(rigidbody.bodyID), sphere_shape, true, EActivation::Activate);
	}

	void PhysicsSystem::UpdateBoxValues(RigidBodyComponent& rigidbody)
	{		
		// Create the settings for the collision volume (the shape).
		BoxShapeSettings box_shape_settings(Vec3(rigidbody.size.x, rigidbody.size.y, rigidbody.size.z)); // Use radius as half extents
		box_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

		// Create the shape
		ShapeSettings::ShapeResult box_shape_result = box_shape_settings.Create();
		ShapeRefC box_shape = box_shape_result.Get(); // We don't expect an error here, but you can check sphere_shape_result for HasError() / GetError()

		// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
		sData.body_interface->SetShape(JPH::BodyID(rigidbody.bodyID), box_shape, true, EActivation::Activate);
	}

	void PhysicsSystem::FreeRigidBody(RigidBodyComponent& rigidbody)
	{
		sData.body_interface->RemoveBody(JPH::BodyID(rigidbody.bodyID));
		bodyIDMapUUID.erase(rigidbody.bodyID);
	}
}