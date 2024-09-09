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
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>

namespace Borealis
{
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
}

#endif //SCRIPTING_UTILS_HPP