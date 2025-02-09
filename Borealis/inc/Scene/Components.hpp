/******************************************************************************/
/*!
\file		Components.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 12, 2024
\brief		Declares the component structs and classes

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
#include <any>
#include <unordered_set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Scene/SceneCamera.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/SkinnedModel.hpp>
#include <Graphics/Animation/Animator.hpp>
#include <Graphics/Material.hpp>
#include <Graphics/Font.hpp>
#include <Graphics/Framebuffer.hpp>
#include <Graphics/ParticleSystem.hpp>
#include <AI/BehaviourTree/BehaviourTree.hpp>
#include <AI/BehaviourTree/BTreeFactory.hpp>
#include <Core/UUID.hpp>
#include <Core/Bitset32.hpp>
#include <Audio/Audio.hpp>
#include <Audio/AudioGroup.hpp>

#include "Graphics/UI/Button.hpp"


namespace Borealis
{
	class Entity;

	struct IDComponent
	{
		UUID ID;
		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(uint64_t uuid) : ID(uuid) {}
	};
	struct TagComponent
	{
		bool active = true;
		std::string Name;
		std::string Tag;
		Bitset32 mLayer;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Name(tag) {
			Tag = "";
		}
	};

	struct TransformComponent
	{
		glm::vec3 Translate { 0.0f, 0.0f ,0.0f };
		glm::vec3 Rotation{ 0.0f, 0.0f ,0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
		UUID ParentID = 0;
		std::unordered_set<UUID> ChildrenID{};
	
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3 translate)
			: Translate(translate) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), Translate);
			glm::mat4 rotation = glm::mat4(glm::quat(glm::radians(Rotation)));
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), Scale);

			return translation * rotation * scale;
		}

		glm::mat4 GetGlobalTransform();
		glm::vec3 GetGlobalTranslate();
		glm::vec3 GetGlobalRotation();
		glm::vec3 GetGlobalScale();

		void GetGlobalTransformComp(glm::vec3* translate, glm::vec3* rotate, glm::vec3* scale);
		void SetGlobalTransform(glm::mat4 transform);
		void SetParent(Entity entity, Entity parent);
		void ResetParent(Entity entity);

		operator glm::mat4() { return GetTransform(); }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Colour {1.0f,1.0f,1.0f,1.0f};
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 colour)
			: Colour(colour) {}
	};

	struct CircleRendererComponent 
	{
		glm::vec4 Colour{ 1.0f,1.0f,1.0f,1.0f };
		float thickness = 1.0;
		float fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4 colour, float thickness, float fade)
			: Colour(colour), thickness(thickness), fade(fade) {}
	};

	struct CameraComponent 
	{
		SceneCamera Camera;
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	//Forward Declaration
	class ScriptEntity;
	struct NativeScriptComponent
	{
		ScriptEntity* Instance = nullptr;
		
		ScriptEntity* (*Init)();
		void (*Free)(NativeScriptComponent*);

		template <typename T>
		void Bind()
		{
			Init = []() {return static_cast<ScriptEntity*> (new T()); };
			Free = [](NativeScriptComponent* nsc) {delete nsc->Instance(); nsc->Instance() = nullptr; };
		}
	};

	// To be done:
	struct MeshFilterComponent
	{
		Ref<Model> Model;

		MeshFilterComponent() = default;
		MeshFilterComponent(const MeshFilterComponent&) = default;
		MeshFilterComponent(Borealis::Model model) { Model = MakeRef<Borealis::Model>(model); }
	};

	struct MeshRendererComponent
	{
		Ref<Material> Material = nullptr;
		bool castShadow = true;
		bool active = true;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
	};

	struct SkinnedMeshRendererComponent
	{
		Ref<SkinnedModel> SkinnnedModel = nullptr;
		Ref<Material>	  Material = nullptr;

		SkinnedMeshRendererComponent() = default;
		SkinnedMeshRendererComponent(const SkinnedMeshRendererComponent&) = default;
	};

	struct AnimatorComponent
	{
		Ref<Animation> animation = nullptr;
		Animator animator{};
		bool loop = true;
		float speed = 1.f;
		AssetHandle currentAnimationHandle;

		AnimatorComponent() = default;
		AnimatorComponent(const AnimatorComponent&) = default;
	};

	// Move into appropraite file another time
	class PhysicMaterial
	{
		public:
			enum CombineMethod :uint8_t
			{
				Average, Minimum, Multiply, Maximum
			};
			PhysicMaterial() = default;
			PhysicMaterial(const PhysicMaterial&) = default;
			PhysicMaterial(float staticFriction, float dynamicFriction, float bounciness, CombineMethod FrictionCombine, CombineMethod BounceCombine)
				: mStaticFriction(staticFriction), mDynamicFriction(dynamicFriction), mBounciness(bounciness), mFrictionCombine(FrictionCombine), mBounceCombine(BounceCombine) {}

			float GetStaticFriction() const { return mStaticFriction; }
			float GetDynamicFriction() const { return mDynamicFriction; }
			float GetBounciness() const { return mBounciness; }
			CombineMethod GetFrictionCombine() const { return mFrictionCombine; }
			CombineMethod GetBounceCombine() const { return mBounceCombine; }

			void SetStaticFriction(float staticFriction) { mStaticFriction = staticFriction; }
			void SetDynamicFriction(float dynamicFriction) { mDynamicFriction = dynamicFriction; }
			void SetBounciness(float bounciness) { mBounciness = bounciness; }
			void SetFrictionCombine(CombineMethod frictionCombine) { mFrictionCombine = frictionCombine; }
			void SetBounceCombine(CombineMethod bounceCombine) { mBounceCombine = bounceCombine; }
		
	private:
		float mStaticFriction = 0.6f;
		float mDynamicFriction = 0.6f;
		float mBounciness = 0;
		CombineMethod mFrictionCombine = CombineMethod::Average;
		CombineMethod mBounceCombine = CombineMethod::Average;
	};


	enum class RigidBodyType : int
	{
		Box,
		Sphere,
		Capsule
	};

	enum class MovementType : int
	{
		Static,
		Dynamic,
		Kinematic
	};

	struct RigidbodyComponent
	{
		MovementType movement = MovementType::Static;
		float friction = 0.5f;
		float bounciness = 0.5f;
		bool dynamicBody = false;
		// not serialised
		glm::vec3 offset = { 0.0f, 0.0f, 0.0f };
		//glm::vec3 velocity = { 0,0,0 };
		//float mass = 1.f;
		//float drag = 0.f;
		//float angularDrag = 0.0f;
		//glm::vec3 centerOfMass = { 0,0,0 };
		//glm::vec3 inertiaTensor = { 1,1,1 };
		//glm::vec3 inertiaTensorRotation = { 0,0,0 };
		//bool AutomaticCenterOfMass = true;
		//bool AutomaticTensor = true;
		//bool useGravity = true;
		//bool isKinematic = false;

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;
	};

	struct ColliderComponent
	{
		virtual ~ColliderComponent() = default;
		bool isTrigger = false;
		bool providesContact = false;
		glm::vec3 center = { 0,0,0 };
		Ref<PhysicMaterial> Material;
		RigidbodyComponent* rigidBody = nullptr;
		unsigned int bodyID = 0;
	};

	struct CylinderColliderComponent : public ColliderComponent
	{
		float radius = 1.f;
		float height = 2.f;
	};

	struct BoxColliderComponent : public ColliderComponent
	{
		glm::vec3 size = { 1,1,1 };
	};

	struct SphereColliderComponent : public ColliderComponent
	{
		float radius = 1.f;
	};

	struct CapsuleColliderComponent : public ColliderComponent
	{
		enum class Direction : uint8_t
		{
			X,
			Y,
			Z
		};

		float radius = 0.5f;
		float height = 2;
		Direction direction = Direction::Y;
	};

	struct CharacterControllerComponent
	{
		float mass = 1.f;
		float strength = 1.f;
		float slopeAngle = 45.f;
		bool enableInertia = true;
		bool moveInAir = true;
		bool sliding = true;
		bool isJump = false;
		float jumpSpeed = 0;
		float gravity = 50.f;

		void* controller = nullptr;
		glm::vec3 targetVelocity = { 0,0,0 };
		glm::vec3 inMovementDirection = { 0,0,0 };

		CharacterControllerComponent() = default;
		~CharacterControllerComponent()
		{
			if (controller)
			{
				delete controller;
			}
		}
		CharacterControllerComponent(const CharacterControllerComponent&) = default;
	};

	struct LightComponent
	{
		enum class Type : uint8_t
		{
			Spot, 
			Directional, 
			Point
		};

		enum class LightAppearance : uint8_t
		{
			Colour,
			Temperature
		};

		enum class ShadowType : uint8_t
		{
			None, 
			Hard, 
			Soft
		};

		glm::vec3 position;
		glm::vec3 direction;
		Type type = Type::Directional;

		glm::vec4 color = { 1.f,1.f,1.f,1.f};
		float Intensity = 1;
		float range = 10; //for spot and point
		float spotAngle = 30; //for spot only

		bool castShadow = false;
		bool isEdited = true;
	};

	struct TextComponent
	{
		enum class TextAlign : uint8_t
		{
			Left,
			Center
		};

		std::string text{};
		uint32_t fontSize = 16; //change to float?
		glm::vec4 colour{ 1.f,1.f,1.f,1.f };
		Ref<Font> font;
		TextAlign align = TextAlign::Left;

		TextComponent() = default;
		TextComponent(const TextComponent&) = default;
	};

	class ScriptInstance;
	struct ScriptComponent
	{
		std::unordered_map <std::string, Ref<ScriptInstance>> mScripts;

		void AddScript(const std::string& name, const Ref<ScriptInstance>& script)
		{
			mScripts[name] = script;
		}

		void RemoveScript(const std::string& name)
		{
			mScripts.erase(name);
		}

		bool HasScript(const std::string& name)
		{
			return mScripts.find(name) != mScripts.end();
		}
	};
	
	struct AudioSourceComponent
	{
		std::string group = "Master";
		bool isLoop = false;
		bool isMute = false;
		bool isPlaying = false;
		float Volume = 1.0f;
		int channelID = 0;

	
		Ref<Audio> audio;

		AudioSourceComponent() = default;
		AudioSourceComponent(const AudioSourceComponent&) = default;
	};

	struct AudioListenerComponent
	{
		bool isAudioListener = true;

		AudioListenerComponent() = default;
		AudioListenerComponent(const AudioListenerComponent&) = default;
	};

	struct BehaviourTreeComponent
	{
		//std::unordered_set<Ref<BehaviourTree>> mBehaviourTrees;
		//void AddTree()
		//{
		//	mBehaviourTrees.emplace(Ref<BehaviourTree>());
		//}
		//void AddTree(Ref<BehaviourTree> bt)
		//{
		//	mBehaviourTrees.emplace(bt);
		//}

		//void Update(float dt)
		//{
		//	for (auto& tree : mBehaviourTrees)
		//	{
		//		tree->Update(dt);
		//	}
		//}
		Ref<BehaviourTreeData> mBehaviourTreeData;
		Ref<BehaviourTree> mBehaviourTrees;
		// List of names in tree-style
		//TreeData -> AssetManager with ID
		//BehaviourNode mRoot;
		BehaviourTreeComponent() = default;
		BehaviourTreeComponent(const BehaviourTreeComponent&) = default;
	};

	struct OutLineComponent
	{
		glm::vec4 color = { 0.043f, 0.8f, 0.961f , 1.f };
		float lineWidth = 2.f;
		bool filled;
		bool active;

		OutLineComponent() = default;
		OutLineComponent(const OutLineComponent&) = default;
	};

	struct CanvasComponent
	{
		glm::vec2 canvasSize{};
		float scaleFactor{};
		Ref<FrameBuffer> canvasFrameBuffer = nullptr;
		enum class RenderMode : uint8_t
		{
			WorldSpace,
			ScreenSpace
		};
		RenderMode renderMode = RenderMode::ScreenSpace;
		int renderIndex{};

		CanvasComponent() = default;
		CanvasComponent(const CanvasComponent&) = default;
	};

	struct CanvasRendererComponent
	{
		bool allowPassthrough = false;

		CanvasRendererComponent() = default;
		CanvasRendererComponent(const CanvasRendererComponent&) = default;
	};

	struct ParticleSystemComponent
	{
		float		duration = 5.f;
		bool		looping = true;
		float		startDelay = 0.f;
		float		startLifeTime = 5.f;
		float		startSpeed = 5.f;
		bool		_3DStartSizeBool = false;
		bool		randomStartSize = false;
		glm::vec3	startSize = glm::vec3{ 1.f }; //if not 3d, use .x for size
		glm::vec3	startSize2 = glm::vec3{ 1.f }; //if not 3d, use .x for size
		bool		_3DStartRotationBool = false;
		glm::vec3	startRotation = glm::vec3{ 0.f }; // if not 3d, use .x for rotation
		bool		randomStartColor = false;
		glm::vec4	startColor = glm::vec4{ 1.f };
		glm::vec4	startColor2 = glm::vec4{ 1.f };
		bool		endColorBool;
		glm::vec4	endColor = glm::vec4{ 1.f };
		float		gravityModifer = 0.f;
		float		simulationSpeed = 1.f;
		uint32_t	maxParticles = 1000;
		float		rateOverTime = 10.f;
		float		angle = 25.f;
		float		radius = 1.f;
		float		radiusThickness = 0.f; //0-1 based on radius
		bool		billboard = true;
		bool		isEdited = false;

		Ref<Texture2D> texture = nullptr;
		Ref<ParticleSystem> particleSystem = nullptr;
		
		//Add variables for over time

		ParticleSystemComponent() = default;
		ParticleSystemComponent(const ParticleSystemComponent&) = default;
	};

	struct ButtonComponent
	{
		std::string onClickFunctionName{};
		std::string onReleaseFunctionName{};
		std::string onHoverFunctionName{};

		std::string onClickClass{};
		std::string onReleaseClass{};
		std::string onHoverClass{};

		UUID onClickEntity = 0;
		UUID onReleaseEntity = 0;
		UUID onHoverEntity = 0;

		bool hovered = false;
		bool clicked = false;
		bool released = false;
		bool interactable = true;

		glm::vec3 center{};
		glm::vec3 size{1.f, 1.f, 1.f};

		void onClick();
		void onRelease();
		void onHover();

		ButtonComponent() = default;
		ButtonComponent(const ButtonComponent&) = default;
	};
}

#endif