/******************************************************************************/
/*!
\file		ScriptingUtils.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	September 07, 2024
\brief		Declares the class for the Scripting Utilities

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <mono/metadata/assembly.h>
#include <mono/metadata/appdomain.h>
#include <Core/LoggerSystem.hpp>
#include <Scripting/ScriptingUtils.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptClass.hpp>

namespace Borealis
{
	char* ReadBytes(const std::string& path, uint32_t* outSize)
	{
		std::ifstream stream(path, std::ios::binary | std::ios::ate);
		BOREALIS_CORE_ASSERT(stream.is_open(), "Failed to open file");

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = static_cast<uint32_t>(end - stream.tellg());

		if (size == 0)
		{
			BOREALIS_CORE_ERROR("File is empty: {0}", path);
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read(buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}

	MonoAssembly* LoadCSharpAssembly(const std::string& path)
	{
		uint32_t size;
		char* buffer = ReadBytes(path, &size);

		BOREALIS_CORE_ASSERT(buffer, "Failed to read file");

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(buffer, size, 1, &status, 0);
		BOREALIS_CORE_ASSERT(!status, mono_image_strerror(status));

		MonoAssembly* assembly = mono_assembly_load_from_full(image, path.c_str(), &status, 0);
		mono_image_close(image);

		delete[] buffer;
		return assembly;
	}

	MonoAssembly* LoadCSharpAssembly(char* buffer, uint32_t size, const char* assemblyName)
	{
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(buffer, size, 1, &status, 0);
		BOREALIS_CORE_ASSERT(!status, mono_image_strerror(status));

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyName, &status, 0);
		mono_image_close(image);

		return assembly;
	}

	void PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* table = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(table);

		for (int32_t i = 0; i < numTypes; ++i)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(table, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			BOREALIS_CORE_TRACE("Type: {0}::{1}", nameSpace, name);
		}
	}

	MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		MonoClass* klass = mono_class_from_name(image, namespaceName, className);

		if (klass == nullptr)
		{
			BOREALIS_CORE_WARN("Failed to get class {0}::{1}", namespaceName, className);
			return nullptr;
		}

		return klass;
	}

	MonoObject* InstantiateClass(MonoAssembly* assembly, MonoDomain* appDomain, const char* namespaceName, const char* className)
	{
		// Get a reference to the class we want to instantiate
		MonoClass* klass = GetClassInAssembly(assembly, namespaceName, className);

		// Allocate an instance of our class
		MonoObject* classInstance = mono_object_new(appDomain, klass);

		if (classInstance == nullptr)
		{
			BOREALIS_CORE_WARN("Failed to get class {0}::{1}", namespaceName, className);
			return nullptr;
		}

		// Call the parameterless (default) constructor
		mono_runtime_object_init(classInstance);
		return classInstance;
	}

	MonoObject* InstantiateClass(MonoClass* klass)
	{
		auto classInstance = mono_object_new(mono_domain_get(), klass);
		mono_runtime_object_init(classInstance);
		return classInstance;
	}
	
	std::vector<uint8_t> mono_array_to_vector(MonoArray* monoArray)
	{
		uintptr_t length = mono_array_length(monoArray);
		std::vector<uint8_t> result(static_cast<uint8_t>(length));
		void* data = mono_array_addr_with_size(monoArray, sizeof(uint8_t), 0);
		memcpy(result.data(), data, length);
		return result;
	}

	std::string ScriptFieldType2String(ScriptFieldType type)
	{
		switch (type)
		{
			case ScriptFieldType::UChar:
				return "unsigned char";
			case ScriptFieldType::Char:
				return "char";
			case ScriptFieldType::UShort:
				return "unsigned short";
			case ScriptFieldType::Short:
				return "short";
			case ScriptFieldType::UInt:
				return "unsigned int";
			case ScriptFieldType::Int:
				return "int";
			case ScriptFieldType::ULong:
				return "unsigned long";
			case ScriptFieldType::Long:
				return "long";
			case ScriptFieldType::Float:
				return "float";
			case ScriptFieldType::Double:
				 return "double";
			case ScriptFieldType::String:
				return "string";
			case ScriptFieldType::Bool:
				return "bool";
			case ScriptFieldType:: Vector2:
				return "Vector2";
			case ScriptFieldType::Vector3:
				return "Vector3";
			case ScriptFieldType::Vector4:
				return "Vector4";
			case ScriptFieldType::None:
				return "None";
		}
		BOREALIS_CORE_ASSERT(false, "Unknown ScriptFieldType");
		return "None";
	}

	ScriptFieldType MonoType2ScriptFieldType(MonoType* type)
	{
		char* name = mono_type_get_name(type);
		std::string strName = name;

		if (strName == "System.Byte")
			return ScriptFieldType::UChar;
		else if (strName == "System.SByte")
			return ScriptFieldType::Char;
		else if (strName == "System.UInt16")
			return ScriptFieldType::UShort;
		else if (strName == "System.Int16")
			return ScriptFieldType::Short;
		else if (strName == "System.UInt32")
			return ScriptFieldType::UInt;
		else if (strName == "System.Int32")
			return ScriptFieldType::Int;
		else if (strName == "System.UInt64")
			return ScriptFieldType::ULong;
		else if (strName == "System.Int64")
			return ScriptFieldType::Long;
		else if (strName == "System.Single")
			return ScriptFieldType::Float;
		else if (strName == "System.Double")
			return ScriptFieldType::Double;
		else if (strName == "System.String")
			return ScriptFieldType::String;
		else if (strName == "System.Boolean")
			return ScriptFieldType::Bool;
		else if (strName == "Borealis.Vector2")
			return ScriptFieldType::Vector2;
		else if (strName == "Borealis.Vector3")
			return ScriptFieldType::Vector3;
		else if (strName == "Borealis.Vector4" || strName == "Borealis.Color")
			return ScriptFieldType::Vector4;
		else
			return ScriptFieldType::None;
	}

	Ref<ScriptClass> GetScriptClassUtils(const std::string& className)
	{
		return ScriptingSystem::GetScriptClass(className);
	}

	static std::unordered_map<std::string, MonoClass*> sScriptAttributes;

	void LoadScriptAttribute(MonoClass* object)
	{
		std::string name = mono_class_get_name(object);
		sScriptAttributes[name] = object;
	}

	MonoClass* GetScriptAttribute(std::string name)
	{
		return sScriptAttributes[name];
	}

	void InitGameObject(MonoObject*& object, UUID id)
	{
		object = mono_object_new(mono_domain_get(), ScriptingSystem::GetScriptClass("GameObject")->GetMonoClass());
		void* args[1];
		MonoMethod* ctor = mono_class_get_method_from_name(ScriptingSystem::GetScriptClass("GameObject")->GetMonoClass(), ".ctor", 1);

		args[0] = &id;
		mono_runtime_invoke(ctor, object, args, NULL);

	}


}