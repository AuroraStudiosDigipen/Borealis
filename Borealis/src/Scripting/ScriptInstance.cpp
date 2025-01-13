/******************************************************************************
/*!
\file       ScriptInstance.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Defines the Script Instance class which is an instance of a script
			component that is attached to an entity.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <Scripting/ScriptingUtils.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <mono/metadata/object.h>
#include <mono/jit/jit.h>

namespace Borealis
{
	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass)
	{
		mScriptClass = scriptClass;
		mGcHandle = mono_gchandle_new(scriptClass->Instantiate(), false);
		//mScriptClass->InvokeMethod(mInstance, mConstructor, &param);
	}

	ScriptInstance::~ScriptInstance()
	{
		mono_gchandle_free(mGcHandle);
	}

	ScriptInstance::ScriptInstance(MonoObject* Instance)
	{
		MonoClass* klass = mono_object_get_class(Instance);
		mScriptClass = ScriptingSystem::GetScriptClass(mono_class_get_name(klass));
		mGcHandle = mono_gchandle_new(Instance, false);
	}

	void ScriptInstance::Init(uint64_t UUIDAddress)
	{
		mono_runtime_object_init(GetInstance());

		uint64_t number = UUIDAddress; // The integer you want to pass
		void* params[1];
		params[0] = &number;
		auto Method = mono_class_get_method_from_name(GetScriptClassUtils("MonoBehaviour")->GetMonoClass(), "SetInstanceID", 1);
		mono_runtime_invoke(Method, GetInstance(), params, nullptr);
	}



	Ref<ScriptClass> ScriptInstance::GetScriptClass()
	{
		return mScriptClass;
	}
	MonoClass* ScriptInstance::GetMonoClass() const
	{
		return mScriptClass->GetMonoClass();
	}
	MonoObject* ScriptInstance::GetInstance()
	{
		return mono_gchandle_get_target(mGcHandle);
	}
	std::string ScriptInstance::GetKlassName() const
	{
		return mScriptClass->GetKlassName();
	}
	bool ScriptInstance::SetFieldValueInternal(const std::string& name, void* value)
	{
		const auto& fields = mScriptClass->mFields;
		auto it = fields.find(name);
		if (it == fields.end())
		{
			BOREALIS_CORE_ERROR("Field {0} not found in class {1}", name, mScriptClass->GetKlassName());
			return false;
		}

		const ScriptField& field = it->second;
		mono_field_set_value(GetInstance(), field.mMonoFieldType, value);
		return true;
	}
	bool ScriptInstance::SetFieldValueInternal(const std::string& name, std::string value)
	{
		const auto& fields = mScriptClass->mFields;
		auto it = fields.find(name);
		if (it == fields.end())
		{
			BOREALIS_CORE_ERROR("Field {0} not found in class {1}", name, mScriptClass->GetKlassName());
			return false;
		}

		const ScriptField& field = it->second;
		MonoString* str = mono_string_new(mono_domain_get(), value.c_str());
		mono_field_set_value(GetInstance(), field.mMonoFieldType, str);
		return true;

	}
	bool ScriptInstance::GetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = mScriptClass->mFields;
		auto it = fields.find(name);
		if (it == fields.end())
		{
			BOREALIS_CORE_ERROR("Field {0} not found in class {1}", name, mScriptClass->GetKlassName());
			return false;
		}

		const ScriptField& field = it->second;
		mono_field_get_value(GetInstance(), field.mMonoFieldType, s_fieldValueBuffer);
		return true;
	}

	bool ScriptInstance::GetFieldValueString(const std::string& name, std::string& output)
	{
		MonoString* monoString;
		const auto& fields = mScriptClass->mFields;
		auto it = fields.find(name);
		if (it == fields.end())
		{
			BOREALIS_CORE_ERROR("Field {0} not found in class {1}", name, mScriptClass->GetKlassName());
			return false;
		}

		const ScriptField& field = it->second;
		mono_field_get_value(GetInstance(), field.mMonoFieldType, &monoString);
		if (monoString)
		{
			char* str = mono_string_to_utf8(monoString);
			output = str;
			mono_free((void*)str);
			return true;
		}
		return false;
	}

	void ScriptInstance::ReplaceFieldValue(ScriptInstance* otherInstance, const std::string& name)
	{
		const auto& fields = mScriptClass->mFields;
		auto it = fields.find(name);
		if (it == fields.end())
		{
			BOREALIS_CORE_ERROR("Field {0} not found in class {1}", name, mScriptClass->GetKlassName());
			return;
		}

		const auto& otherFields = mScriptClass->mFields;
		auto it2 = otherFields.find(name);
		if (it == otherFields.end())
		{
			BOREALIS_CORE_ERROR("Field {0} not found in class {1}", name, otherInstance->GetScriptClass()->GetKlassName());
			return;
		}

		if (it->second.mMonoFieldType != otherFields.at(name).mMonoFieldType)
		{
			BOREALIS_CORE_ERROR("Field {0} in class {1} is not the same type as field {0} in class {1}", name, mScriptClass->GetKlassName(), name, otherInstance->GetScriptClass()->GetKlassName());
			return;
		}


		const ScriptField& field = it->second;

		auto fieldType = mono_field_get_type(field.mMonoFieldType);

		if (mono_type_is_reference(fieldType)) 
		{
			MonoObject* value_from_instance2 = mono_field_get_value_object(mono_get_root_domain(),it2->second.mMonoFieldType , otherInstance->GetInstance());
			// Set the value into the first instance (reference type)
			mono_field_set_value(GetInstance(), it->second.mMonoFieldType, value_from_instance2);
		}
		else
		{
			if (field.mType == ScriptFieldType::Long)
			{
				long value = otherInstance->GetFieldValue<long>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}

			else if (field.mType == ScriptFieldType::Short)
			{
				short value = otherInstance->GetFieldValue<short>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::UChar)
			{
				byte value = otherInstance->GetFieldValue<byte>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::UInt)
			{
				unsigned int value = otherInstance->GetFieldValue<unsigned int>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::ULong)
			{
				unsigned long value = otherInstance->GetFieldValue<unsigned long>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::UShort)
			{
				unsigned short value = otherInstance->GetFieldValue<unsigned short>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::Vector2)
			{
				glm::vec2 value = otherInstance->GetFieldValue<glm::vec2>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::Vector3)
			{
				glm::vec3 value = otherInstance->GetFieldValue<glm::vec3>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::Vector4)
			{
				glm::vec4 value = otherInstance->GetFieldValue<glm::vec4>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::Char)
			{
				char value = otherInstance->GetFieldValue<char>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::Int)
			{
				int value = otherInstance->GetFieldValue<int>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::Float)
			{
				float value = otherInstance->GetFieldValue<float>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::Double)
			{
				double value = otherInstance->GetFieldValue<double>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::Bool)
			{
				bool value = otherInstance->GetFieldValue<bool>(name);
				mono_field_set_value(GetInstance(), field.mMonoFieldType, &value);
			}
			else if (field.mType == ScriptFieldType::String)
			{
				std::string value = otherInstance->GetFieldValue<std::string>(name);
				MonoString* str = mono_string_new(mono_domain_get(), value.c_str());
				mono_field_set_value(GetInstance(), field.mMonoFieldType, str);
			}
			else
			{
				BOREALIS_CORE_ERROR("Field {0} in class {1} is not a supported type", name, mScriptClass->GetKlassName());
			}
			
		}
	}

	bool ScriptInstance::IsActive()
	{
		return ScriptingSystem::GetEnabled(shared_from_this());
	}

#ifdef _DEB
#define DefineMonoBehaviourMethod(methodName) \
	void ScriptInstance::methodName() \
	{\
		if (mScriptClass->GetMethod( #methodName, 0) == nullptr) \
		{ \
			return; \
		} \
		MonoObject* exception = nullptr; \
		mono_runtime_invoke(mScriptClass->GetMethod(#methodName, 0), GetInstance(), nullptr, &exception); \
		if (exception) \
		{ \
			mono_print_unhandled_exception(exception); \
		} \
	}\

#define DefineMonoBehaviourCollision(methodName) \
	void ScriptInstance::methodName(UUID colliderID) \
	{\
		if (mScriptClass->GetMethod(#methodName, 1) == nullptr) \
		{ \
			return; \
		} \
		ScriptInstance collider(ScriptingSystem::GetScriptClass("Collider")); \
		collider.SetFieldValue("InstanceID", &colliderID); \
		void* params[1]; \
		params[0] = &collider; \
		MonoObject* exception = nullptr; \
		mono_runtime_invoke(mScriptClass->GetMethod(#methodName, 1), GetInstance(), params, &exception); \
		if (exception) \
		{ \
			mono_print_unhandled_exception(exception); \
		} \
	}\


#else
#define DefineMonoBehaviourMethod(methodName) \
	void ScriptInstance::methodName() \
	{\
		if (mScriptClass->GetMethod(#methodName, 0) == nullptr) \
		{ \
			return; \
		} \
		mono_runtime_invoke(mScriptClass->GetMethod(#methodName, 0), GetInstance(), nullptr, nullptr); \
	}\

#define DefineMonoBehaviourCollision(methodName) \
	void ScriptInstance::methodName(UUID colliderID) \
	{\
		if (mScriptClass->GetMethod(#methodName, 1) == nullptr || colliderID == 0) \
		{ \
			return; \
		} \
		MonoObject* collider; \
		InitGameObject(collider, colliderID, "Collider", false); \
		void* params[1]; \
		params[0] = collider; \
		mono_runtime_invoke(mScriptClass->GetMethod(#methodName, 1), GetInstance(), params, nullptr); \
	}\

#endif

	DefineMonoBehaviourMethod(Awake);
	DefineMonoBehaviourMethod(Start);
	DefineMonoBehaviourMethod(Update);
	DefineMonoBehaviourMethod(LateUpdate);
	DefineMonoBehaviourMethod(FixedUpdate);
	DefineMonoBehaviourMethod(OnEnable);
	DefineMonoBehaviourMethod(OnDisable);
	DefineMonoBehaviourMethod(OnDestroy);
	DefineMonoBehaviourMethod(OnApplicationQuit);
	DefineMonoBehaviourMethod(OnApplicationPause);
	DefineMonoBehaviourMethod(OnApplicationFocus);
	DefineMonoBehaviourMethod(OnGUI);
	DefineMonoBehaviourMethod(OnDrawGizmos);
	DefineMonoBehaviourMethod(OnDrawGizmosSelected);
	DefineMonoBehaviourMethod(OnValidate);
	DefineMonoBehaviourMethod(Reset);
	DefineMonoBehaviourMethod(OnBecameVisible);
	DefineMonoBehaviourMethod(OnBecameInvisible);
	DefineMonoBehaviourMethod(OnPreCull);
	DefineMonoBehaviourMethod(OnPreRender);
	DefineMonoBehaviourMethod(OnPostRender);
	DefineMonoBehaviourMethod(OnRenderObject);
	DefineMonoBehaviourMethod(OnWillRenderObject);
	DefineMonoBehaviourMethod(OnRenderImage);
	DefineMonoBehaviourMethod(OnAudioFilterRead);
	DefineMonoBehaviourMethod(OnParticleCollision);
	DefineMonoBehaviourMethod(OnJointBreak);
	DefineMonoBehaviourMethod(OnAnimatorMove);
	DefineMonoBehaviourMethod(OnAnimatorIK);
	DefineMonoBehaviourCollision(OnCollisionEnter);
	DefineMonoBehaviourCollision(OnCollisionExit);
	DefineMonoBehaviourCollision(OnCollisionStay);
	DefineMonoBehaviourCollision(OnTriggerEnter);
	DefineMonoBehaviourCollision(OnTriggerExit);
	DefineMonoBehaviourCollision(OnTriggerStay);
	
}// End of namespace Borealis



