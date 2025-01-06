/******************************************************************************/
/*!
\file		ScripClass.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	September 15, 2024
\brief		Declares the class ScriptClass that handles a CS script that 
			is attached to an entity

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "BorealisPCH.hpp"
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/reflection.h>
#include "Scripting/ScriptClass.hpp"
#include <Scripting/ScriptingUtils.hpp>
namespace Borealis
{ 
	ScriptClass::ScriptClass(const std::string& namespaceName, const std::string& className, MonoAssembly* assembly)
	{
		mNamespaceName = namespaceName;
		mClassName = className;
		mMonoClass = GetClassInAssembly(assembly, namespaceName.c_str(), className.c_str());
	}

	bool ScriptClass::IsNativeComponent() const
	{
		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_class(mMonoClass);

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

	bool ScriptClass::IsAssetField() const
	{
		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_class(mMonoClass);

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

	int ScriptClass::GetAssetType() const
	{
		MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_class(mMonoClass);
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


	MonoObject* ScriptClass::Instantiate()
	{
		return InstantiateClass(mMonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(std::string methodName, int paramCount)
	{
		return mono_class_get_method_from_name(mMonoClass, methodName.c_str(), paramCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}

	MonoClass* ScriptClass::GetMonoClass() const
	{
		return mMonoClass;
	}

	std::string ScriptClass::GetKlassName() const
	{
		return mClassName;
	}

}