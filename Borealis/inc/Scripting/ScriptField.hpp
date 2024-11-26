/******************************************************************************
/*!
\file       ScriptField.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 16, 2024
\brief      Declares the Script Field struct which represents a field in a script.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SCRIPT_FIELD_HPP
#define SCRIPT_FIELD_HPP
#include <Core/UUID.hpp>
extern "C" {
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoClass MonoClass;
}

namespace Borealis
{
	enum class ScriptFieldType
	{
		None = 0,
		Bool, UChar, Char, UShort, Short,
		UInt, Int, ULong, Long,

		Float, Double, String,

		Vector2, Vector3, Vector4
	};
	struct ScriptField
	{
		ScriptFieldType mType; // The type of the field
		std::string mName; // The name of the field
		MonoClassField* mMonoFieldType; // The mono field type

		// Properties of Fields
		std::string mFieldClassName() const; // The name of the class of the field
		UUID GetAttachedID(MonoObject* object) const; // The ID of the attached object
		UUID GetGameObjectID(MonoObject* object) const;
		bool isPublic() const;
		bool isPrivate() const;
		bool hasHideInInspector(MonoClass* klass) const;
		bool hasSerializeField(MonoClass* klass) const;
		bool hasHeader(MonoClass* klass) const;
		std::string GetHeader(MonoClass* klass) const;
		bool isMonoBehaviour() const;
		bool isGameObject() const;
		bool isNativeComponent() const;
		bool isAssetField() const;
		int GetAssetType() const;
	};
}

#endif
