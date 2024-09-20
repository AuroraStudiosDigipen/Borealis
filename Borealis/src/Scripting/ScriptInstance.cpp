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
#include <mono/metadata/object.h>

namespace Borealis
{
	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass)
	{
		mScriptClass = scriptClass;
		mInstance = scriptClass->Instantiate();
		//mScriptClass->InvokeMethod(mInstance, mConstructor, &param);
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
		return mInstance;
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
		mono_field_set_value(mInstance, field.mMonoFieldType, value);
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
		mono_field_get_value(mInstance, field.mMonoFieldType, s_fieldValueBuffer);
		return true;
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
		mono_runtime_invoke(mScriptClass->GetMethod(#methodName, 0), mInstance, nullptr, &exception); \
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
		mono_runtime_invoke(mScriptClass->GetMethod(#methodName, 0), mInstance, nullptr, nullptr); \
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
}// End of namespace Borealis


