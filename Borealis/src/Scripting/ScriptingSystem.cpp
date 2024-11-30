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
#include <mono/metadata/threads.h>
#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptingUtils.hpp>
#include <Scripting/ScriptingExposedInternal.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <Core/Core.hpp>
#include <Core/LoggerSystem.hpp>
#include <Scene/SceneManager.hpp>
#include <Scene/Components.hpp>
#include <Scene/Entity.hpp>
#include <AI/BehaviourTree/BehaviourNode.hpp>
#include <AI/BehaviourTree/BTreeFactory.hpp>

#include <Core/Project.hpp>

namespace Borealis
{

	 std::unordered_map<std::string, Ref<ScriptClass>> ScriptingSystem::mScriptClasses;
	 std::unordered_map<std::string, std::unordered_set<UUID>> ScriptingSystem::mEntityScriptMap; // Scripts attached to entities


	struct ScriptingSystemData
	{
		MonoDomain* mRootDomain = nullptr;
		MonoDomain* mAppDomain = nullptr;
		MonoAssembly* mRoslynAssembly = nullptr;
		MonoAssembly* mScriptAssembly = nullptr;
		std::vector <std::string> mCSharpList;
	};


	static ScriptingSystemData* sData;

	void ScriptingSystem::RegisterCSharpClass(ScriptClass klass)
	{
		Ref<ScriptClass> scriptClass;
		if (mScriptClasses.find(klass.GetKlassName()) != mScriptClasses.end())
		{
			mScriptClasses[klass.GetKlassName()]->SetMonoClass(klass.GetMonoClass());
			scriptClass = mScriptClasses[klass.GetKlassName()];
			scriptClass->mFields.clear();
			scriptClass->mOrder.clear();
		}
		else
		{
			scriptClass = MakeRef<ScriptClass>(klass);
			mScriptClasses[klass.GetKlassName()] = scriptClass;
		}		

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

	void* ScriptingSystem::InitCoreAssembly()
	{
		
		return InstantiateClass(sData->mRoslynAssembly, sData->mAppDomain, "Borealis", "RoslynCompiler");
		
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

	void ScriptingSystem::CompileCSharpQueue(std::string cSharpPath)
	{
		auto thread = mono_thread_attach(sData->mAppDomain);
		// Compile the C# script
		mono_domain_set(sData->mAppDomain, true);
		MonoObject* monoCompiler = (MonoObject*)InitCoreAssembly();
		MonoArray* monoFilePaths = mono_array_new(mono_domain_get(), mono_get_string_class(), sData->mCSharpList.size());

		for (size_t i = 0; i < sData->mCSharpList.size(); ++i) {
			MonoString* monoString = mono_string_new(mono_domain_get(), sData->mCSharpList[i].c_str());
			mono_array_set(monoFilePaths, MonoString*, i, monoString);
		}

		MonoString* str2 = mono_string_new(mono_domain_get(), "CSharp_Assembly");

		void* args[3] = { monoFilePaths, str2 };

		auto method = mono_class_get_method_from_name(GetScriptClassUtils("RoslynCompiler")->GetMonoClass(), "CompileCode", 2);
		MonoObject* result = mono_runtime_invoke(method, monoCompiler, args, nullptr);

		if (result) {
			MonoArray* byteArray = (MonoArray*)result;
			int length = mono_array_length(byteArray);
			void* data = mono_array_addr_with_size(byteArray, 0, 0);

			std::ofstream file(cSharpPath, std::ios::binary);
			if (file.is_open())
			{
				file.write((char*)data, length);
				file.close();
			}
		}

		sData->mCSharpList.clear();
		mono_thread_detach(thread);
	}

	void ScriptingSystem::PushCSharpQueue(std::string filepath)
	{
		sData->mCSharpList.push_back(filepath);
	}


	static void RegisterCSharpScriptsFromAssembly(MonoAssembly* assembly)
	{
		void* iterator = nullptr;
		auto assemblyImage = mono_assembly_get_image(assembly);

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(assemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* behaviourClass = mono_class_from_name(assemblyImage, "Borealis", "MonoBehaviour");
		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "RoslynCompiler", assembly));
		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "MonoBehaviour", assembly));
		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "Behaviour", assembly));
		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "GameObject", assembly));
		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "Object", assembly));

		ScriptingSystem::RegisterCSharpClass(ScriptClass("Borealis", "BehaviourNode", assembly));

		MonoClass* attributeClass = mono_class_from_name(mono_get_corlib(), "System", "Attribute");
		MonoClass* behaviourNodeClass = mono_class_from_name(assemblyImage, "Borealis", "BehaviourNode");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(assemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(assemblyImage, cols[MONO_TYPEDEF_NAME]);

			MonoClass* currClass = mono_class_from_name(assemblyImage, nameSpace, className);

			if (!currClass)
			{
				continue;
			}
			if (mono_class_is_subclass_of(currClass, attributeClass, false))
			{
				// Register attribute
				LoadScriptAttribute(currClass);
			}

			else
			{
				continue;
			}
		}
		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(assemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(assemblyImage, cols[MONO_TYPEDEF_NAME]);

			MonoClass* currClass = mono_class_from_name(assemblyImage, nameSpace, className);
			if (!currClass)
			{
				continue;
			}
			MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_class(currClass);
			if (attributeInfo)
			{
				if (mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("NativeComponent")))
				{
					ScriptingSystem::RegisterCSharpClass(ScriptClass(nameSpace, className, sData->mRoslynAssembly));

				}

				if (mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("AssetField")))
				{
					ScriptingSystem::RegisterCSharpClass(ScriptClass(nameSpace, className, sData->mRoslynAssembly));

				}
			}
		}
	}


	void ScriptingSystem::LoadScriptAssemblies(std::string filepath)
	{
		// Check if file exists
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			BOREALIS_CORE_ERROR("Failed to open file {0}", filepath);
			return;
		}


		auto thread = mono_thread_attach(sData->mRootDomain);
		LoadScriptAssembliesNonThreaded(filepath);

		mono_thread_detach(thread);
	}

	void ScriptingSystem::LoadScriptAssembliesNonThreaded(std::string filepath)
	{
		mono_domain_set(sData->mRootDomain, true);
		mono_domain_unload(sData->mAppDomain);
		char friendlyName[] = "BorealisAppDomain";
		sData->mAppDomain = mono_domain_create_appdomain(friendlyName, nullptr);
		mono_domain_set(sData->mAppDomain, true);
		sData->mRoslynAssembly = LoadCSharpAssembly("resources/scripts/core/BorealisScriptCore.dll");
		RegisterCSharpScriptsFromAssembly(sData->mRoslynAssembly);

		sData->mScriptAssembly = LoadCSharpAssembly(filepath);

		void* iterator = nullptr;
		auto assemblyImage = mono_assembly_get_image(sData->mScriptAssembly);

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(assemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		MonoClass* monoBehaviour = GetScriptClassUtils("MonoBehaviour")->GetMonoClass();
		MonoClass* behaviourNodeClass = GetScriptClassUtils("BehaviourNode")->GetMonoClass();	

		BTreeFactory::Instance().mControlFlowNames.clear();
		BTreeFactory::Instance().mDecoratorNames.clear();
		BTreeFactory::Instance().mLeafNames.clear();

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(assemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(assemblyImage, cols[MONO_TYPEDEF_NAME]);
			
			MonoClass* currClass = mono_class_from_name(assemblyImage, nameSpace, className);
			if (!currClass)
			{
				continue;
			}

			
			if (mono_class_is_subclass_of(currClass, monoBehaviour, false))
			{
				// Register attribute
				ScriptingSystem::RegisterCSharpClass(ScriptClass(nameSpace, className, sData->mScriptAssembly));
			}
			else if (mono_class_is_subclass_of(currClass, behaviourNodeClass, false))
			{
				// Register attribute
				ScriptingSystem::RegisterCSharpClass(ScriptClass(nameSpace, className, sData->mScriptAssembly));

				// Check attribute
				MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_class(currClass);
				if (attributeInfo)
				{
					auto attributeClass = mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("BTNodeClass"));
					if (attributeClass)
					{
						auto field = mono_class_get_field_from_name(mono_object_get_class(attributeClass), "nodeType");
						NodeType nodeType;
						mono_field_get_value(attributeClass, field, &nodeType);
						switch (nodeType)
						{
						case NodeType::CONTROLFLOW:
							BTreeFactory::Instance().mControlFlowNames.insert(className);
							break;
						case NodeType::DECORATOR:
							BTreeFactory::Instance().mDecoratorNames.insert(className);
							break;
						case NodeType::LEAF:
							BTreeFactory::Instance().mLeafNames.insert(className);
							break;
						default:
							break;
						}
					}
				}
			}
			else
			{
				continue;
			}
		}
	}

	void ScriptingSystem::AttachAppDomain()
	{
		mono_domain_set(sData->mAppDomain, true);
	}

	void ScriptingSystem::DetachAppDomain()
	{
		mono_domain_set(sData->mRootDomain, true);
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
			auto name = mono_type_get_name(managedType);
			std::string strName(name);
			mono_free(name);
			GCFM::mHasComponentFunctions[strName] = [](Entity& entity) { return entity.HasComponent<T>(); };
			GCFM::mAddComponentFunctions[strName] = [](Entity& entity) { entity.AddComponent<T>(); };
			GCFM::mRemoveComponentFunctions[strName] = [](Entity& entity) { entity.GetComponent<T>(); };
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
		RegisterComponent<MeshRendererComponent>();
		//RegisterComponent<BoxColliderComponent>();
		//RegisterComponent<CapsuleColliderComponent>();
		RegisterComponent<RigidBodyComponent>();
		RegisterComponent<OutLineComponent>();

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
		sData->mAppDomain = mono_domain_create_appdomain(friendlyName, nullptr); // Compiler
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

	void ScriptingSystem::Reload(AssetMetaData const& assetMetaData)
	{
		for (auto [assetHandle, assetMetaDataFromRegistry] : Project::GetEditorAssetsManager()->GetAssetRegistry())
		{
			if (assetMetaDataFromRegistry.Type == AssetType::Script)
			{
				ScriptingSystem::PushCSharpQueue(assetMetaDataFromRegistry.SourcePath.string());
			}
		}
		ScriptingSystem::CompileCSharpQueue(Project::GetProjectPath() + "/Cache/CSharp_Assembly.dll");
		ScriptingSystem::LoadScriptAssemblies(Project::GetProjectPath() + "/Cache/CSharp_Assembly.dll");
	}
	void* ScriptingSystem::GetScriptDomain()
	{
		return sData->mAppDomain;
	}
}