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
#include <mono/jit/jit.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/reflection.h>
#include <Scripting/ScriptField.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptClass.hpp>
#include <Scripting/ScriptingUtils.hpp>
#include <Audio/AudioEngine.hpp>
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

	std::string ScriptField::GetAudioName(MonoObject* object) const
	{
		auto Method = mono_class_get_method_from_name(GetScriptClassUtils("AudioClip")->GetMonoClass(), "GetAudioName", 0);
		MonoObject* result = mono_runtime_invoke(Method, object, nullptr, nullptr);
		MonoString* monoString = reinterpret_cast<MonoString*>(result);
		if (monoString)
		{
			char* str = mono_string_to_utf8(monoString);
			std::string output = str;
			mono_free((void*)str);
			return output;
		}
		return "";
	}

	void ScriptField::SetAudioName(MonoObject* object, std::string name) const
	{
		auto Method = mono_class_get_method_from_name(GetScriptClassUtils("AudioClip")->GetMonoClass(), "SetAudioName", 1);
		MonoString* monoString = mono_string_new(mono_domain_get(), name.c_str());
		void* args[1] = { monoString };
		mono_runtime_invoke(Method, object, args, nullptr);
	}

	void ScriptField::SetAudioID(MonoObject* object, std::array<uint8_t, 16> id) const
	{
		if (!AudioEngine::DoesEventExist(id)) return;
		MonoArray* monoArray = mono_array_new(mono_domain_get(), mono_get_byte_class(), 16);
		for (int i = 0; i < 16; i++)
		{
			mono_array_set(monoArray, uint8_t, i, id[i]);
		}
		auto Method = mono_class_get_method_from_name(GetScriptClassUtils("AudioClip")->GetMonoClass(), "SetAudioID", 1);
		void* args[1] = { monoArray };
		mono_runtime_invoke(Method, object, args, nullptr);
	}

	std::array<uint8_t, 16> ScriptField::GetAudioID(MonoObject* object) const
	{
		auto Method = mono_class_get_method_from_name(GetScriptClassUtils("AudioClip")->GetMonoClass(), "GetAudioID", 0);
		MonoObject* result = mono_runtime_invoke(Method, object, nullptr, nullptr);
		MonoArray* monoArray = reinterpret_cast<MonoArray*>(result);
		std::array<uint8_t, 16> id;
		for (int i = 0; i < 16; i++)
		{
			id[i] = mono_array_get(monoArray, uint8_t, i);
		}
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
	bool ScriptField::hasHideInInspector(MonoClass* klass) const
	{
		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_field(klass, mMonoFieldType);

		if (attributeInfo)
		{
			auto attributeClass = mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("HideInInspector"));
			if (attributeClass)
			{
				return true;
			}
		}

		return false;
	}
	bool ScriptField::hasSerializeField(MonoClass* klass) const
	{
		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_field(klass, mMonoFieldType);

		if (attributeInfo)
		{
			auto attributeClass = mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("SerializeField"));
			if (attributeClass)
			{
				return true;
			}
		}

		return false;
	}

	bool ScriptField::hasHeader(MonoClass* klass) const
	{
		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_field(klass, mMonoFieldType);

		if (attributeInfo)
		{
			auto attributeClass = mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("Header"));
			if (attributeClass)
			{
				return true;
			}
		}

		return false;
	}

	std::string ScriptField::GetHeader(MonoClass* klass) const
	{
		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_field(klass, mMonoFieldType);

		if (attributeInfo)
		{
			auto attributeClass = mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("Header"));
			if (attributeClass)
			{
				
				auto field = mono_class_get_field_from_name(mono_object_get_class(attributeClass), "header");

				MonoString* monoString;
				mono_field_get_value(attributeClass, field, &monoString);
				if (monoString)
				{
					char* str = mono_string_to_utf8(monoString);
					std::string output = str;
					mono_free((void*)str);
					return output;
				}
			}
		}

		return "";
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
	bool ScriptField::isNativeComponent() const
	{
		auto type = mono_field_get_type(mMonoFieldType);
		auto klass = mono_class_from_mono_type(type);

		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_class(klass);

		if (attributeInfo)
		{
			auto attributeClass = mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("NativeComponent"));
			if (attributeClass)
			{
				return true;
			}
		}

		return false;

	}
	bool ScriptField::isAssetField() const
	{
		auto type = mono_field_get_type(mMonoFieldType);
		auto klass = mono_class_from_mono_type(type);

		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_class(klass);

		if (attributeInfo)
		{
			auto attributeClass = mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("AssetField"));
			if (attributeClass)
			{
				return true;
			}
		}

		return false;
	}
	int ScriptField::GetAssetType() const
	{
		auto type = mono_field_get_type(mMonoFieldType);
		auto klass = mono_class_from_mono_type(type);
		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_class(klass);
		int nodeType = -1;

		if (attributeInfo)
		{
			auto attributeClass = mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("AssetField"));
			if (attributeClass)
			{
				auto field = mono_class_get_field_from_name(mono_object_get_class(attributeClass), "Type");
				mono_field_get_value(attributeClass, field, &nodeType);
			}
		}
		return nodeType;
	}
}

