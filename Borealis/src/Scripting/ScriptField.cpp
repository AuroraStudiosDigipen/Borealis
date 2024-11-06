/******************************************************************************
/*!
\file       ScriptField.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines the ScriptField structs which represents a field in a script.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/reflection.h>
#include <Scripting/ScriptField.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptClass.hpp>
#include <Scripting/ScriptingUtils.hpp>
namespace Borealis
{
	std::string ScriptField::mFieldClassName() const
	{
		const char* fullname = mono_type_get_name(mono_field_get_type(mMonoFieldType));

		// Find the last occurrence of '.'
		const char* last_dot = strrchr(fullname, '.');
		if (last_dot) {
			return last_dot + 1; // Return the substring after the last dot
		}

		return fullname; // Return the full name if no dot was found
	}
	UUID ScriptField::GetAttachedID(MonoObject* object) const
	{
		auto Method = mono_class_get_method_from_name(GetScriptClassUtils("MonoBehaviour")->GetMonoClass(), "GetInstanceID", 0);
		MonoObject* result = mono_runtime_invoke(Method, object, nullptr, nullptr);
		uint64_t id = *reinterpret_cast<uint64_t*>(mono_object_unbox(result));

		return id;

		
	}
	UUID ScriptField::GetGameObjectID(MonoObject* object) const
	{
		auto Method = mono_class_get_method_from_name(GetScriptClassUtils("Object")->GetMonoClass(), "GetInstanceID", 0);
		MonoObject* result = mono_runtime_invoke(Method, object, nullptr, nullptr);
		uint64_t id = *reinterpret_cast<uint64_t*>(mono_object_unbox(result));

		return id;


	}
	bool ScriptField::isPublic() const
	{
		return mono_field_get_flags(mMonoFieldType) & MONO_FIELD_ATTR_PUBLIC;
	}
	bool ScriptField::isPrivate() const
	{
		return mono_field_get_flags(mMonoFieldType) & MONO_FIELD_ATTR_PRIVATE;
	}
	bool ScriptField::hasHideInInspector() const
	{
		return false;
	}
	bool ScriptField::hasSerializeField() const
	{
		return false;
	}

	bool ScriptField::isMonoBehaviour() const
	{
		auto type = mono_field_get_type(mMonoFieldType);
		auto klass = mono_class_from_mono_type(type);
		return mono_class_is_subclass_of(klass, ScriptingSystem::GetScriptClass("MonoBehaviour")->GetMonoClass(), false);
	}

	bool ScriptField::isGameObject() const
	{
		auto type = mono_field_get_type(mMonoFieldType);
		auto klass = mono_class_from_mono_type(type);
		return mono_class_is_subclass_of(klass, ScriptingSystem::GetScriptClass("GameObject")->GetMonoClass(), false);
	}
}

