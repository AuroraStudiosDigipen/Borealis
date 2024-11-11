/******************************************************************************/
/*!
\file		Scripting System.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	September 06, 2024
\brief		Defines the class for the Scripting System

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/


#include <BorealisPCH.hpp>
#include <typeinfo>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/metadata.h>
#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptingUtils.hpp>
#include <Scripting/ScriptingExposedInternal.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <Core/Core.hpp>
#include <Core/LoggerSystem.hpp>
#include <Scene/SceneManager.hpp>
#include <Scene/Components.hpp>
#include <Scene/Entity.hpp>

namespace Borealis
{

	 std::unordered_map<std::string, Ref<ScriptClass>> ScriptingSystem::mScriptClasses;
	 std::unordered_map<std::string, std::unordered_set<UUID>> ScriptingSystem::mEntityScriptMap; // Scripts attached to entities


	struct ScriptingSystemData
	{
		MonoDomain* mRootDomain = nullptr;
		MonoDomain* mAppDomain = nullptr;
		MonoAssembly* mRoslynAssembly = nullptr;
		std::vector<MonoAssembly*> mAssemblies;
	};


	static ScriptingSystemData* sData;

	void ScriptingSystem::RegisterCSharpClass(ScriptClass klass)
	{
		auto scriptClass = MakeRef<ScriptClass>(klass);
		mScriptClasses[klass.GetKlassName()] = scriptClass;

		void* iterator = nullptr;
		while (MonoClassField* field = mono_class_get_fields(scriptClass->GetMonoClass(), &iterator))
		{
			auto fieldName = mono_field_get_name(field);
			auto fieldType = mono_field_get_type(field);

			ScriptFieldType SFType = MonoType2ScriptFieldType(fieldType);
			scriptClass->mFields[fieldName] = ScriptField{ SFType, fieldName, field };
			scriptClass->mOrder.push_back(fieldName);
		}
	}

	void ScriptingSystem::InitCoreAssembly()
	{
		
		InstantiateClass(sData->mRoslynAssembly, sData->mAppDomain, "Borealis", "RoslynCompiler");
		
	}

	bool ScriptingSystem::GetEnabled(Ref<ScriptInstance> instance)
	{
		auto klass = mono_class_from_name(mono_assembly_get_image(sData->mRoslynAssembly), "Borealis", "Behaviour");
		auto field = mono_class_get_field_from_name(klass, "enabled");
		bool output = true;
		char s_fieldValueBuffer[1];
		mono_field_get_value(instance->GetInstance(), field, s_fieldValueBuffer);
		output = *(bool*)s_fieldValueBuffer;
		return output;
	}

	void ScriptingSystem::SetEnabled(Ref<ScriptInstance> instance, bool enabled)
	{
		auto klass = mono_class_from_name(mono_assembly_get_image(sData->mRoslynAssembly), "Borealis", "Behaviour");
		auto field = mono_class_get_field_from_name(klass, "enabled");
		mono_field_set_value(instance->GetInstance(), field, &enabled);
	}

	static void RegisterCSharpScriptsFromAssembly(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		void* iterator = nullptr;
		auto assemblyImage = mono_assembly_get_image(sData->mRoslynAssembly);

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(assemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* behaviourClass = mono_class_from_name(assemblyImage, "Borealis", "MonoBehaviour");

		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "MonoBehaviour", assembly));
		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "Behaviour", assembly));
		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "GameObject", assembly));
		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "Object", assembly));


		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(assemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(assemblyImage, cols[MONO_TYPEDEF_NAME]);

			MonoClass* currClass = mono_class_from_name(assemblyImage, nameSpace, className);

			if (currClass == behaviourClass)
			{
				continue;
			}
			if (mono_class_is_subclass_of(currClass, behaviourClass, false))
			{
				ScriptingSystem::RegisterCSharpClass(ScriptClass(nameSpace, className, assembly));
			}
			else
			{
				continue;
			}
		}
	}
	

	template <typename T>
	static void RegisterComponent()
	{
		std::string typeName = typeid(T).name();
		

		if (typeName.find("::") != std::string::npos)
		{
			typeName = "Borealis." + typeName.substr(typeName.find("::") + 2);
		}
		if (typeName.find("Component") != std::string::npos)
		{
			typeName = typeName.substr(0, typeName.find("Component"));
		}
		MonoType* managedType = mono_reflection_type_from_name(typeName.data(), mono_assembly_get_image(sData->mRoslynAssembly));

		if (managedType)
		{
			GCFM::mHasComponentFunctions[managedType] = [](Entity& entity) { return entity.HasComponent<T>(); };
			GCFM::mAddComponentFunctions[managedType] = [](Entity& entity) { entity.AddComponent<T>(); };
			GCFM::mRemoveComponentFunctions[managedType] = [](Entity& entity) { entity.GetComponent<T>(); };
		}
		else
		{
			BOREALIS_CORE_WARN("Failed to register component {0}", typeName);
		}
	}

	static void RegisterComponents()
	{
		RegisterComponent<TransformComponent>();
		//RegisterComponent<CameraComponent>();
		RegisterComponent<SpriteRendererComponent>();
		//RegisterComponent<IDComponent>();
		//RegisterComponent<TagComponent>();
		//RegisterComponent<CircleRendererComponent>();
		//RegisterComponent<MeshFilterComponent>();
		//RegisterComponent<MeshRendererComponent>();
		//RegisterComponent<BoxColliderComponent>();
		//RegisterComponent<CapsuleColliderComponent>();
		//RegisterComponent<RigidBodyComponent>();
		//RegisterComponent<LightComponent>();

	}

	void ScriptingSystem::Init()
	{
		sData = new ScriptingSystemData();
		InitMono();
	}
	void ScriptingSystem::Free()
	{
		FreeMono();
		delete sData;
	}

	void ScriptingSystem::Update(float deltaTime)
	{
	}

	template <typename T>
	static void GetComponent(UUID id, T* component)
	{
		Scene* scene = SceneManager::GetActiveScene().get();
		Entity entity = scene->GetEntityByUUID(id);
		*component = entity.GetComponent<T>();
	}


	void ScriptingSystem::InitMono()
	{
		mono_set_assemblies_path("mono/lib/4.5");
		sData->mRootDomain = mono_jit_init("BorealisJitRuntime");
		
		BOREALIS_CORE_ASSERT(sData->mRootDomain, "Failed to initialize Mono runtime");

		char friendlyName[] = "BorealisAppDomain";
		sData->mAppDomain = mono_domain_create_appdomain(friendlyName, nullptr);
		mono_domain_set(sData->mAppDomain, true);

		RegisterInternals();

		sData->mRoslynAssembly = LoadCSharpAssembly("resources/scripts/core/BorealisScriptCore.dll");
		
		// Add all internal functions here
		RegisterComponents();

		// From Runtime assemblies as well
		RegisterCSharpScriptsFromAssembly(sData->mRoslynAssembly);
	}
	void ScriptingSystem::FreeMono()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(sData->mAppDomain);
		sData->mAppDomain = nullptr;

		mono_jit_cleanup(sData->mRootDomain);
		sData->mRootDomain = nullptr;
	}

}