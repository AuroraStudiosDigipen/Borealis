/******************************************************************************/
/*!
\file		ScriptingUtils.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	September 07, 2024
\brief		Declares the class for the Scripting Utilities

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SCRIPTING_UTILS_HPP
#define SCRIPTING_UTILS_HPP
#include <string>
#include <Scripting/ScriptField.hpp>
#include <Core/Core.hpp>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoType MonoType;
	typedef struct _MonoArray MonoArray;
	typedef struct _MonoDomain MonoDomain;
}


namespace Borealis
{
	class ScriptClass;
	/*!***********************************************************************
		\brief
			Reads the bytes from a file
		\param[in] path
			File path to read from
		\param[out] outSize
			Size of the data read
		\return
			String of bytes read from the file
	*************************************************************************/
	char* ReadBytes(const std::string& path, uint32_t* outSize);

	/*!***********************************************************************
		\brief
			Load a C Sharp Assembly
		\param path
			Path to the assembly
		\return
			MonoAssembly* to the loaded assembly
	*************************************************************************/
	MonoAssembly* LoadCSharpAssembly(const std::string& path);

	MonoAssembly* LoadCSharpAssembly(char* bytes, uint32_t size, const char* assemblyName);

	/*!***********************************************************************
		\brief
			Get a class from an assembly
		\param assembly
			MonoAssembly* to the assembly
		\param namespaceName
			Namespace of the class
		\param className
			Name of the class
		\return
			MonoClass* to the class
	*************************************************************************/
	MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);

	/*!***********************************************************************
		\brief
			Instantiates a class object in C#
		\param[in] assembly
			MonoAssembly* to the assembly
		\param[in] appDomain
			MonoDomain* to the app domain
		\param[in] namespaceName
			Namespace of the class
		\param[in] className
			Name of the class
		\return
			MonoObject* to the instantiated class
	*************************************************************************/
	MonoObject* InstantiateClass(MonoAssembly* assembly, MonoDomain* appDomain, const char* namespaceName, const char* className);

	/*!***********************************************************************
		\brief
			Instantiates a class object in C#
		\param[in] klass
			The class to instantiate
	*************************************************************************/
	MonoObject* InstantiateClass(MonoClass* klass);

	/*!***********************************************************************
		\brief
			Converts a monoArray to a vector of char
		\param[in] monoArray
			MonoArray* to convert
		\return
			Vector of uint8_t
	*************************************************************************/
	std::vector<uint8_t> mono_array_to_vector(MonoArray* monoArray);

	// Debugging Tools
	/*!***********************************************************************
		\brief
			Prints all the namespace::class names in an assembly
		\param assembly
			MonoAssembly* to the assembly
	*************************************************************************/
	void PrintAssemblyTypes(MonoAssembly* assembly);

	/*!***********************************************************************
		\brief
			Converts a ScriptFieldType to a corresponding string
		\return
			String of the ScriptFieldType
	*************************************************************************/
	std::string ScriptFieldType2String(ScriptFieldType type);

	/*!***********************************************************************
		\brief
			Converts a MonoType to a ScriptFieldType
		\return
			ScriptFieldType of the MonoType
	*************************************************************************/
	ScriptFieldType MonoType2ScriptFieldType(MonoType* type);

	/*!***********************************************************************
		\brief
			Gets a script class based on the name via the scripting system
		\param[in] className
			Name of the class
	*************************************************************************/
	Ref<ScriptClass> GetScriptClassUtils(const std::string& className);

	void LoadScriptAttribute(MonoClass* object);

	MonoClass* GetScriptAttribute(std::string name);

	void InitGameObject(MonoObject*& object, UUID id);

}

#endif //SCRIPTING_UTILS_HPP