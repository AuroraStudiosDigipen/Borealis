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

// STL includes+
#include <iostream>
#include <cstdarg>
#include <thread>
#include <chrono>
#include <mutex>

#include <Physics/PhysicsSystem.hpp>
#include <Core/Utils.hpp>
#include <Jolt/Jolt.h>
#include <Scene/Entity.hpp>
#include <Scene/SceneManager.hpp>
#include <Core/LayerList.hpp>
#include <Core/BitSet32.hpp>


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
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Character/CharacterBase.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include <Graphics/Renderer2D.hpp>



JPH_SUPPRESS_WARNINGS
#undef AddJob

using namespace std;
using namespace JPH;
using namespace JPH::literals;

static std::mutex persistMutex;
static std::mutex enterMutex;
static std::mutex exitMutex;
static std::mutex bodyMutex;
static std::mutex characterMutex;

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
	static constexpr ObjectLayer MOVING = 31;
	static constexpr ObjectLayer NUM_LAYERS = 32;
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
			return true;
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
		for (int i = 1; i < 31; i++)
		{
			mObjectToBroadPhase[i] = BroadPhaseLayers::MOVING;
		}
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
	virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			return true;
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

	//Debug Renderer
	class MyDebugRenderer : public JPH::DebugRendererSimple
	{
	public:
		virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override
		{
			// Implement
			Renderer2D::DrawLine(glm::vec3(inFrom.GetX(),inFrom.GetY(),inFrom.GetZ()), glm::vec3(inTo.GetX(),inTo.GetY(),inTo.GetZ()), glm::vec4(inColor.r,inColor.g,inColor.b,inColor.a));
			//Renderer2D::DrawLine(glm::vec3(0.f), glm::vec3(100.f), glm::vec4(0.f,1.f,0.f,1.f));
		}

		virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override
		{
			// Implement
			BOREALIS_CORE_ERROR("DrawTriangle not implemented");
		}

		virtual void DrawText3D(JPH::RVec3Arg inPosition, const string_view& inString, JPH::ColorArg inColor, float inHeight) override
		{
			// Implement
			BOREALIS_CORE_ERROR("DrawText3D not implemented");
		}
	};

	struct PhysicsSystemData
	{
		JPH::PhysicsSystem* mSystem;
		JPH::TempAllocatorImpl* temp_allocator;
		JPH::JobSystemThreadPool* job_system;
		JPH::BodyInterface* body_interface;
		JPH::JobSystem::Barrier* barrier;
		MyDebugRenderer* debug_renderer;
		BodyManager::DrawSettings draw_settings;
		BPLayerInterfaceImpl* broad_phase_layer_interface;
		ObjectVsBroadPhaseLayerFilterImpl* object_vs_broadphase_layer_filter;
		ObjectLayerPairFilterImpl* object_vs_object_layer_filter;
		MyContactListener* contact_listener;
		MyBodyActivationListener* body_activation_listener;

		std::queue<CollisionPair> onCollisionPairAddedQueue;
		std::queue<CollisionPair> onCollisionPairRemovedQueue;
		std::queue<CollisionPair> onCollisionPairPersistedQueue;

		std::queue<CollisionPair> onTriggerPairAddedQueue;
		std::queue<CollisionPair> onTriggerPairRemovedQueue;
		std::queue<CollisionPair> onTriggerPairPersistedQueue;
	};

	static PhysicsSystemData sPhysicsData;
	static unordered_map<unsigned int, Borealis::UUID> bodyIDMapUUID;
	static unordered_map<unsigned int, bool> bodySensorMap;

	void MyContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
	{
		std::lock_guard<std::mutex>lock(enterMutex);
		if (inBody1.IsSensor() || inBody2.IsSensor())
		{
			sPhysicsData.onTriggerPairAddedQueue.push({ PhysicsSystem::BodyIDToUUID(inBody1.GetID().GetIndexAndSequenceNumber()), PhysicsSystem::BodyIDToUUID(inBody2.GetID().GetIndexAndSequenceNumber()) });
		}
		else
		{
			sPhysicsData.onCollisionPairAddedQueue.push({ PhysicsSystem::BodyIDToUUID(inBody1.GetID().GetIndexAndSequenceNumber()), PhysicsSystem::BodyIDToUUID(inBody2.GetID().GetIndexAndSequenceNumber()) });
		}
	}

	void MyContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
	{
		std::lock_guard<std::mutex>lock(persistMutex);

		if (inBody1.IsSensor() || inBody2.IsSensor())
		{
			sPhysicsData.onTriggerPairPersistedQueue.push({ PhysicsSystem::BodyIDToUUID(inBody1.GetID().GetIndexAndSequenceNumber()), PhysicsSystem::BodyIDToUUID(inBody2.GetID().GetIndexAndSequenceNumber()) });
		}
		else
		{
			sPhysicsData.onCollisionPairPersistedQueue.push({ PhysicsSystem::BodyIDToUUID(inBody1.GetID().GetIndexAndSequenceNumber()), PhysicsSystem::BodyIDToUUID(inBody2.GetID().GetIndexAndSequenceNumber()) });
		}
	}

	void MyContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
	{
		std::lock_guard<std::mutex>lock(exitMutex);

		if (PhysicsSystem::BodyIDToIsSensor(inSubShapePair.GetBody1ID().GetIndexAndSequenceNumber()) || PhysicsSystem::BodyIDToIsSensor(inSubShapePair.GetBody2ID().GetIndexAndSequenceNumber()))
		{
			sPhysicsData.onTriggerPairRemovedQueue.push({ PhysicsSystem::BodyIDToUUID(inSubShapePair.GetBody1ID().GetIndexAndSequenceNumber()), PhysicsSystem::BodyIDToUUID(inSubShapePair.GetBody2ID().GetIndexAndSequenceNumber()) });
		}
		else
		{
			sPhysicsData.onCollisionPairRemovedQueue.push({ PhysicsSystem::BodyIDToUUID(inSubShapePair.GetBody1ID().GetIndexAndSequenceNumber()), PhysicsSystem::BodyIDToUUID(inSubShapePair.GetBody2ID().GetIndexAndSequenceNumber()) });
		}
		
	}

	std::queue<CollisionPair>& PhysicsSystem::GetCollisionEnterQueue() {return sPhysicsData.onCollisionPairAddedQueue; }
	std::queue<CollisionPair>& PhysicsSystem::GetCollisionPersistQueue() { return sPhysicsData.onCollisionPairPersistedQueue; }
	std::queue<CollisionPair>& PhysicsSystem::GetCollisionExitQueue() { return sPhysicsData.onCollisionPairRemovedQueue; }

	std::queue<CollisionPair>& PhysicsSystem::GetTriggerEnterQueue() { return sPhysicsData.onTriggerPairAddedQueue; }
	std::queue<CollisionPair>& PhysicsSystem::GetTriggerPersistQueue() { return sPhysicsData.onTriggerPairPersistedQueue; }
	std::queue<CollisionPair>& PhysicsSystem::GetTriggerExitQueue() { return sPhysicsData.onTriggerPairRemovedQueue; }

	void PhysicsSystem::EndScene()
	{
		while (!sPhysicsData.onCollisionPairAddedQueue.empty())
		{
			sPhysicsData.onCollisionPairAddedQueue.pop();
		}
		while (!sPhysicsData.onCollisionPairPersistedQueue.empty())
		{
			sPhysicsData.onCollisionPairPersistedQueue.pop();
		}
		while (!sPhysicsData.onCollisionPairRemovedQueue.empty())
		{
			sPhysicsData.onCollisionPairRemovedQueue.pop();
		}
		while (!sPhysicsData.onTriggerPairAddedQueue.empty())
		{
			sPhysicsData.onTriggerPairAddedQueue.pop();
		}
		while (!sPhysicsData.onTriggerPairPersistedQueue.empty())
		{
			sPhysicsData.onTriggerPairPersistedQueue.pop();
		}
		while (!sPhysicsData.onTriggerPairRemovedQueue.empty())
		{
			sPhysicsData.onTriggerPairRemovedQueue.pop();
		}
		bodyIDMapUUID.clear();
		bodySensorMap.clear();
	}

	void PhysicsSystem::Init()
{
	sPhysicsData.broad_phase_layer_interface = new BPLayerInterfaceImpl();
	sPhysicsData.object_vs_broadphase_layer_filter = new ObjectVsBroadPhaseLayerFilterImpl();
	sPhysicsData.object_vs_object_layer_filter = new ObjectLayerPairFilterImpl();
	sPhysicsData.contact_listener = new MyContactListener();
	sPhysicsData.body_activation_listener = new MyBodyActivationListener();
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
	sPhysicsData.temp_allocator = new TempAllocatorImpl(10 * 1024 * 1024);
	sPhysicsData.job_system = new JobSystemThreadPool(2048, 8, thread::hardware_concurrency() - 1);

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
	sPhysicsData.mSystem = new JPH::PhysicsSystem();
	sPhysicsData.mSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *sPhysicsData.broad_phase_layer_interface, *sPhysicsData.object_vs_broadphase_layer_filter, *sPhysicsData.object_vs_object_layer_filter);
	// A body activation listener gets notified when bodies activate and go to sleep
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	
	sPhysicsData.mSystem->SetBodyActivationListener(sPhysicsData.body_activation_listener);

	// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	sPhysicsData.mSystem->SetContactListener(sPhysicsData.contact_listener);

	// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
	// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
	sPhysicsData.body_interface = &sPhysicsData.mSystem->GetBodyInterface();

	// Create a debug renderer
	sPhysicsData.debug_renderer = new MyDebugRenderer();
	sPhysicsData.draw_settings.mDrawShapeWireframe = true;

	// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
	// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
	// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
	sPhysicsData.mSystem->OptimizeBroadPhase();
}

	void PhysicsSystem::DrawDebug()
	{
		sPhysicsData.mSystem->DrawBodies(sPhysicsData.draw_settings, sPhysicsData.debug_renderer);
	}

	void PhysicsSystem::PushTransform(ColliderComponent& collider, TransformComponent& transform, RigidbodyComponent* rigidbody)
	{
		auto entityTransform = transform.GetGlobalTransform();
		auto modelCenter = collider.center;
		auto actualCenterVec4 = entityTransform * glm::vec4(modelCenter, 1.0f);
		auto actualCenter = glm::vec3(actualCenterVec4.x, actualCenterVec4.y, actualCenterVec4.z);
	
		// Convert position (glm::vec3 to Jolt's RVec3)
		JPH::RVec3 newPosition = JPH::RVec3(actualCenter.x, actualCenter.y, actualCenter.z);

		// Convert Euler angles (vec3) to quaternion (quat)
		glm::quat rotation = glm::quat(glm::radians(transform.Rotation));  // Assuming Rotation is in degrees

		// Convert glm::quat to Jolt's Quat (JPH::Quat)
		JPH::Quat newRotation = JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w);

		// Set position and rotation in the physics system
		sPhysicsData.body_interface->SetPosition((BodyID)collider.bodyID, newPosition, EActivation::Activate);
		if (rigidbody->movement != MovementType::Kinematic)
		{
			sPhysicsData.body_interface->SetRotation((BodyID)collider.bodyID, newRotation, EActivation::Activate);
		}

		//Set motion type if necceassary
		if (rigidbody)
		{
			if (rigidbody->movement == MovementType::Kinematic)
			{
				sPhysicsData.body_interface->SetMotionType((BodyID)collider.bodyID, EMotionType::Kinematic, EActivation::Activate);
			}
			else if (rigidbody->movement == MovementType::Static)
			{
				sPhysicsData.body_interface->SetMotionType((BodyID)collider.bodyID, EMotionType::Static, EActivation::Activate);

			}
			else if (rigidbody->movement == MovementType::Dynamic)
			{
				sPhysicsData.body_interface->SetMotionType((BodyID)collider.bodyID, EMotionType::Dynamic, EActivation::Activate);

			}
		}
	}

	void PhysicsSystem::PullTransform(ColliderComponent& collider, TransformComponent& transform)
	{
		// Get position from the physics system (JPH::RVec3 to glm::vec3)

		JPH::RVec3 newPosition = sPhysicsData.body_interface->GetPosition((BodyID)collider.bodyID);
		glm::vec3 newTranslate = glm::vec3(newPosition.GetX(), newPosition.GetY(), newPosition.GetZ());
		// Get rotation from the physics system (JPH::Quat to glm::quat)
		JPH::Quat newRotation = sPhysicsData.body_interface->GetRotation((BodyID)collider.bodyID);
		glm::quat rotation = glm::quat(newRotation.GetW(), newRotation.GetX(), newRotation.GetY(), newRotation.GetZ());
		// Convert quaternion to Euler angles (quat to vec3) in degrees
		glm::vec3 newRotate = glm::degrees(glm::eulerAngles(rotation));  // Euler angles in degrees
		glm::vec3 newScale = transform.GetGlobalScale();

		glm::mat4 rotationMatrix = glm::mat4(glm::quat(glm::radians(newRotate)));
		glm::mat4 translationMatrix = glm::mat4(1.f);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), newScale);
		glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		auto offsetCenter = modelMatrix * glm::vec4(collider.center, 1.0f);

		newTranslate -= glm::vec3(offsetCenter.x, offsetCenter.y, offsetCenter.z);

		glm::mat4 newTransform = glm::translate(glm::mat4(1.0f), newTranslate) * rotationMatrix * scaleMatrix;


		if(transform.ParentID == 0)
		{
			transform.Translate = newTranslate;
			transform.Rotation = newRotate;
			transform.Scale = newScale;
		}
		else
		{
			auto parentEntity = SceneManager::GetActiveScene()->GetEntityByUUID(transform.ParentID);
			auto parentTransform = parentEntity.GetComponent<TransformComponent>().GetGlobalTransform();
		
				auto localTransform = glm::inverse(parentTransform) * newTransform;
				Math::MatrixDecomposition(&localTransform, &transform.Translate, &transform.Rotation, &transform.Scale);
			

		}
	}

	void PhysicsSystem::PushCharacterTransform(CharacterControllerComponent& character, glm::vec3 position, glm::vec3 rotation)
	{
		// Convert position (glm::vec3 to Jolt's RVec3)
		JPH::RVec3 newPosition = JPH::RVec3(position.x, position.y, position.z);

		// Convert Euler angles (vec3) to quaternion (quat)
		glm::quat rotationQuat = glm::quat(glm::radians(rotation));  // Assuming Rotation is in degrees

		// Convert glm::quat to Jolt's Quat (JPH::Quat)
		JPH::Quat newRotation = JPH::Quat(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w);

		// Set position and rotation in the physics system
		reinterpret_cast<CharacterVirtual*>(character.controller)->SetPosition(newPosition);
		reinterpret_cast<CharacterVirtual*>(character.controller)->SetRotation(newRotation);
	}

	void PhysicsSystem::PullCharacterTransform(CharacterControllerComponent& character, glm::vec3& position, glm::vec3& rotation)
	{
		// Get position from the physics system (JPH::RVec3 to glm::vec3)
		JPH::RVec3 newPosition = reinterpret_cast<CharacterVirtual*>(character.controller)->GetPosition();
		position = glm::vec3(newPosition.GetX(), newPosition.GetY(), newPosition.GetZ());

		// Get rotation from the physics system (JPH::Quat to glm::quat)
		JPH::Quat newRotation = reinterpret_cast<CharacterVirtual*>(character.controller)->GetRotation();
		glm::quat rotationQuat = glm::quat(newRotation.GetW(), newRotation.GetX(), newRotation.GetY(), newRotation.GetZ());

		// Convert quaternion to Euler angles (quat to vec3) in degrees
		rotation = glm::degrees(glm::eulerAngles(rotationQuat));  // Euler angles in degrees

	}

	bool PhysicsSystem::shouldDebugDraw = false;

	bool PhysicsSystem::DebugDrawGet() 
	{
		return shouldDebugDraw;
	}

	void PhysicsSystem::DebugDrawSet(bool value)
	{
		shouldDebugDraw = value;
	}

	void PhysicsSystem::StartJobQueue()
	{
		sPhysicsData.barrier = sPhysicsData.job_system->CreateBarrier();
	}

	void PhysicsSystem::PushJob(std::string name, std::function<void()> function, uint32_t dependenciesNum)
	{
		sPhysicsData.barrier->AddJob(sPhysicsData.job_system->CreateJob(name.c_str(), JPH::Color::sRed, function, dependenciesNum));
	}

	void PhysicsSystem::EndJobQueue()
	{
		sPhysicsData.job_system->WaitForJobs(sPhysicsData.barrier);
		sPhysicsData.job_system->DestroyBarrier(sPhysicsData.barrier);
	}

	void PhysicsSystem::Update(float dt)
	{
		sPhysicsData.mSystem->Update(dt, 1, sPhysicsData.temp_allocator, sPhysicsData.job_system);
	}

	void PhysicsSystem::Free()
	{
		delete sPhysicsData.body_activation_listener;
		delete sPhysicsData.broad_phase_layer_interface;
		delete sPhysicsData.object_vs_broadphase_layer_filter;
		delete sPhysicsData.object_vs_object_layer_filter;
		delete sPhysicsData.contact_listener;
		delete sPhysicsData.temp_allocator;
		delete sPhysicsData.job_system;
		delete sPhysicsData.debug_renderer;
		delete sPhysicsData.mSystem;
		UnregisterTypes();
		delete Factory::sInstance;
		Factory::sInstance = nullptr;
	}

	UUID PhysicsSystem::BodyIDToUUID(unsigned int bodyID)
	{
		return bodyIDMapUUID[bodyID];
	}

	bool PhysicsSystem::BodyIDToIsSensor(unsigned int bodyID)
	{
		return bodySensorMap[bodyID];
	}

	void PhysicsSystem::SetActive(unsigned int bodyID)
	{
		if(!sPhysicsData.body_interface->IsAdded((BodyID)bodyID))
		sPhysicsData.body_interface->AddBody((BodyID)bodyID, EActivation::Activate);
	}

	void PhysicsSystem::SetInactive(unsigned int bodyID)
	{
		if(sPhysicsData.body_interface->IsAdded((BodyID)bodyID))
		sPhysicsData.body_interface->RemoveBody((BodyID)bodyID);
	}

	std::pair<glm::vec3, glm::vec3> PhysicsSystem::calculateBoundingVolume(const Model& model)
	{
		glm::vec3 minExtent{}, maxExtent{};

		for (const auto& mesh : model.mMeshes)
		{
			for (const auto& pos : mesh.GetPosition())
			{
				// Update min and max extents for each axis
				minExtent.x = std::min(minExtent.x, pos.x);
				minExtent.y = std::min(minExtent.y, pos.y);
				minExtent.z = std::min(minExtent.z, pos.z);
														   
				maxExtent.x = std::max(maxExtent.x, pos.x);
				maxExtent.y = std::max(maxExtent.y, pos.y);
				maxExtent.z = std::max(maxExtent.z, pos.z);
			}
		}

		glm::vec3 boundingVolumeCenter = (minExtent + maxExtent) * 0.5f;

		return { boundingVolumeCenter, maxExtent - minExtent };

	}


	std::pair<glm::vec3, glm::vec3> PhysicsSystem::calculateBoundingVolume(const SkinnedModel& model)
	{
		glm::vec3 minExtent{}, maxExtent{};

		for (const auto& mesh : model.mMeshes)
		{
			for (const auto& pos : mesh.GetVertices())
			{
				// Update min and max extents for each axis
				minExtent.x = std::min(minExtent.x, pos.Position.x);
				minExtent.y = std::min(minExtent.y, pos.Position.y);
				minExtent.z = std::min(minExtent.z, pos.Position.z);

				maxExtent.x = std::max(maxExtent.x, pos.Position.x);
				maxExtent.y = std::max(maxExtent.y, pos.Position.y);
				maxExtent.z = std::max(maxExtent.z, pos.Position.z);
			}
		}

		glm::vec3 boundingVolumeCenter = (minExtent + maxExtent) * 0.5f;

		return { boundingVolumeCenter, maxExtent - minExtent };

	}

	glm::vec3 PhysicsSystem::calculateBoxSize(glm::vec3 minExtent, glm::vec3 maxExtent)
	{
		// Calculate the size of the box
		return maxExtent - minExtent;
	}

	float PhysicsSystem::calculateSphereRadius(glm::vec3 boundingVolume)
	{
		return (glm::length(boundingVolume) * 0.5f);
	}

	std::pair<float, float> PhysicsSystem::calculateCapsuleDimensions(glm::vec3 boundingVolume)
	{
		// Radius is half of the smallest width in the X or Z dimensions
		float radius = 0.5f * std::min(boundingVolume.x, boundingVolume.z);

		// Half-height is half of the height (Y dimension), minus the radius
		float halfHeight = 0.5f * boundingVolume.y - radius;

		return { radius, halfHeight };
	}

	std::pair<float, float> PhysicsSystem::calculateCylinderDimensions(glm::vec3 boundingVolume)
	{
		// Radius is half of the smallest width in the X or Z dimensions
		float radius = 0.5f * std::min(boundingVolume.x, boundingVolume.z);

		// Half-height is half of the height (Y dimension)
		float halfHeight = 0.5f * boundingVolume.y;

		return { radius, halfHeight };

	}

	void PhysicsSystem::AddForce(unsigned int bodyID, glm::vec3 force)
	{
		sPhysicsData.body_interface->AddForce((BodyID)bodyID, JPH::RVec3(force.x, force.y, force.z));
	}

	void PhysicsSystem::AddTorque(unsigned int bodyID, glm::vec3 torque)
	{
		sPhysicsData.body_interface->AddTorque((BodyID)bodyID, JPH::RVec3(torque.x, torque.y, torque.z));
	}

	void PhysicsSystem::AddImpulse(unsigned int bodyID, glm::vec3 impulse)
	{
		sPhysicsData.body_interface->AddImpulse((BodyID)bodyID, JPH::RVec3(impulse.x, impulse.y, impulse.z));
	}

	glm::vec3 PhysicsSystem::GetLinearVelocity(unsigned int bodyID)
	{
		auto data = sPhysicsData.body_interface->GetLinearVelocity((BodyID)bodyID);
		return { data.GetX(), data.GetY(), data.GetZ() };
	}

	glm::vec3 PhysicsSystem::GetAngularVelocity(unsigned int bodyID)
	{
		auto data = sPhysicsData.body_interface->GetAngularVelocity((BodyID)bodyID);
		return { data.GetX(), data.GetY(), data.GetZ() };
	}

	glm::vec3 PhysicsSystem::GetLinearVelocity(void* character)
	{

		CharacterVirtual* mCharacter = reinterpret_cast<CharacterVirtual*>(character);
		return { mCharacter->GetLinearVelocity().GetX(), mCharacter->GetLinearVelocity().GetY(), mCharacter->GetLinearVelocity().GetZ() };
	}

	void PhysicsSystem::SetLinearVelocity(void* character, glm::vec3 vel)
	{
		CharacterVirtual* mCharacter = reinterpret_cast<CharacterVirtual*>(character);
		mCharacter->SetLinearVelocity({ vel.x,vel.y,vel.z });
	}

	void PhysicsSystem::SetLinearVelocity(unsigned int bodyID, glm::vec3 velocity)
	{
		sPhysicsData.body_interface->SetLinearVelocity((BodyID)bodyID, JPH::RVec3(velocity.x, velocity.y, velocity.z));
	}

	void PhysicsSystem::SetAngularVelocity(unsigned int bodyID, glm::vec3 velocity)
	{
		sPhysicsData.body_interface->SetAngularVelocity((BodyID)bodyID, JPH::RVec3(velocity.x, velocity.y, velocity.z));
	}

	glm::vec3 PhysicsSystem::GetPosition(unsigned int bodyID)
	{
		auto Data = sPhysicsData.body_interface->GetPosition((BodyID)bodyID);
		//std::cout << Data.GetX() << " , " << Data.GetY() << " , " << Data.GetZ() << std::endl;
		return { Data.GetX(), Data.GetY(), Data.GetZ() };
	}

	void PhysicsSystem::SetPosition(unsigned int bodyID, glm::vec3 position)
	{
		// Set the position of the body
		sPhysicsData.body_interface->SetPosition(
			(BodyID)bodyID,
			JPH::RVec3(position.x, position.y, position.z),
			EActivation::Activate
		);
	}


	glm::vec3 PhysicsSystem::GetRotation(unsigned int bodyID)
	{
		auto Data = sPhysicsData.body_interface->GetRotation((BodyID)bodyID);
		return { Data.GetX(), Data.GetY(), Data.GetZ() };
	}

	void PhysicsSystem::SetRotation(unsigned int bodyID, glm::vec3 rotation)
	{
		// Convert bodyID to BodyID (assuming BodyID is convertible from unsigned int)
		BodyID joltBodyID(bodyID);

		// Convert glm::vec3 rotation (Euler angles in radians) to a Quat
		Quat joltRotation = Quat::sEulerAngles(Vec3(rotation.x, rotation.y, rotation.z));

		// Set the rotation with activation
		EActivation activationMode = EActivation::Activate; // Activate the body (or use EActivation::DontActivate if preferred)
		sPhysicsData.body_interface->SetRotation(joltBodyID, joltRotation, activationMode);
	}

	void PhysicsSystem::move(ColliderComponent& rigidbody, glm::vec3 motion)
	{
		// Get the current position of the body
		JPH::RVec3 JoltMotion = JPH::RVec3(motion.x, motion.y, motion.z);

		// Set the new position
		sPhysicsData.body_interface->SetLinearVelocity((BodyID)rigidbody.bodyID, JoltMotion);
	}

	void PhysicsSystem::addCharacter(CharacterControllerComponent& character, TransformComponent& transform, ColliderComponent& collider, UUID entityID)
	{
		CharacterVirtualSettings settings;
		ShapeRefC shape;
		ShapeSettings::ShapeResult shape_result;

		BoxColliderComponent* boxPtr = dynamic_cast<BoxColliderComponent*>(&collider);
		SphereColliderComponent* spherePtr = dynamic_cast<SphereColliderComponent*>(&collider);
		CapsuleColliderComponent* capsulePtr = dynamic_cast<CapsuleColliderComponent*>(&collider);
		CylinderColliderComponent* cylinderPtr = dynamic_cast<CylinderColliderComponent*>(&collider);


		if (boxPtr)
		{
			glm::vec3 size = { boxPtr->size.x * 0.5f * transform.Scale.x, boxPtr->size.y * 0.5f * transform.Scale.y, boxPtr->size.z * 0.5f * transform.Scale.z };
			BoxShapeSettings box_shape_settings(Vec3(size.x, size.y, size.z));
			box_shape_settings.SetEmbedded();
			shape_result = box_shape_settings.Create();
			shape = shape_result.Get();
		}
		else if (spherePtr)
		{
			SphereShapeSettings sphere_shape_settings(spherePtr->radius *((transform.Scale.x + transform.Scale.y + transform.Scale.z) * 0.33f)); //For sphere scaling of xyz should be equal. 
			sphere_shape_settings.SetEmbedded();
			shape_result = sphere_shape_settings.Create();
			shape = shape_result.Get();
		}
		else if (capsulePtr)
		{
			CapsuleShapeSettings capsule_shape_settings(capsulePtr->height * transform.Scale.y, capsulePtr->radius * (transform.Scale.x + transform.Scale.z) * 0.5f); //For capsule scaling when Y is up, X and Z is the width and Y is the height.
			capsule_shape_settings.SetEmbedded();
			shape_result = capsule_shape_settings.Create();
			shape = shape_result.Get();
		}
		else if (cylinderPtr)
		{
			CylinderShapeSettings cylinder_shape_settings(cylinderPtr->height * transform.Scale.y, cylinderPtr->radius * (transform.Scale.x + transform.Scale.z) * 0.5f);
			cylinder_shape_settings.SetEmbedded();
			shape_result = cylinder_shape_settings.Create();
			shape = shape_result.Get();
		}

		settings.mMaxSlopeAngle = JPH::DegreesToRadians(character.slopeAngle);
		settings.mMaxStrength = character.strength;
		settings.mMass = character.mass;
		settings.mShape = shape;
		settings.mInnerBodyShape = shape;
		Entity BrEntity = SceneManager::GetActiveScene()->GetEntityByUUID(entityID);
		settings.mInnerBodyLayer = BrEntity.GetComponent<TagComponent>().mLayer.toUint16();
		settings.SetEmbedded();

		std::lock_guard<std::mutex>lock(characterMutex);
		character.controller = new CharacterVirtual(&settings, RVec3(transform.Translate.x, transform.Translate.y,transform.Translate.z), Quat::sIdentity(), sPhysicsData.mSystem);
		bodyIDMapUUID[reinterpret_cast<CharacterVirtual*>(character.controller)->GetInnerBodyID().GetIndexAndSequenceNumber()] = entityID;
		bodySensorMap[reinterpret_cast<CharacterVirtual*>(character.controller)->GetInnerBodyID().GetIndexAndSequenceNumber()] = false;

		BOREALIS_CORE_WARN(entityID);
	}


	void PhysicsSystem::PrePhysicsUpdate(float dt, void* Character)
	{
		CharacterVirtual* mCharacter = reinterpret_cast<CharacterVirtual*>(Character);
		bool sEnableStickToFloor = true;
		bool sEnableWalkStairs = true;
		auto vel = mCharacter->GetLinearVelocity();

		// Settings for our update function
		CharacterVirtual::ExtendedUpdateSettings update_settings;
		if (!sEnableStickToFloor)
			update_settings.mStickToFloorStepDown = Vec3::sZero();
		else
			update_settings.mStickToFloorStepDown = -mCharacter->GetUp() * update_settings.mStickToFloorStepDown.Length();
		if (!sEnableWalkStairs)
			update_settings.mWalkStairsStepUp = Vec3::sZero();
		else
			update_settings.mWalkStairsStepUp = mCharacter->GetUp() * update_settings.mWalkStairsStepUp.Length();

		// Update the character position
		mCharacter->ExtendedUpdate(dt,
			-mCharacter->GetUp() * sPhysicsData.mSystem->GetGravity().Length(),
			update_settings,
			{},
			{},
			{ },
			{ },
			*sPhysicsData.temp_allocator);

	}

	bool PhysicsSystem::IsCharacterOnGround(void* Character)
	{
		CharacterVirtual* mCharacter = reinterpret_cast<CharacterVirtual*>(Character);
		return mCharacter->GetGroundState() != CharacterVirtual::EGroundState::InAir;
	}

	void PhysicsSystem::HandleInput(float inDeltaTime, CharacterControllerComponent& controllerComp)
	{
		CharacterVirtual* mCharacter = reinterpret_cast<CharacterVirtual*>(controllerComp.controller);

		bool player_controls_horizontal_velocity = controllerComp.enableInertia || mCharacter->IsSupported();
		if (player_controls_horizontal_velocity)
		{
			// Smooth the player input : Enable character inertia
			controllerComp.targetVelocity = true ? 0.25f * controllerComp.inMovementDirection + 0.75f * controllerComp.targetVelocity : controllerComp.inMovementDirection;

			// True if the player intended to mov
			controllerComp.sliding = glm::all(glm::lessThan(glm::abs(controllerComp.inMovementDirection), glm::vec3(glm::epsilon<float>())));
		}
		else
		{
			// While in air we allow sliding
			controllerComp.sliding = true;
		}

		// Update the character rotation and its up vector to match the up vector set by the user settings

		// A cheaper way to update the character's ground velocity,
		// the platforms that the character is standing on may have changed velocity
		mCharacter->UpdateGroundVelocity();

		// Determine new basic velocity
		Vec3 current_vertical_velocity = mCharacter->GetLinearVelocity().Dot(mCharacter->GetUp()) * mCharacter->GetUp();
		Vec3 ground_velocity = mCharacter->GetGroundVelocity();
		Vec3 new_velocity;
		bool moving_towards_ground = (current_vertical_velocity.GetY() - ground_velocity.GetY()) < 0.1f;
		if (mCharacter->GetGroundState() == CharacterVirtual::EGroundState::OnGround	// If on ground
			&& (controllerComp.enableInertia ?
				moving_towards_ground													// Inertia enabled: And not moving away from ground
				: !mCharacter->IsSlopeTooSteep(mCharacter->GetGroundNormal())))			// Inertia disabled: And not on a slope that is too steep
		{
			// Assume velocity of ground when on ground
			new_velocity = ground_velocity;

			// Jump
			if (controllerComp.isJump && moving_towards_ground)
			{
				new_velocity += controllerComp.jumpSpeed * mCharacter->GetUp();
				controllerComp.isJump = false;
			}
		}
		else
			new_velocity = current_vertical_velocity;

		// Gravity
		new_velocity += (Vec3(0.f,-controllerComp.gravity,0.f)) * inDeltaTime;

		if (player_controls_horizontal_velocity)
		{
			// Player input
			new_velocity += JPH::Vec3(controllerComp.targetVelocity.x, controllerComp.targetVelocity.y, controllerComp.targetVelocity.z);
		}
		else
		{
			// Preserve horizontal velocity
			Vec3 current_horizontal_velocity = mCharacter->GetLinearVelocity() - current_vertical_velocity;
			new_velocity += current_horizontal_velocity;
		}

		// Update character velocity
		mCharacter->SetLinearVelocity(new_velocity);

		// Stance switch
		/*if (inSwitchStance)
		{
			bool is_standing = mCharacter->GetShape() == mStandingShape;
			const Shape* shape = is_standing ? mCrouchingShape : mStandingShape;
			if (mCharacter->SetShape(shape, 1.5f * mPhysicsSystem->GetPhysicsSettings().mPenetrationSlop, mPhysicsSystem->GetDefaultBroadPhaseLayerFilter(Layers::MOVING), mPhysicsSystem->GetDefaultLayerFilter(Layers::MOVING), { }, { }, *mTempAllocator))
			{
				const Shape* inner_shape = is_standing ? mInnerCrouchingShape : mInnerStandingShape;
				mCharacter->SetInnerBodyShape(inner_shape);
			}
		}*/
	}

	void PhysicsSystem::FreeCharacter(CharacterControllerComponent& character)
	{

		bodyIDMapUUID.erase(reinterpret_cast<CharacterVirtual*>(character.controller)->GetInnerBodyID().GetIndexAndSequenceNumber());
		bodySensorMap.erase(reinterpret_cast<CharacterVirtual*>(character.controller)->GetInnerBodyID().GetIndexAndSequenceNumber());

		if(character.controller)
		{
			auto InnerID = reinterpret_cast<CharacterVirtual*>(character.controller)->GetInnerBodyID();
			delete character.controller;
			sPhysicsData.mSystem->GetBodyInterface().RemoveBody(InnerID);
			sPhysicsData.mSystem->GetBodyInterface().DestroyBody(InnerID);
		}


		character.controller = nullptr;
	}

	class ObjectLayerFilterImpl : public ObjectLayerFilter
	{
	public:
		ObjectLayerFilterImpl(Bitset32 l) : bitset(l)
		{

		}
		bool ShouldCollide(ObjectLayer inLayer) const override
		{
			// some function to extract the bitset to index numbers;
			// for example: bitset contains index 3 and 6 (layer 3 & 6)

			for (int index : bitset.ToBitsList())
			{
				if (index == inLayer)
				{
					return true;
				}
			}
			return false;
		}
	private:
		Bitset32 bitset;
	};
	

	bool PhysicsSystem::RayCast(glm::vec3 origin, glm::vec3 direction, float maxDistance, Bitset32 LayerMask)
	{
		direction = glm::normalize(direction);
		direction *= maxDistance; //set distance of ray
		RRayCast ray{ Vec3(origin.x, origin.y, origin.z), Vec3(direction.x, direction.y, direction.z) };
		auto& narrowPhaseQuery = sPhysicsData.mSystem->GetNarrowPhaseQuery();
		RayCastResult result;
		//result.mFraction = maxDistance;
		if (LayerMask.to_ulong() == 0)
		{
			return narrowPhaseQuery.CastRay(ray, result);
		}
		return narrowPhaseQuery.CastRay(ray, result, {}, ObjectLayerFilterImpl(LayerMask));
		if (PhysicsSystem::shouldDebugDraw)
		{
			Renderer2D::DrawLine(origin, origin + direction * result.mFraction, glm::vec4(1, 0, 0, 1));

		}
	}


	bool PhysicsSystem::RayCast(glm::vec3 origin, glm::vec3 direction, RaycastHit* hitInfo, float maxDistance, Bitset32 LayerMask)
	{
		direction = glm::normalize(direction);
		direction *= maxDistance; //set distance of ray
		RRayCast ray{ Vec3(origin.x, origin.y, origin.z), Vec3(direction.x, direction.y, direction.z) };
		auto& narrowPhaseQuery = sPhysicsData.mSystem->GetNarrowPhaseQuery();
		RayCastResult result;
		bool output;
		if (LayerMask.to_ulong() == 0)
		{
			output = narrowPhaseQuery.CastRay(ray, result);
		}
		else
		{
			output = narrowPhaseQuery.CastRay(ray, result, {}, ObjectLayerFilterImpl(LayerMask));
		}

		hitInfo->colliderID = *(reinterpret_cast<uint32_t*>(&result.mBodyID));
		hitInfo->distance = maxDistance * result.mFraction;
		hitInfo->ID = PhysicsSystem::BodyIDToUUID(result.mBodyID.GetIndexAndSequenceNumber());

		JPH::BodyLockRead lock(sPhysicsData.mSystem->GetBodyLockInterface(), result.mBodyID);
		if (lock.Succeeded())
		{
			const JPH::Body& body = lock.GetBody();
			auto normals = body.GetWorldSpaceSurfaceNormal(result.mSubShapeID2, ray.GetPointOnRay(result.mFraction));
			hitInfo->normal = { normals.GetX(), normals.GetY(), normals.GetZ() };
		}
		hitInfo->point = origin + direction * hitInfo->distance;

		if (PhysicsSystem::shouldDebugDraw)
		{
			Renderer2D::DrawLine(origin, origin + direction * result.mFraction, glm::vec4(1, 0, 0, 1));

		}
		return output;
	}

	class RayCollector : public CastRayCollector
	{
	public:

		void AddHit(const ResultType& inResult) override
		{
			hits.push_back(inResult);
		}
		std::vector<RayCastResult> hits;
	};

	std::vector<RaycastHit> PhysicsSystem::RayCastAll(glm::vec3 origin, glm::vec3 direction, float maxDistance, Bitset32 LayerMask)
	{
		direction = glm::normalize(direction);
		direction *= maxDistance;
		RayCollector collector;
		RRayCast ray{ Vec3(origin.x, origin.y, origin.z), Vec3(direction.x, direction.y, direction.z) };
		auto& narrowPhaseQuery = sPhysicsData.mSystem->GetNarrowPhaseQuery();
		if (LayerMask.to_ulong() == 0)
		{
			narrowPhaseQuery.CastRay(ray, {}, collector);
		}
		else
		{
			narrowPhaseQuery.CastRay(ray, {}, collector, {}, ObjectLayerFilterImpl(LayerMask));
		}
		std::vector<RaycastHit> output;
		for (auto hitResult : collector.hits)
		{
			RaycastHit hit;
			hit.colliderID = *(reinterpret_cast<uint32_t*>(&hitResult.mBodyID));
			hit.distance = maxDistance * hitResult.mFraction;
			hit.ID = PhysicsSystem::BodyIDToUUID(hitResult.mBodyID.GetIndexAndSequenceNumber());
			
			JPH::BodyLockWrite lock(sPhysicsData.mSystem->GetBodyLockInterface(), hitResult.mBodyID);
			if (lock.Succeeded())
			{
				JPH::Body& body = lock.GetBody();
				auto normals = body.GetWorldSpaceSurfaceNormal(hitResult.mSubShapeID2, ray.GetPointOnRay(hitResult.mFraction));
				hit.normal = { normals.GetX(), normals.GetY(), normals.GetZ() };
			}
			hit.point = origin + direction * hit.distance;

			output.push_back(hit);
		}

		return output;
	}

	void PhysicsSystem::UpdateScale(ColliderComponent& collider, TransformComponent& transform)
	{
		ShapeRefC shape;
		ShapeSettings::ShapeResult shape_result;

		BoxColliderComponent* boxPtr = dynamic_cast<BoxColliderComponent*>(&collider);
		SphereColliderComponent* spherePtr = dynamic_cast<SphereColliderComponent*>(&collider);
		CapsuleColliderComponent* capsulePtr = dynamic_cast<CapsuleColliderComponent*>(&collider);
		CylinderColliderComponent* cylinderPtr = dynamic_cast<CylinderColliderComponent*>(&collider);

		if (boxPtr)
		{
			glm::vec3 size = { boxPtr->size.x * 0.5f * transform.Scale.x, boxPtr->size.y * 0.5f * transform.Scale.y, boxPtr->size.z * 0.5f * transform.Scale.z };
			if (size.x < 0)
			{
				size.x = -size.x;
			}

			if (size.y < 0)
			{
				size.y = -size.y;
			}

			if (size.z < 0)
			{
				size.z = -size.z;
			}
			BoxShapeSettings box_shape_settings(Vec3(size.x, size.y, size.z));
			box_shape_settings.SetEmbedded();
			shape_result = box_shape_settings.Create();
			shape = shape_result.Get();
		}
		else if (spherePtr)
		{
			SphereShapeSettings sphere_shape_settings(spherePtr->radius * ((transform.Scale.x + transform.Scale.y + transform.Scale.z) * 0.33f)); //For sphere scaling of xy
			sphere_shape_settings.SetEmbedded();
			shape_result = sphere_shape_settings.Create();
			shape = shape_result.Get();
		}
		else if (capsulePtr)
		{
			CapsuleShapeSettings capsule_shape_settings(capsulePtr->height * transform.Scale.y, capsulePtr->radius * (transform.Scale.x + transform.Scale.z) * 0.5f);
			capsule_shape_settings.SetEmbedded();
			shape_result = capsule_shape_settings.Create();
			shape = shape_result.Get();
		}
		else if (cylinderPtr)
		{
			CylinderShapeSettings cylinder_shape_settings(cylinderPtr->height * transform.Scale.y, cylinderPtr->radius * (transform.Scale.x + transform.Scale.z)*0.5f);
			cylinder_shape_settings.SetEmbedded();
			shape_result = cylinder_shape_settings.Create();
			shape = shape_result.Get();
		}
		
		sPhysicsData.body_interface->SetShape((BodyID)collider.bodyID, shape, true, EActivation::Activate);

	}

	void PhysicsSystem::addBody(TransformComponent& transform, RigidbodyComponent* rigidbody, ColliderComponent& collider, UUID entityID)
	{
		ShapeRefC shape;
		ShapeSettings::ShapeResult shape_result;

		BoxColliderComponent* boxPtr = dynamic_cast<BoxColliderComponent*>(&collider);
		SphereColliderComponent* spherePtr = dynamic_cast<SphereColliderComponent*>(&collider);
		CapsuleColliderComponent* capsulePtr = dynamic_cast<CapsuleColliderComponent*>(&collider);
		CylinderColliderComponent* cylinderPtr = dynamic_cast<CylinderColliderComponent*>(&collider);

		if (boxPtr)
		{
			glm::vec3 size = { boxPtr->size.x * 0.5f * transform.Scale.x, boxPtr->size.y * 0.5f * transform.Scale.y, boxPtr->size.z * 0.5f * transform.Scale.z };
			if (size.x < 0)
			{
				size.x = -size.x;
			}

			if (size.y < 0)
			{
				size.y = -size.y;
			}

			if (size.z < 0)
			{
				size.z = -size.z;
			}
			BoxShapeSettings box_shape_settings(Vec3(size.x, size.y, size.z));
			box_shape_settings.SetEmbedded();
			shape_result = box_shape_settings.Create();
			shape = shape_result.Get();
		}
		else if (spherePtr)
		{
			SphereShapeSettings sphere_shape_settings(spherePtr->radius * ((transform.Scale.x + transform.Scale.y + transform.Scale.z) * 0.33f)); //For sphere scaling of xy
			sphere_shape_settings.SetEmbedded();
			shape_result = sphere_shape_settings.Create();
			shape = shape_result.Get();
		}
		else if (capsulePtr)
		{
			CapsuleShapeSettings capsule_shape_settings(capsulePtr->height * transform.Scale.y, capsulePtr->radius * (transform.Scale.x+transform.Scale.z) * 0.5f);
			capsule_shape_settings.SetEmbedded();
			shape_result = capsule_shape_settings.Create();
			shape = shape_result.Get();
		}
		else if (cylinderPtr)
		{
			CylinderShapeSettings tCapsule_shape_settings(cylinderPtr->height * transform.Scale.y, cylinderPtr->radius * (transform.Scale.x + transform.Scale.z) * 0.5f);
			tCapsule_shape_settings.SetEmbedded();
			shape_result = tCapsule_shape_settings.Create();
			shape = shape_result.Get();
		}

		auto entityTransform = transform.GetTransform();
		auto modelCenter = collider.center;
		auto actualCenterVec4 = entityTransform * glm::vec4(modelCenter, 1.0f);
		auto actualCenter = glm::vec3(actualCenterVec4.x, actualCenterVec4.y, actualCenterVec4.z);


		auto brEntity = SceneManager::GetActiveScene()->GetEntityByUUID(entityID);
		auto rotation = transform.GetGlobalRotation();
		auto tagComponent = brEntity.GetComponent<TagComponent>();

		glm::quat quatRot = glm::quat(glm::radians(rotation));  // Assuming Rotation is in degrees

		// Convert glm::quat to Jolt's Quat (JPH::Quat)
		JPH::Quat newRotation = JPH::Quat(quatRot.x, quatRot.y, quatRot.z, quatRot.w);

		BodyCreationSettings body_settings(shape, RVec3(actualCenter.x, actualCenter.y, actualCenter.z), newRotation, EMotionType::Static, Layers::MOVING);

		if (rigidbody)
		{
			if (rigidbody->movement == MovementType::Dynamic) {
				body_settings.mMotionType = EMotionType::Dynamic;
			}
			else if (rigidbody->movement == MovementType::Kinematic)
			{
				body_settings.mMotionType = EMotionType::Kinematic;
			}
			else
			{
				body_settings.mMotionType = EMotionType::Static;
			}
			body_settings.mObjectLayer = tagComponent.mLayer.toUint16();
			body_settings.mAllowDynamicOrKinematic = true;
			body_settings.mFriction = rigidbody->friction;
			body_settings.mRestitution = 0.f;
		}
		else
		{
			body_settings.mMotionType = EMotionType::Static;
			body_settings.mObjectLayer = tagComponent.mLayer.toUint16();
		}

		// Create the actual rigid body
		Body* body = sPhysicsData.body_interface->CreateBody(body_settings); // Handle nullptr in a real scenario
		if (!body) {
			// Handle error (e.g., failed to create body)
			return;
		}

		// Add it to the world
		sPhysicsData.body_interface->AddBody(body->GetID(), EActivation::Activate);

		// Store the BodyID in the RigidBodyComponent
		collider.bodyID = body->GetID().GetIndexAndSequenceNumber();

		std::lock_guard<std::mutex>lock(bodyMutex);

		if (collider.isTrigger)
		{
			body->SetIsSensor(true);
			bodySensorMap[collider.bodyID] = true;
		}
		else
		{
			bodySensorMap[collider.bodyID] = false;
		}

		bodyIDMapUUID[collider.bodyID] = entityID;
	}

	void PhysicsSystem::FreeRigidBody(ColliderComponent& collider)
	{
		if (collider.rigidBody)
		{
			sPhysicsData.body_interface->RemoveBody(JPH::BodyID(collider.bodyID));
			sPhysicsData.body_interface->DestroyBody(JPH::BodyID(collider.bodyID));
			bodyIDMapUUID.erase(collider.bodyID);
			bodySensorMap.erase(collider.bodyID);
		}
	}
}