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
		bool isPublic() const;
		bool isPrivate() const;
		bool hasHideInInspector() const;
		bool hasSerializeField() const;
		bool isMonoBehaviour() const;
	};
}

#endif
