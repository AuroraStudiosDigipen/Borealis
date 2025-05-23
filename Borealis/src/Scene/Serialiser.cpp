/******************************************************************************/
/*!
\file		Serialiser.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 14, 2024
\brief		Defines the class for Serialiser

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <yaml-cpp/yaml.h>
#include <imgui.h>
#include <Scene/ReflectionInstance.hpp>
#include <Scene/Serialiser.hpp>
#include <Scene/SerialiserUtils.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Components.hpp>
#include <AI/BehaviourTree/BehaviourNode.hpp>
#include <Core/LoggerSystem.hpp>
#include <ImGui/ImGuiFontLib.hpp>
#include <Assets/AssetManager.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <Scripting/ScriptField.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptingUtils.hpp>
#include <Core/HierarchyLayerManager.hpp>
#include <Core/Project.hpp>
#include <Audio/AudioEngine.hpp>
#include <Core/Utils.hpp>
#include <openssl/evp.h>
namespace Borealis
{

	struct ScriptInitialData
	{
		Ref<ScriptInstance> scriptInstance;
		UUID targetUUID;
		std::string scriptFieldName;
	};
	static std::queue<ScriptInitialData> scriptQueue;

	void SerializeTexture(YAML::Emitter& out, Ref<Texture2D> texture)
	{
		if (texture)
		{
			out << YAML::Key << "Texture" << YAML::Value << texture->mAssetHandle;
		}
	}

	Serialiser::Serialiser(const Ref<Scene>& scene) : mScene(scene) {}

	void Serialiser::SerializeEntity(YAML::Emitter& out, Entity& entity)
	{
		out << YAML::Flow << YAML::BeginMap;
		out << YAML::Key << "EntityID" << YAML::Value << entity.GetComponent<IDComponent>().ID;
		if (entity.HasComponent<TagComponent>())
		{
			SerializeComponent(out, entity.GetComponent<TagComponent>());
		}

		if (entity.HasComponent<CameraComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CameraComponent>());
		}

		if (entity.HasComponent<TransformComponent>())
		{
			SerializeComponent(out, entity.GetComponent<TransformComponent>());
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			SerializeComponent(out, entity.GetComponent<SpriteRendererComponent>());
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CircleRendererComponent>());
		}

		if (entity.HasComponent<MeshFilterComponent>())
		{
			SerializeComponent(out, entity.GetComponent<MeshFilterComponent>());
		}

		if (entity.HasComponent<MeshRendererComponent>())
		{
			SerializeComponent(out, entity.GetComponent<MeshRendererComponent>());
		}

		if (entity.HasComponent<SkinnedMeshRendererComponent>())
		{
			SerializeComponent(out, entity.GetComponent<SkinnedMeshRendererComponent>());
		}

		if (entity.HasComponent<AnimatorComponent>())
		{
			SerializeComponent(out, entity.GetComponent<AnimatorComponent>());
		}

		if (entity.HasComponent<RigidbodyComponent>())
		{
			SerializeComponent(out, entity.GetComponent<RigidbodyComponent>());
		}

		if (entity.HasComponent<LightComponent>())
		{
			SerializeComponent(out, entity.GetComponent<LightComponent>());
		}

		if (entity.HasComponent<TextComponent>())
		{
			SerializeComponent(out, entity.GetComponent<TextComponent>());
		}

		if (entity.HasComponent<AudioListenerComponent>())
		{
			SerializeComponent(out, entity.GetComponent<AudioListenerComponent>());
		}

		if (entity.HasComponent<AudioSourceComponent>())
		{
			SerializeComponent(out, entity.GetComponent<AudioSourceComponent>());
		}

		if (entity.HasComponent<CharacterControllerComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CharacterControllerComponent>());
		}

		if (entity.HasComponent<BoxColliderComponent>())
		{
			SerializeComponent(out, entity.GetComponent<BoxColliderComponent>());
		}


		if (entity.HasComponent<SphereColliderComponent>())
		{
			SerializeComponent(out, entity.GetComponent<SphereColliderComponent>());
		}

		if (entity.HasComponent<CylinderColliderComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CylinderColliderComponent>());
		}


		if (entity.HasComponent<CapsuleColliderComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CapsuleColliderComponent>());
		}
		if (entity.HasComponent<BehaviourTreeComponent>())
		{
			SerializeComponent(out, entity.GetComponent<BehaviourTreeComponent>());
		}

		if (entity.HasComponent<OutLineComponent>())
		{
			SerializeComponent(out, entity.GetComponent<OutLineComponent>());
		}

		if (entity.HasComponent<CanvasComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CanvasComponent>());
		}

		if (entity.HasComponent<CanvasRendererComponent>())
		{
			SerializeComponent(out, entity.GetComponent<CanvasRendererComponent>());
		}		
		
		if (entity.HasComponent<ParticleSystemComponent>())
		{
			SerializeComponent(out, entity.GetComponent<ParticleSystemComponent>());
		}

		if (entity.HasComponent<ButtonComponent>())
		{
			SerializeComponent(out, entity.GetComponent<ButtonComponent>());
		}

		if (entity.HasComponent<UIAnimatorComponent>())
		{
			SerializeComponent(out, entity.GetComponent<UIAnimatorComponent>());
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;

			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			for (auto [name, script] : scriptComponent.mScripts)
			{
				out << YAML::Key << name;
				out << YAML::BeginMap;

				for (auto [fieldName, field] : script->GetScriptClass()->mFields)
				{
					if ((field.isPrivate() && !field.hasSerializeField(script->GetMonoClass()) || field.hasHideInInspector(script->GetMonoClass())))
					{
						continue;
					}

					if (field.mFieldClassName() == "AudioClip")
					{
						MonoObject* Data = script->GetFieldValue<MonoObject*>(fieldName);
						if (!Data) continue;
						auto assetID = field.GetAudioID(Data);
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << field.mFieldClassName().c_str();
						out << YAML::Key << "Data" << YAML::Value << AudioEngine::GuidToString(assetID);
						out << YAML::EndMap;
					}

					if (field.isAssetField() || field.isNativeComponent())
					{
						MonoObject* Data = script->GetFieldValue<MonoObject*>(fieldName);
						if (!Data)
						{
							continue;
						}
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << field.mFieldClassName().c_str();
						out << YAML::Key << "Data" << YAML::Value << field.GetGameObjectID(Data);
						out << YAML::EndMap;
						continue;
					}

					if (field.isGameObject())
					{
						MonoObject* Data = script->GetFieldValue<MonoObject*>(fieldName);
						if (!Data)
						{
							continue;
						}
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "GameObject";
						out << YAML::Key << "Data" << YAML::Value << field.GetGameObjectID(Data);
						out << YAML::EndMap;
						continue;
					}

					if (field.isMonoBehaviour())
					{
						MonoObject* Data = script->GetFieldValue<MonoObject*>(fieldName);
						if (!Data)
						{
							continue;
						}
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "MonoBehaviour";
						out << YAML::Key << "Data" << YAML::Value << field.GetAttachedID(Data);
						out << YAML::EndMap;
						continue;
					}

					if (field.mType == ScriptFieldType::Bool)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Bool";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<bool>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Float)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Float";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<float>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Int)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Int";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<int>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::String)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "String";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<std::string>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Vector2)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Vector2";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<glm::vec2>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Vector3)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Vector3";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<glm::vec3>(fieldName);
						out << YAML::EndMap;
						continue;
					}

					if (field.mType == ScriptFieldType::Vector4)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Vector4";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<glm::vec4>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::UChar)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "UChar";
						out << YAML::Key << "Data" << YAML::Value << static_cast<unsigned>(script->GetFieldValue<unsigned char>(fieldName));
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Char)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Char";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<char>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::UShort)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "UShort";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<unsigned short>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Short)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Short";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<short>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::UInt)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "UInt";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<unsigned>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Long)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Long";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<long long>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::ULong)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "ULong";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<unsigned long long>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Double)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Double";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<double>(fieldName);
						out << YAML::EndMap;
						continue;
					}
				}

				out << YAML::EndMap;
			}

			out << YAML::EndMap;
		}

		SerialiseAbstractItems(out, entity);

		out << YAML::EndMap;

		for (auto child : entity.GetComponent<TransformComponent>().ChildrenID)
		{
			auto childEntity = mScene->GetEntityByUUID(child);
			SerializeEntity(out, childEntity);
		}
	}

	void Serialiser::SerializeEntityScript(YAML::Emitter& out, Entity& entity)
	{

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Flow << YAML::BeginMap;
			out << YAML::Key << "EntityID" << YAML::Value << entity.GetComponent<IDComponent>().ID;
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;

			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			for (auto [name, script] : scriptComponent.mScripts)
			{
				out << YAML::Key << name;
				out << YAML::BeginMap;

				for (auto [fieldName, field] : script->GetScriptClass()->mFields)
				{
					if ((field.isPrivate() && !field.hasSerializeField(script->GetMonoClass()) || field.hasHideInInspector(script->GetMonoClass())))
					{
						continue;
					}

					if (field.mFieldClassName() == "AudioClip")
					{
						MonoObject* Data = script->GetFieldValue<MonoObject*>(fieldName);
						if (!Data) continue;
						auto assetID = field.GetAudioID(Data);
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << field.mFieldClassName().c_str();
						out << YAML::Key << "Data" << YAML::Value << AudioEngine::GuidToString(assetID);
						out << YAML::EndMap;
					}

					if (field.isAssetField() || field.isNativeComponent())
					{
						MonoObject* Data = script->GetFieldValue<MonoObject*>(fieldName);
						if (!Data)
						{
							continue;
						}
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << field.mFieldClassName().c_str();
						out << YAML::Key << "Data" << YAML::Value << field.GetGameObjectID(Data);
						out << YAML::EndMap;
						continue;
					}

					if (field.isGameObject())
					{
						MonoObject* Data = script->GetFieldValue<MonoObject*>(fieldName);
						if (!Data)
						{
							continue;
						}
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "GameObject";
						out << YAML::Key << "Data" << YAML::Value << field.GetGameObjectID(Data);
						out << YAML::EndMap;
						continue;
					}

					if (field.isMonoBehaviour())
					{
						MonoObject* Data = script->GetFieldValue<MonoObject*>(fieldName);
						if (!Data)
						{
							continue;
						}
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "MonoBehaviour";
						out << YAML::Key << "Data" << YAML::Value << field.GetAttachedID(Data);
						out << YAML::EndMap;
						continue;
					}

					if (field.mType == ScriptFieldType::Bool)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Bool";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<bool>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Float)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Float";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<float>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Int)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Int";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<int>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::String)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "String";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<std::string>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Vector2)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Vector2";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<glm::vec2>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Vector3)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Vector3";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<glm::vec3>(fieldName);
						out << YAML::EndMap;
						continue;
					}

					if (field.mType == ScriptFieldType::Vector4)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Vector4";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<glm::vec4>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::UChar)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "UChar";
						out << YAML::Key << "Data" << YAML::Value << static_cast<unsigned>(script->GetFieldValue<unsigned char>(fieldName));
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Char)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Char";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<char>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::UShort)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "UShort";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<unsigned short>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Short)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Short";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<short>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::UInt)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "UInt";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<unsigned>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Long)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Long";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<long long>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::ULong)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "ULong";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<unsigned long long>(fieldName);
						out << YAML::EndMap;
						continue;
					}
					if (field.mType == ScriptFieldType::Double)
					{
						out << YAML::Key << field.mName << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << "Double";
						out << YAML::Key << "Data" << YAML::Value << script->GetFieldValue<double>(fieldName);
						out << YAML::EndMap;
						continue;
					}
				}

				out << YAML::EndMap;
			}

			out << YAML::EndMap;
			out << YAML::EndMap;
		}


		for (auto child : entity.GetComponent<TransformComponent>().ChildrenID)
		{
			auto childEntity = mScene->GetEntityByUUID(child);
			SerializeEntityScript(out, childEntity);
		}
	}

	bool Serialiser::SerialiseScene(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap
			<< YAML::Key << "Scene" << YAML::Value << mScene->GetName()

			<< YAML::Key << "RenderSettings" << YAML::Value << YAML::BeginMap
			<< YAML::Key << "BloomBool" << YAML::Value << mScene->GetSceneRenderConfig().bloom
			<< YAML::Key << "BloomThreshold" << YAML::Value << mScene->GetSceneRenderConfig().ubo.threshold
			<< YAML::Key << "BloomScale" << YAML::Value << mScene->GetSceneRenderConfig().ubo.bloomScale
			<< YAML::Key << "BloomKnee" << YAML::Value << mScene->GetSceneRenderConfig().ubo.knee
			<< YAML::Key << "Exposure" << YAML::Value << mScene->GetSceneRenderConfig().ubo.exposure
			<< YAML::EndMap

			<< YAML::Key <<"Entities"	<<	YAML::Value << YAML::BeginSeq;

		auto view = HierarchyLayerManager::GetInstance().GetEntitiesInLayerOrder();
		for (auto id : view)
		{
			auto entity = mScene->GetEntityByUUID(id);
			SerializeEntity(out, entity);

		}
		out << YAML::EndSeq;
		out	<< YAML::EndMap;

		// Create directory if doesnt exist
		std::filesystem::path fileSystemPaths = filepath;
		std::filesystem::create_directories(fileSystemPaths.parent_path());

		std::ofstream outStream(filepath);
		outStream << out.c_str();
		outStream.close();
		return true;
	}
	static std::stringstream backupSS;
	void Serialiser::SerialiseBackupScriptData(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap
			<< YAML::Key << "Scene" << YAML::Value << mScene->GetName()
			<< YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		auto view = HierarchyLayerManager::GetInstance().GetEntitiesInLayerOrder();
		for (auto id : view)
		{
			auto entity = mScene->GetEntityByUUID(id);
			SerializeEntityScript(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		backupSS << out.c_str();
	}

	void Serialiser::DeserialiseBackupScriptData(const std::string& filepath)
	{
		std::stringstream ss;
		ss << backupSS.rdbuf();
		YAML::Node data = YAML::Load(ss.str());
		auto entities = data["Entities"];

		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["EntityID"].as<uint64_t>(); // UUID
				DeserialiseEntityScript(entity, mScene->GetRegistry(), *reinterpret_cast<UUID*>(&uuid));
				Entity entity2(mScene->mEntityMap[uuid], mScene.get());
			}
			while (!scriptQueue.empty())
			{
				auto& scriptData = scriptQueue.front();
				Entity targetEntity = mScene->GetEntityByUUID(scriptData.targetUUID);
				auto& scriptComponent = targetEntity.GetComponent<ScriptComponent>();
				auto script = scriptComponent.mScripts.find(scriptData.scriptInstance->GetScriptClass()->mFields[scriptData.scriptFieldName].mFieldClassName());
				scriptData.scriptInstance->SetFieldValue(scriptData.scriptFieldName, script->second->GetInstance());
				scriptQueue.pop();
			}
		}
		backupSS.clear();
	}

	static std::unordered_set<UUID> sEntityList;

	void Serialiser::DeserialiseEntityScript(YAML::detail::iterator_value& entity, entt::registry& registry, UUID& uuid)
	{
		auto BorealisEntity = mScene->GetEntityByUUID(uuid);
		if (BorealisEntity.HasComponent<ScriptComponent>())
		{
			BorealisEntity.RemoveComponent<ScriptComponent>();
		}
		auto scriptComponent = entity["ScriptComponent"];
		if (scriptComponent)
		{
			auto& sc = BorealisEntity.AddComponent<ScriptComponent>();
			for (const auto& script : scriptComponent)
			{
				std::string scriptName = script.first.as<std::string>();
				auto scriptInstance = MakeRef<ScriptInstance>(ScriptingSystem::GetScriptClass(scriptName));
				scriptInstance->Init(BorealisEntity.GetUUID()); // Initialise the script instance (set the entity reference
				sc.AddScript(scriptName, scriptInstance);

				const YAML::Node& fields = script.second;
				if (fields) {
					for (const auto& field : fields) {


						// Each field will have a name and a corresponding node
						std::string fieldName = field.first.as<std::string>();
						const YAML::Node& fieldData = field.second;
						fieldData["Type"].as<std::string>();


						if (scriptInstance->GetScriptClass()->mFields.find(fieldName) == scriptInstance->GetScriptClass()->mFields.end())
						{
							BOREALIS_CORE_WARN("Field not in script", fieldName, scriptName);
							continue;
						}
						ScriptField scriptField = scriptInstance->GetScriptClass()->mFields[fieldName];

						if ((scriptField.isPrivate() && !scriptField.hasSerializeField(scriptInstance->GetMonoClass()) || scriptField.hasHideInInspector(scriptInstance->GetMonoClass())))
						{
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "AudioClip")
						{
							std::string data = fieldData["Data"].as<std::string>();
							MonoObject* field = nullptr;
							InitAudioObject(field, AudioEngine::StringToGuid(data), fieldData["Type"].as<std::string>());
							scriptInstance->SetFieldValue(fieldName, field);

							continue;
						}


						if (fieldData["Type"].as<std::string>() == "GameObject")
						{
							uint64_t data = fieldData["Data"].as<uint64_t>();
							if (!mScene->EntityExists(data))
							{
								continue;
							}
							MonoObject* field = nullptr;
							InitGameObject(field, data, fieldData["Type"].as<std::string>());
							scriptInstance->SetFieldValue(fieldName, field);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "MonoBehaviour")
						{
							uint64_t data = fieldData["Data"].as<uint64_t>();
							if (!mScene->EntityExists(data))
							{
								continue;
							}
							scriptQueue.push({ scriptInstance, data, fieldName });
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Bool")
						{
							bool data = fieldData["Data"].as<bool>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Float")
						{
							float data = fieldData["Data"].as<float>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Int")
						{
							int data = fieldData["Data"].as<int>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "String")
						{
							std::string data = fieldData["Data"].as<std::string>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Vector2")
						{
							glm::vec2 data = fieldData["Data"].as<glm::vec2>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Vector3")
						{
							glm::vec3 data = fieldData["Data"].as<glm::vec3>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Vector4")
						{
							glm::vec4 data = fieldData["Data"].as<glm::vec4>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "UChar")
						{
							unsigned char data = static_cast<unsigned char>(fieldData["Data"].as<unsigned>());
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Char")
						{
							char data = fieldData["Data"].as<char>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "UShort")
						{
							unsigned short data = fieldData["Data"].as<unsigned short>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Short")
						{
							short data = fieldData["Data"].as<short>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "UInt")
						{
							unsigned data = fieldData["Data"].as<unsigned>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Long")
						{
							long long data = fieldData["Data"].as<long long>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "ULong")
						{
							unsigned long long data = fieldData["Data"].as<unsigned long long>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Double")
						{
							double data = fieldData["Data"].as<double>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (scriptInstance->GetScriptClass()->mFields[fieldName].isAssetField())
						{
							uint64_t data = fieldData["Data"].as<uint64_t>();
							MonoObject* field = nullptr;
							InitGameObject(field, data, fieldData["Type"].as<std::string>());
							scriptInstance->SetFieldValue(fieldName, field);
							continue;
						}

						if (scriptInstance->GetScriptClass()->mFields[fieldName].isNativeComponent())
						{
							uint64_t data = fieldData["Data"].as<uint64_t>();
							if (!mScene->EntityExists(data))
							{
								continue;
							}
							MonoObject* field = nullptr;
							InitGameObject(field, data, fieldData["Type"].as<std::string>());
							scriptInstance->SetFieldValue(fieldName, field);
							continue;
						}
					}
				}
			}
		}
	}


	entt::entity Serialiser::DeserialiseEntity(YAML::detail::iterator_value& entity, entt::registry& registry, UUID& uuid)
	{

		auto loadedEntity = registry.create();
		auto BorealisEntity = Entity(loadedEntity, mScene.get());
		BorealisEntity.AddComponent<IDComponent>(uuid);

		DeserialiseComponent<TagComponent>(entity, BorealisEntity);
		DeserialiseComponent<TransformComponent>(entity, BorealisEntity);
		DeserialiseComponent<SpriteRendererComponent>(entity, BorealisEntity);
		DeserialiseComponent<CircleRendererComponent>(entity, BorealisEntity);
		DeserialiseComponent<CameraComponent>(entity, BorealisEntity);
		DeserialiseComponent<MeshFilterComponent>(entity, BorealisEntity);
		DeserialiseComponent<MeshRendererComponent>(entity, BorealisEntity);
		DeserialiseComponent<RigidbodyComponent>(entity, BorealisEntity);
		DeserialiseComponent<LightComponent>(entity, BorealisEntity);
		DeserialiseComponent<CharacterControllerComponent>(entity, BorealisEntity);
		DeserialiseComponent<AudioSourceComponent>(entity, BorealisEntity);
		DeserialiseComponent<AudioListenerComponent>(entity, BorealisEntity);
		DeserialiseComponent<TextComponent>(entity, BorealisEntity);
		DeserialiseComponent<SkinnedMeshRendererComponent>(entity, BorealisEntity);
		DeserialiseComponent<AnimatorComponent>(entity, BorealisEntity);
		DeserialiseComponent<OutLineComponent>(entity, BorealisEntity);
		DeserialiseComponent<CanvasComponent>(entity, BorealisEntity);
		DeserialiseComponent<CanvasRendererComponent>(entity, BorealisEntity);
		DeserialiseComponent<ParticleSystemComponent>(entity, BorealisEntity);
		DeserialiseComponent<BoxColliderComponent>(entity, BorealisEntity);
		DeserialiseComponent<SphereColliderComponent>(entity, BorealisEntity);
		DeserialiseComponent<CapsuleColliderComponent>(entity, BorealisEntity);
		DeserialiseComponent<CylinderColliderComponent>(entity, BorealisEntity);
		DeserialiseComponent<BehaviourTreeComponent>(entity, BorealisEntity);
		DeserialiseComponent<ButtonComponent>(entity, BorealisEntity);
		DeserialiseComponent<UIAnimatorComponent>(entity, BorealisEntity);
		DeserialiseAbstractItems(entity, BorealisEntity);
		TagList::AddEntity(BorealisEntity.GetComponent<TagComponent>().Tag, uuid);
		if (BorealisEntity.HasComponent<ParticleSystemComponent>()) {
			BorealisEntity.GetComponent<ParticleSystemComponent>().Init();
		}

		auto scriptComponent = entity["ScriptComponent"];
		if (scriptComponent)
		{
			auto& sc = BorealisEntity.AddComponent<ScriptComponent>();
			for (const auto& script : scriptComponent)
			{
				std::string scriptName = script.first.as<std::string>();
				auto scriptInstance = MakeRef<ScriptInstance>(ScriptingSystem::GetScriptClass(scriptName));
				scriptInstance->Init(BorealisEntity.GetUUID()); // Initialise the script instance (set the entity reference
				sc.AddScript(scriptName, scriptInstance);

				const YAML::Node& fields = script.second;
				if (fields) {
					for (const auto& field : fields) {


						// Each field will have a name and a corresponding node
						std::string fieldName = field.first.as<std::string>();
						const YAML::Node& fieldData = field.second;
						fieldData["Type"].as<std::string>();


						if (scriptInstance->GetScriptClass()->mFields.find(fieldName) == scriptInstance->GetScriptClass()->mFields.end())
						{
							BOREALIS_CORE_WARN("Field not in script", fieldName, scriptName);
							continue;
						}
						ScriptField scriptField = scriptInstance->GetScriptClass()->mFields[fieldName];

						if ((scriptField.isPrivate() && !scriptField.hasSerializeField(scriptInstance->GetMonoClass()) || scriptField.hasHideInInspector(scriptInstance->GetMonoClass())))
						{
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "AudioClip")
						{
							std::string data = fieldData["Data"].as<std::string>();
							MonoObject* field = nullptr;
							InitAudioObject(field, AudioEngine::StringToGuid(data), fieldData["Type"].as<std::string>());
							scriptInstance->SetFieldValue(fieldName, field);
							
							continue;
						}


						if (fieldData["Type"].as<std::string>() == "GameObject")
						{
							uint64_t data = fieldData["Data"].as<uint64_t>();

							if (sEntityList.find(data) == sEntityList.end())
							{
								continue;
							}
							MonoObject* field = nullptr;
							InitGameObject(field, data, fieldData["Type"].as<std::string>());
							scriptInstance->SetFieldValue(fieldName, field);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "MonoBehaviour")
						{
							uint64_t data = fieldData["Data"].as<uint64_t>();
							if (sEntityList.find(data) == sEntityList.end())
							{
								continue;
							}
							scriptQueue.push({ scriptInstance, data, fieldName });
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Bool")
						{
							bool data = fieldData["Data"].as<bool>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Float")
						{
							float data = fieldData["Data"].as<float>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Int")
						{
							int data = fieldData["Data"].as<int>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "String")
						{
							std::string data = fieldData["Data"].as<std::string>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Vector2")
						{
							glm::vec2 data = fieldData["Data"].as<glm::vec2>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Vector3")
						{
							glm::vec3 data = fieldData["Data"].as<glm::vec3>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Vector4")
						{
							glm::vec4 data = fieldData["Data"].as<glm::vec4>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "UChar")
						{
							unsigned char data = static_cast<unsigned char>(fieldData["Data"].as<unsigned>());
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Char")
						{
							char data = fieldData["Data"].as<char>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "UShort")
						{
							unsigned short data = fieldData["Data"].as<unsigned short>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Short")
						{
							short data = fieldData["Data"].as<short>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "UInt")
						{
							unsigned data = fieldData["Data"].as<unsigned>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Long")
						{
							long long data = fieldData["Data"].as<long long>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "ULong")
						{
							unsigned long long data = fieldData["Data"].as<unsigned long long>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (fieldData["Type"].as<std::string>() == "Double")
						{
							double data = fieldData["Data"].as<double>();
							scriptInstance->SetFieldValue(fieldName, &data);
							continue;
						}

						if (scriptInstance->GetScriptClass()->mFields[fieldName].isAssetField())
						{
							uint64_t data = fieldData["Data"].as<uint64_t>();
							MonoObject* field = nullptr;
							InitGameObject(field, data, fieldData["Type"].as<std::string>());
							scriptInstance->SetFieldValue(fieldName, field);
							continue;
						}

						if (scriptInstance->GetScriptClass()->mFields[fieldName].isNativeComponent())
						{
							uint64_t data = fieldData["Data"].as<uint64_t>();
							if (sEntityList.find(data) == sEntityList.end())
							{
								continue;
							}
							MonoObject* field = nullptr;
							InitGameObject(field, data, fieldData["Type"].as<std::string>());
							scriptInstance->SetFieldValue(fieldName, field);
							continue;
						}
					}
				}
			}
		}
		return loadedEntity;
	}

	static bool decryptFileToStream(const std::string& inputPath,
		unsigned char* key, unsigned char* iv,
		std::stringstream& decryptedStream) {
		std::ifstream inFile(inputPath, std::ios::binary);
		if (!inFile) return false;

		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (!ctx) return false;

		EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);

		const size_t bufferSize = 4096;
		std::vector<unsigned char> inBuf(bufferSize);
		std::vector<unsigned char> outBuf(bufferSize);

		int outLen = 0;
		while (inFile.good()) {
			inFile.read(reinterpret_cast<char*>(inBuf.data()), bufferSize);
			std::streamsize readBytes = inFile.gcount();

			if (!EVP_DecryptUpdate(ctx, outBuf.data(), &outLen, inBuf.data(), static_cast<int>(readBytes))) {
				EVP_CIPHER_CTX_free(ctx);
				return false;
			}

			// Write decrypted data to stringstream
			decryptedStream.write(reinterpret_cast<char*>(outBuf.data()), outLen);
		}

		if (!EVP_DecryptFinal_ex(ctx, outBuf.data(), &outLen)) {
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}

		// Write the final decrypted data to stringstream
		decryptedStream.write(reinterpret_cast<char*>(outBuf.data()), outLen);

		EVP_CIPHER_CTX_free(ctx);
		return true;
	}

	bool Serialiser::DeserialiseScene(const std::string& filepath, bool encrypt)
	{
		std::stringstream ss;
		TagList::Clear();

		if (encrypt)
		{
			auto IV = Project::assembleIV();
			auto Key = Project::assembleKey();
			decryptFileToStream(filepath, Key.data(), IV.data(), ss);
		}
		else
		{
			std::ifstream inStream(filepath);
			ss << inStream.rdbuf();
			inStream.close();
		}
		

		YAML::Node data = YAML::Load(ss.str());
		if (!data["Scene"])
		{
			BOREALIS_CORE_ERROR("Scene not found in file: {}", filepath);
			return false;
		}

		std::string sceneName = data["Scene"].as<std::string>();
		BOREALIS_CORE_INFO("Deserialising scene: {}", sceneName);
		// Deserialise scene info such as viewport sizes
		mScene->ResizeViewport(1920, 1080);
		auto entities = data["Entities"];


		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["EntityID"].as<uint64_t>(); // UUID
				sEntityList.insert(uuid);
			}
		}

		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["EntityID"].as<uint64_t>(); // UUID
				mScene->mEntityMap[uuid] = DeserialiseEntity(entity, mScene->GetRegistry(), *reinterpret_cast<UUID*>(&uuid));
				Entity entity2(mScene->mEntityMap[uuid], mScene.get());
			}
			while (!scriptQueue.empty())
			{
				auto& scriptData = scriptQueue.front();
				Entity targetEntity = mScene->GetEntityByUUID(scriptData.targetUUID);
				auto& scriptComponent = targetEntity.GetComponent<ScriptComponent>();
				auto script = scriptComponent.mScripts.find(scriptData.scriptInstance->GetScriptClass()->mFields[scriptData.scriptFieldName].mFieldClassName());
				scriptData.scriptInstance->SetFieldValue(scriptData.scriptFieldName, script->second->GetInstance());
				scriptQueue.pop();
			}
		}

		sEntityList.clear();

		// Delete invalid children
		for (auto[id, entity] : mScene->mEntityMap)
		{
			auto& transform = mScene->GetRegistry().get<TransformComponent>(entity);
			std::vector<int> removeID;
			int i = 0;
			for (auto childID : transform.ChildrenID)
			{
				if (!mScene->mEntityMap.contains(childID))
				{
					removeID.push_back(i);
				}
				i++;
			}

			for (auto iterator = removeID.rbegin(); iterator < removeID.rend(); iterator++)
			{
				transform.ChildrenID.erase(transform.ChildrenID.begin() + *iterator);
			}
		}

		auto renderSettings = data["RenderSettings"];
		if (renderSettings)
		{
			mScene->GetSceneRenderConfig().bloom = renderSettings["BloomBool"].as<bool>();
			mScene->GetSceneRenderConfig().ubo.threshold = renderSettings["BloomThreshold"].as<float>();
			mScene->GetSceneRenderConfig().ubo.bloomScale = renderSettings["BloomScale"].as<float>();
			mScene->GetSceneRenderConfig().ubo.knee = renderSettings["BloomKnee"].as<float>();
			mScene->GetSceneRenderConfig().ubo.exposure = renderSettings["Exposure"].as<float>();
		}

		return true;
	}

	void Serialiser::operator()(const Ref<Scene>& scene)
	{
		mScene = scene;
	}

	void Serialiser::SerialisePrefab(const std::string& filepath, Entity entity)
	{
		YAML::Emitter out;
		out << YAML::BeginMap
			<< YAML::Key << "Scene" << YAML::Value << "Prefab"
			<< YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		SerializeEntity(out, entity);

		out << YAML::EndSeq
			<< YAML::EndMap;

		// Create directory if doesnt exist
		std::filesystem::path fileSystemPaths = filepath;
		std::filesystem::create_directories(fileSystemPaths.parent_path());

		std::ofstream outStream(filepath);
		outStream << out.c_str();
		outStream.close();
	}

	bool Serialiser::DeserialiseEditorStyle()
	{
		std::string filepath = "settings/style.setting";
		std::ifstream inStream(filepath);
		std::stringstream ss;
		ss << inStream.rdbuf();
		inStream.close();

		YAML::Node data = YAML::Load(ss.str());
		if (!data["Styles"])
		{
			BOREALIS_CORE_ERROR("Style not found in file: {}", filepath);
			return false;
		}

		auto lightMode = data["LightMode"];

		sImGuiLightColours[ImGuiCol_Text] = lightMode["Text"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TextDisabled] = lightMode["TextDisabled"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_WindowBg] = lightMode["WindowBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ChildBg] = lightMode["ChildBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PopupBg] = lightMode["PopupBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Border] = lightMode["Border"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_BorderShadow] = lightMode["BorderShadow"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_FrameBg] = lightMode["FrameBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_FrameBgHovered] = lightMode["FrameBgHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_FrameBgActive] = lightMode["FrameBgActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TitleBg] = lightMode["TitleBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TitleBgActive] = lightMode["TitleBgActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TitleBgCollapsed] = lightMode["TitleBgCollapsed"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_MenuBarBg] = lightMode["MenuBarBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ScrollbarBg] = lightMode["ScrollbarBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ScrollbarGrab] = lightMode["ScrollbarGrab"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ScrollbarGrabHovered] = lightMode["ScrollbarGrabHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ScrollbarGrabActive] = lightMode["ScrollbarGrabActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_CheckMark] = lightMode["CheckMark"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_SliderGrab] = lightMode["SliderGrab"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_SliderGrabActive] = lightMode["SliderGrabActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Button] = lightMode["Button"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ButtonHovered] = lightMode["ButtonHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ButtonActive] = lightMode["ButtonActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Header] = lightMode["Header"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_HeaderHovered] = lightMode["HeaderHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_HeaderActive] = lightMode["HeaderActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Separator] = lightMode["Separator"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_SeparatorHovered] = lightMode["SeparatorHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_SeparatorActive] = lightMode["SeparatorActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ResizeGrip] = lightMode["ResizeGrip"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ResizeGripHovered] = lightMode["ResizeGripHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ResizeGripActive] = lightMode["ResizeGripActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_Tab] = lightMode["Tab"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabHovered] = lightMode["TabHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabActive] = lightMode["TabActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabUnfocused] = lightMode["TabUnfocused"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabUnfocusedActive] = lightMode["TabUnfocusedActive"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabSelected] = lightMode["TabSelected"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabSelectedOverline] = lightMode["TabSelectedOverline"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabDimmed] = lightMode["TabDimmed"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabDimmedSelected] = lightMode["TabDimmedSelected"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TabDimmedSelectedOverline] = lightMode["TabDimmedSelectedOverline"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PlotLines] = lightMode["PlotLines"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PlotLinesHovered] = lightMode["PlotLinesHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PlotHistogram] = lightMode["PlotHistogram"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_PlotHistogramHovered] = lightMode["PlotHistogramHovered"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TextSelectedBg] = lightMode["TextSelectedBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_DragDropTarget] = lightMode["DragDropTarget"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_NavHighlight] = lightMode["NavHighlight"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_NavWindowingHighlight] = lightMode["NavWindowingHighlight"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_NavWindowingDimBg] = lightMode["NavWindowingDimBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_ModalWindowDimBg] = lightMode["ModalWindowDimBg"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_DockingEmptyBg] = lightMode["DockingEmptyBackground"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_DockingPreview] = lightMode["DockingPreview"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableHeaderBg] = lightMode["TableHeaderBackground"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableBorderStrong] = lightMode["TableBorderStrong"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableBorderLight] = lightMode["TableBorderLight"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableRowBg] = lightMode["TableRowBackground"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TableRowBgAlt] = lightMode["TableRowBackgroundAlt"].as<ImVec4>();
		sImGuiLightColours[ImGuiCol_TextLink] = lightMode["TextLink"].as<ImVec4>();

		auto darkMode = data["DarkMode"];
		sImGuiDarkColours[ImGuiCol_Text] = darkMode["Text"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TextDisabled] = darkMode["TextDisabled"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_WindowBg] = darkMode["WindowBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ChildBg] = darkMode["ChildBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PopupBg] = darkMode["PopupBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Border] = darkMode["Border"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_BorderShadow] = darkMode["BorderShadow"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_FrameBg] = darkMode["FrameBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_FrameBgHovered] = darkMode["FrameBgHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_FrameBgActive] = darkMode["FrameBgActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TitleBg] = darkMode["TitleBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TitleBgActive] = darkMode["TitleBgActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TitleBgCollapsed] = darkMode["TitleBgCollapsed"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_MenuBarBg] = darkMode["MenuBarBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ScrollbarBg] = darkMode["ScrollbarBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ScrollbarGrab] = darkMode["ScrollbarGrab"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ScrollbarGrabHovered] = darkMode["ScrollbarGrabHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ScrollbarGrabActive] = darkMode["ScrollbarGrabActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_CheckMark] = darkMode["CheckMark"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_SliderGrab] = darkMode["SliderGrab"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_SliderGrabActive] = darkMode["SliderGrabActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Button] = darkMode["Button"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ButtonHovered] = darkMode["ButtonHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ButtonActive] = darkMode["ButtonActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Header] = darkMode["Header"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_HeaderHovered] = darkMode["HeaderHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_HeaderActive] = darkMode["HeaderActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Separator] = darkMode["Separator"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_SeparatorHovered] = darkMode["SeparatorHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_SeparatorActive] = darkMode["SeparatorActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ResizeGrip] = darkMode["ResizeGrip"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ResizeGripHovered] = darkMode["ResizeGripHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ResizeGripActive] = darkMode["ResizeGripActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_Tab] = darkMode["Tab"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabHovered] = darkMode["TabHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabActive] = darkMode["TabActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabUnfocused] = darkMode["TabUnfocused"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabUnfocusedActive] = darkMode["TabUnfocusedActive"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabSelected] = darkMode["TabSelected"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabSelectedOverline] = darkMode["TabSelectedOverline"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabDimmed] = darkMode["TabDimmed"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabDimmedSelected] = darkMode["TabDimmedSelected"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TabDimmedSelectedOverline] = darkMode["TabDimmedSelectedOverline"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PlotLines] = darkMode["PlotLines"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PlotLinesHovered] = darkMode["PlotLinesHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PlotHistogram] = darkMode["PlotHistogram"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_PlotHistogramHovered] = darkMode["PlotHistogramHovered"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TextSelectedBg] = darkMode["TextSelectedBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_DragDropTarget] = darkMode["DragDropTarget"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_NavHighlight] = darkMode["NavHighlight"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_NavWindowingHighlight] = darkMode["NavWindowingHighlight"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_NavWindowingDimBg] = darkMode["NavWindowingDimBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_ModalWindowDimBg] = darkMode["ModalWindowDimBg"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_DockingEmptyBg] = darkMode["DockingEmptyBackground"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_DockingPreview] = darkMode["DockingPreview"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableHeaderBg] = darkMode["TableHeaderBackground"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableBorderStrong] = darkMode["TableBorderStrong"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableBorderLight] = darkMode["TableBorderLight"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableRowBg] = darkMode["TableRowBackground"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TableRowBgAlt] = darkMode["TableRowBackgroundAlt"].as<ImVec4>();
		sImGuiDarkColours[ImGuiCol_TextLink] = darkMode["TextLink"].as<ImVec4>();

		return true;
	}


	bool Serialiser::SerialiseEditorStyle()
	{
		std::string filepath = "settings/style.setting";
		YAML::Emitter out;
		out << YAML::BeginMap
			<< YAML::Key << "Styles"  << YAML::Value << "StyleType: Light/Dark"
			<< YAML::Key << "LightMode" << YAML::Value << YAML::BeginMap;


		ImGuiStyle& style = ImGui::GetStyle();
		out << YAML::Key << "Text" << YAML::Value << style.Colors[ImGuiCol_Text];
		out << YAML::Key << "TextDisabled" << YAML::Value << style.Colors[ImGuiCol_TextDisabled];
		out << YAML::Key << "WindowBg" << YAML::Value << style.Colors[ImGuiCol_WindowBg];
		out << YAML::Key << "ChildBg" << YAML::Value << style.Colors[ImGuiCol_ChildBg];
		out << YAML::Key << "PopupBg" << YAML::Value << style.Colors[ImGuiCol_PopupBg];
		out << YAML::Key << "Border" << YAML::Value << style.Colors[ImGuiCol_Border];
		out << YAML::Key << "BorderShadow" << YAML::Value << style.Colors[ImGuiCol_BorderShadow];
		out << YAML::Key << "FrameBg" << YAML::Value << style.Colors[ImGuiCol_FrameBg];
		out << YAML::Key << "FrameBgHovered" << YAML::Value << style.Colors[ImGuiCol_FrameBgHovered];
		out << YAML::Key << "FrameBgActive" << YAML::Value << style.Colors[ImGuiCol_FrameBgActive];
		out << YAML::Key << "TitleBg" << YAML::Value << style.Colors[ImGuiCol_TitleBg];	
		out << YAML::Key << "TitleBgActive" << YAML::Value << style.Colors[ImGuiCol_TitleBgActive];
		out << YAML::Key << "TitleBgCollapsed" << YAML::Value << style.Colors[ImGuiCol_TitleBgCollapsed];
		out << YAML::Key << "MenuBarBg" << YAML::Value << style.Colors[ImGuiCol_MenuBarBg];
		out << YAML::Key << "ScrollbarBg" << YAML::Value << style.Colors[ImGuiCol_ScrollbarBg];
		out << YAML::Key << "ScrollbarGrab" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrab];
		out << YAML::Key << "ScrollbarGrabHovered" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrabHovered];
		out << YAML::Key << "ScrollbarGrabActive" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrabActive];
		out << YAML::Key << "CheckMark" << YAML::Value << style.Colors[ImGuiCol_CheckMark];
		out << YAML::Key << "SliderGrab" << YAML::Value << style.Colors[ImGuiCol_SliderGrab];
		out << YAML::Key << "SliderGrabActive" << YAML::Value << style.Colors[ImGuiCol_SliderGrabActive];
		out << YAML::Key << "Button" << YAML::Value << style.Colors[ImGuiCol_Button];
		out << YAML::Key << "ButtonHovered" << YAML::Value << style.Colors[ImGuiCol_ButtonHovered];
		out << YAML::Key << "ButtonActive" << YAML::Value << style.Colors[ImGuiCol_ButtonActive];
		out << YAML::Key << "Header" << YAML::Value << style.Colors[ImGuiCol_Header];
		out << YAML::Key << "HeaderHovered" << YAML::Value << style.Colors[ImGuiCol_HeaderHovered];
		out << YAML::Key << "HeaderActive" << YAML::Value << style.Colors[ImGuiCol_HeaderActive];
		out << YAML::Key << "Separator" << YAML::Value << style.Colors[ImGuiCol_Separator];
		out << YAML::Key << "SeparatorHovered" << YAML::Value << style.Colors[ImGuiCol_SeparatorHovered];
		out << YAML::Key << "SeparatorActive" << YAML::Value << style.Colors[ImGuiCol_SeparatorActive];
		out << YAML::Key << "ResizeGrip" << YAML::Value << style.Colors[ImGuiCol_ResizeGrip];
		out << YAML::Key << "ResizeGripHovered" << YAML::Value << style.Colors[ImGuiCol_ResizeGripHovered];
		out << YAML::Key << "ResizeGripActive" << YAML::Value << style.Colors[ImGuiCol_ResizeGripActive];
		out << YAML::Key << "Tab" << YAML::Value << style.Colors[ImGuiCol_Tab];
		out << YAML::Key << "TabHovered" << YAML::Value << style.Colors[ImGuiCol_TabHovered];
		out << YAML::Key << "TabActive" << YAML::Value << style.Colors[ImGuiCol_TabActive];
		out << YAML::Key << "TabUnfocused" << YAML::Value << style.Colors[ImGuiCol_TabUnfocused];
		out << YAML::Key << "TabUnfocusedActive" << YAML::Value << style.Colors[ImGuiCol_TabUnfocusedActive];
		out << YAML::Key << "TabSelected" << YAML::Value << style.Colors[ImGuiCol_TabSelected];
		out << YAML::Key << "TabSelectedOverline" << YAML::Value << style.Colors[ImGuiCol_TabSelectedOverline];
		out << YAML::Key << "TabDimmed" << YAML::Value << style.Colors[ImGuiCol_TabDimmed];
		out << YAML::Key << "TabDimmedSelected" << YAML::Value << style.Colors[ImGuiCol_TabDimmedSelected];
		out << YAML::Key << "TabDimmedSelectedOverline" << YAML::Value << style.Colors[ImGuiCol_TabDimmedSelectedOverline];
		out << YAML::Key << "PlotLines" << YAML::Value << style.Colors[ImGuiCol_PlotLines];
		out << YAML::Key << "PlotLinesHovered" << YAML::Value << style.Colors[ImGuiCol_PlotLinesHovered];
		out << YAML::Key << "PlotHistogram" << YAML::Value << style.Colors[ImGuiCol_PlotHistogram];
		out << YAML::Key << "PlotHistogramHovered" << YAML::Value << style.Colors[ImGuiCol_PlotHistogramHovered];
		out << YAML::Key << "TextSelectedBg" << YAML::Value << style.Colors[ImGuiCol_TextSelectedBg];
		out << YAML::Key << "DragDropTarget" << YAML::Value << style.Colors[ImGuiCol_DragDropTarget];
		out << YAML::Key << "NavHighlight" << YAML::Value << style.Colors[ImGuiCol_NavHighlight];
		out << YAML::Key << "NavWindowingHighlight" << YAML::Value << style.Colors[ImGuiCol_NavWindowingHighlight];
		out << YAML::Key << "NavWindowingDimBg" << YAML::Value << style.Colors[ImGuiCol_NavWindowingDimBg];
		out << YAML::Key << "ModalWindowDimBg" << YAML::Value << style.Colors[ImGuiCol_ModalWindowDimBg];
		out << YAML::Key << "DockingEmptyBackground" << YAML::Value << style.Colors[ImGuiCol_DockingEmptyBg];
		out << YAML::Key << "DockingPreview" << YAML::Value << style.Colors[ImGuiCol_DockingPreview];
		out << YAML::Key << "TableHeaderBackground" << YAML::Value << style.Colors[ImGuiCol_TableHeaderBg];
		out << YAML::Key << "TableBorderStrong" << YAML::Value << style.Colors[ImGuiCol_TableBorderStrong];
		out << YAML::Key << "TableBorderLight" << YAML::Value << style.Colors[ImGuiCol_TableBorderLight];
		out << YAML::Key << "TableRowBackground" << YAML::Value << style.Colors[ImGuiCol_TableRowBg];
		out << YAML::Key << "TableRowBackgroundAlt" << YAML::Value << style.Colors[ImGuiCol_TableRowBgAlt];
		out << YAML::Key << "TextLink" << YAML::Value << style.Colors[ImGuiCol_TextLink];

		out << YAML::EndMap;

		out << YAML::Key << "DarkMode" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Text" << YAML::Value << style.Colors[ImGuiCol_Text];
		out << YAML::Key << "TextDisabled" << YAML::Value << style.Colors[ImGuiCol_TextDisabled];
		out << YAML::Key << "WindowBg" << YAML::Value << style.Colors[ImGuiCol_WindowBg];
		out << YAML::Key << "ChildBg" << YAML::Value << style.Colors[ImGuiCol_ChildBg];
		out << YAML::Key << "PopupBg" << YAML::Value << style.Colors[ImGuiCol_PopupBg];
		out << YAML::Key << "Border" << YAML::Value << style.Colors[ImGuiCol_Border];
		out << YAML::Key << "BorderShadow" << YAML::Value << style.Colors[ImGuiCol_BorderShadow];
		out << YAML::Key << "FrameBg" << YAML::Value << style.Colors[ImGuiCol_FrameBg];
		out << YAML::Key << "FrameBgHovered" << YAML::Value << style.Colors[ImGuiCol_FrameBgHovered];
		out << YAML::Key << "FrameBgActive" << YAML::Value << style.Colors[ImGuiCol_FrameBgActive];
		out << YAML::Key << "TitleBg" << YAML::Value << style.Colors[ImGuiCol_TitleBg];
		out << YAML::Key << "TitleBgActive" << YAML::Value << style.Colors[ImGuiCol_TitleBgActive];
		out << YAML::Key << "TitleBgCollapsed" << YAML::Value << style.Colors[ImGuiCol_TitleBgCollapsed];
		out << YAML::Key << "MenuBarBg" << YAML::Value << style.Colors[ImGuiCol_MenuBarBg];
		out << YAML::Key << "ScrollbarBg" << YAML::Value << style.Colors[ImGuiCol_ScrollbarBg];
		out << YAML::Key << "ScrollbarGrab" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrab];
		out << YAML::Key << "ScrollbarGrabHovered" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrabHovered];
		out << YAML::Key << "ScrollbarGrabActive" << YAML::Value << style.Colors[ImGuiCol_ScrollbarGrabActive];
		out << YAML::Key << "CheckMark" << YAML::Value << style.Colors[ImGuiCol_CheckMark];
		out << YAML::Key << "SliderGrab" << YAML::Value << style.Colors[ImGuiCol_SliderGrab];
		out << YAML::Key << "SliderGrabActive" << YAML::Value << style.Colors[ImGuiCol_SliderGrabActive];
		out << YAML::Key << "Button" << YAML::Value << style.Colors[ImGuiCol_Button];
		out << YAML::Key << "ButtonHovered" << YAML::Value << style.Colors[ImGuiCol_ButtonHovered];
		out << YAML::Key << "ButtonActive" << YAML::Value << style.Colors[ImGuiCol_ButtonActive];
		out << YAML::Key << "Header" << YAML::Value << style.Colors[ImGuiCol_Header];
		out << YAML::Key << "HeaderHovered" << YAML::Value << style.Colors[ImGuiCol_HeaderHovered];
		out << YAML::Key << "HeaderActive" << YAML::Value << style.Colors[ImGuiCol_HeaderActive];
		out << YAML::Key << "Separator" << YAML::Value << style.Colors[ImGuiCol_Separator];
		out << YAML::Key << "SeparatorHovered" << YAML::Value << style.Colors[ImGuiCol_SeparatorHovered];
		out << YAML::Key << "SeparatorActive" << YAML::Value << style.Colors[ImGuiCol_SeparatorActive];
		out << YAML::Key << "ResizeGrip" << YAML::Value << style.Colors[ImGuiCol_ResizeGrip];
		out << YAML::Key << "ResizeGripHovered" << YAML::Value << style.Colors[ImGuiCol_ResizeGripHovered];
		out << YAML::Key << "ResizeGripActive" << YAML::Value << style.Colors[ImGuiCol_ResizeGripActive];
		out << YAML::Key << "Tab" << YAML::Value << style.Colors[ImGuiCol_Tab];
		out << YAML::Key << "TabHovered" << YAML::Value << style.Colors[ImGuiCol_TabHovered];
		out << YAML::Key << "TabActive" << YAML::Value << style.Colors[ImGuiCol_TabActive];
		out << YAML::Key << "TabUnfocused" << YAML::Value << style.Colors[ImGuiCol_TabUnfocused];
		out << YAML::Key << "TabUnfocusedActive" << YAML::Value << style.Colors[ImGuiCol_TabUnfocusedActive];
		out << YAML::Key << "TabSelected" << YAML::Value << style.Colors[ImGuiCol_TabSelected];
		out << YAML::Key << "TabSelectedOverline" << YAML::Value << style.Colors[ImGuiCol_TabSelectedOverline];
		out << YAML::Key << "TabDimmed" << YAML::Value << style.Colors[ImGuiCol_TabDimmed];
		out << YAML::Key << "TabDimmedSelected" << YAML::Value << style.Colors[ImGuiCol_TabDimmedSelected];
		out << YAML::Key << "TabDimmedSelectedOverline" << YAML::Value << style.Colors[ImGuiCol_TabDimmedSelectedOverline];
		out << YAML::Key << "PlotLines" << YAML::Value << style.Colors[ImGuiCol_PlotLines];
		out << YAML::Key << "PlotLinesHovered" << YAML::Value << style.Colors[ImGuiCol_PlotLinesHovered];
		out << YAML::Key << "PlotHistogram" << YAML::Value << style.Colors[ImGuiCol_PlotHistogram];
		out << YAML::Key << "PlotHistogramHovered" << YAML::Value << style.Colors[ImGuiCol_PlotHistogramHovered];
		out << YAML::Key << "TextSelectedBg" << YAML::Value << style.Colors[ImGuiCol_TextSelectedBg];
		out << YAML::Key << "DragDropTarget" << YAML::Value << style.Colors[ImGuiCol_DragDropTarget];
		out << YAML::Key << "NavHighlight" << YAML::Value << style.Colors[ImGuiCol_NavHighlight];
		out << YAML::Key << "NavWindowingHighlight" << YAML::Value << style.Colors[ImGuiCol_NavWindowingHighlight];
		out << YAML::Key << "NavWindowingDimBg" << YAML::Value << style.Colors[ImGuiCol_NavWindowingDimBg];
		out << YAML::Key << "ModalWindowDimBg" << YAML::Value << style.Colors[ImGuiCol_ModalWindowDimBg];
		out << YAML::Key << "DockingEmptyBackground" << YAML::Value << style.Colors[ImGuiCol_DockingEmptyBg];
		out << YAML::Key << "DockingPreview" << YAML::Value << style.Colors[ImGuiCol_DockingPreview];
		out << YAML::Key << "TableHeaderBackground" << YAML::Value << style.Colors[ImGuiCol_TableHeaderBg];
		out << YAML::Key << "TableBorderStrong" << YAML::Value << style.Colors[ImGuiCol_TableBorderStrong];
		out << YAML::Key << "TableBorderLight" << YAML::Value << style.Colors[ImGuiCol_TableBorderLight];
		out << YAML::Key << "TableRowBackground" << YAML::Value << style.Colors[ImGuiCol_TableRowBg];
		out << YAML::Key << "TableRowBackgroundAlt" << YAML::Value << style.Colors[ImGuiCol_TableRowBgAlt];
		out << YAML::Key << "TextLink" << YAML::Value << style.Colors[ImGuiCol_TextLink];

		out << YAML::EndMap
			<< YAML::EndMap;

		// Create directory if doesnt exist
		std::filesystem::path fileSystemPaths = filepath;
		std::filesystem::create_directories(fileSystemPaths.parent_path());

		std::ofstream outStream(filepath);
		outStream << out.c_str();
		outStream.close();
		return true;
	}
}