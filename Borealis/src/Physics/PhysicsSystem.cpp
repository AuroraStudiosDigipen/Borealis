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

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>


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

	virtual void			OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		//cout << "A contact was added" << endl;
	}

	virtual void			OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		//cout << "A contact was persisted" << endl;
	}

	virtual void			OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
	{
		//cout << "A contact was removed" << endl;
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
	JPH::BodyID object1_id;
	JPH::BodyID object2_id;

};

static PhysicsSystemData sData;
namespace Borealis
{
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

	// Next we can create a rigid body to serve as the floor, we make a large box
	// Create the settings for the collision volume (the shape).
	// Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
	BoxShapeSettings floor_shape_settings(Vec3(100.0f, 1.0f, 100.0f));
	floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

	// Create the shape
	ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

	// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
	BodyCreationSettings floor_settings(floor_shape, RVec3(0.0_r, -3.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);

	// Create the actual rigid body
	Body* floor = sData.body_interface->CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

	// Add it to the world
	sData.body_interface->AddBody(floor->GetID(), EActivation::DontActivate);

	// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
	// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
	// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
	sData.mSystem->OptimizeBroadPhase();
}

	void PhysicsSystem::PushTransform(unsigned int bodyID, TransformComponent& transform)
	{
		// Convert position (glm::vec3 to Jolt's RVec3)
		JPH::RVec3 newPosition = JPH::RVec3(transform.Translate.x, transform.Translate.y, transform.Translate.z);

		// Convert Euler angles (vec3) to quaternion (quat)
		glm::quat rotation = glm::quat(glm::radians(transform.Rotation));  // Assuming Rotation is in degrees

		// Convert glm::quat to Jolt's Quat (JPH::Quat)
		JPH::Quat newRotation = JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w);

		// Set position and rotation in the physics system
		sData.body_interface->SetPosition((BodyID)bodyID, newPosition, EActivation::Activate);
		sData.body_interface->SetRotation((BodyID)bodyID, newRotation, EActivation::Activate);
	}

	void PhysicsSystem::PullTransform(unsigned int bodyID, TransformComponent& transform)
	{
		// Get position from the physics system (JPH::RVec3 to glm::vec3)
		JPH::RVec3 newPosition = sData.body_interface->GetPosition((BodyID)bodyID);
		transform.Translate = glm::vec3(newPosition.GetX(), newPosition.GetY(), newPosition.GetZ());

		// Get rotation from the physics system (JPH::Quat to glm::quat)
		JPH::Quat newRotation = sData.body_interface->GetRotation((BodyID)bodyID);
		glm::quat rotation = glm::quat(newRotation.GetW(), newRotation.GetX(), newRotation.GetY(), newRotation.GetZ());

		// Convert quaternion to Euler angles (quat to vec3) in degrees
		transform.Rotation = glm::degrees(glm::eulerAngles(rotation));  // Euler angles in degrees
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

	void PhysicsSystem::addSquareBody(float radius, glm::vec3 position, RigidBodyComponent& rigidbody) {

		// Create the settings for the collision volume (the shape).
		BoxShapeSettings box_shape_settings(Vec3(radius, radius, radius)); // Use radius as half extents

		// Mark it as embedded (prevent it from being freed when reference count goes to 0)
		box_shape_settings.SetEmbedded();

		// Create the shape
		ShapeSettings::ShapeResult box_shape_result = box_shape_settings.Create();
		ShapeRefC box_shape = box_shape_result.Get(); // Check for errors in a real-world scenario

		// Create the settings for the body itself. Note that here you can also set other properties like restitution/friction.
		BodyCreationSettings box_settings(box_shape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);

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
		BodyCreationSettings sphere_settings(sphere_shape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);

		// Create the actual rigid body
		Body* sphere = sData.body_interface->CreateBody(sphere_settings); // Make sure to handle potential nullptr errors

		// Add it to the world
		sData.body_interface->AddBody(sphere->GetID(), EActivation::Activate);

		// Store the BodyID in the RigidBodyComponent
		rigidbody.bodyID = sphere->GetID().GetIndexAndSequenceNumber();
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
		BoxShapeSettings box_shape_settings(Vec3(rigidbody.radius, rigidbody.radius, rigidbody.radius)); // Use radius as half extents
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
	}
}