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
#include <Graphics/Material.hpp>
#include <Graphics/Font.hpp>
#include <AI/BehaviourTree/BehaviourTree.hpp>
#include <Core/UUID.hpp>
#include <Audio/Audio.hpp>

namespace Borealis
{

	struct IDComponent
	{
		UUID ID;
		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(uint64_t uuid) : ID(uuid) {}
	};
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translate { 0.0f, 0.0f ,0.0f };
		glm::vec3 Rotation{ 0.0f, 0.0f ,0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
	
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
		Ref<Material> Material;
		bool castShadow = true;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
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

	struct BoxColliderComponent
	{
		bool isTrigger = false;
		bool providesContact = false;
		Ref<PhysicMaterial> Material;
		glm::vec3 Center{ 0,0,0 };
		glm::vec3 Size{ 1,1,1 };
		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
	};

	struct CapsuleColliderComponent
	{
		enum class Direction : uint8_t
		{
			X,
			Y,
			Z
		};

		bool isTrigger = false;
		bool providesContact = false;
		Ref<PhysicMaterial> Material;
		float radius = 0.5f;
		float height = 2;
		Direction direction = Direction::Y;
		CapsuleColliderComponent() = default;
		CapsuleColliderComponent(const CapsuleColliderComponent&) = default;
	};


	enum class RigidBodyType : int
	{
		Box,
		Circle
	};
	struct RigidBodyComponent
	{

		RigidBodyType isBox = RigidBodyType::Box;
		float radius = 1.5f; //radius for circle, side for cube

		

		// not serialised
		unsigned int bodyID = 0;
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

		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent&) = default;
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

		
		glm::vec3 offset;
		//glm::vec4 Colour = { 1,1,1,1 };
		glm::vec2 InnerOuterSpot = { 100, 120 };
		//float Temperature = 6500;
		//float Intensity = 1;
		//float IndirectMultiplier = 1;
		//float Range = 10;
		Type type = Type::Directional;
		glm::vec3 direction = {0.0, -1.0, 0.0};
		glm::vec3 ambient = {0.4, 0.4, 0.4};
		glm::vec3 diffuse = {1.f, 1.f, 1.f};
		glm::vec3 specular = {1.f, 1.f, 1.f};
		float linear = 0.05f;
		float quadratic = 0.032f;
		/*ShadowType shadowType = ShadowType::None;
		LightAppearance lightAppearance = LightAppearance::Colour;*/
	};

	struct TextComponent
	{
		std::string text{};
		uint32_t fontSize = 16;
		Ref<Font> font;

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
		std::unordered_set<Ref<BehaviourTree>> mBehaviourTrees;
		void AddTree()
		{
			mBehaviourTrees.emplace(Ref<BehaviourTree>());
		}
		void AddTree(Ref<BehaviourTree> bt)
		{
			mBehaviourTrees.emplace(bt);
		}

		void Update(float dt)
		{
			for (auto& tree : mBehaviourTrees)
			{
				tree->Update(dt);
			}
		}

	};

}

#endif