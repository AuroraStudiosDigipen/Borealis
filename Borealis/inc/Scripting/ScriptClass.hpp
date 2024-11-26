/******************************************************************************/
/*!
\file		ScriptClass.hpp
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


#ifndef SCRIPT_CLASS_HPP
#define SCRIPT_CLASS_HPP
#include <string>
#include <Scripting/ScriptField.hpp>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoString MonoString;
}


namespace Borealis
{

	class ScriptClass
	{
	public:
		friend class ScriptingSystem;

		/*!***********************************************************************
			\brief
				Constructor of the Script Class
			\param[in] namespaceName
				The namespace of the class
			\param[in] className
				The name of the class
			\param[in] assembly
				The assembly that the class is in
		*************************************************************************/
		ScriptClass(const std::string& namespaceName, const std::string& className, MonoAssembly* assembly);

		/*!***********************************************************************
			\brief
				Destructor of the Script Class
		*************************************************************************/
		~ScriptClass() = default;

		/*!***********************************************************************
			\brief
				Instantiate the class
			\return
				The instance of the class
		*************************************************************************/
		MonoObject* Instantiate();

		/*!***********************************************************************
			\brief
				Get the method from the class
			\param[in] methodName
				The name of the method
			\param[in] paramCount
				The number of parameters the method takes
			\return
				The method
		*************************************************************************/
		MonoMethod* GetMethod(std::string methodName, int paramCount);

		/*!***********************************************************************
			\brief
				Invoke the method
			\param[in] instance
				The instance of the class
			\param[in] method
				The method to invoke
			\param[in] params
				The parameters to pass to the method
			\return
				The return value of the method
		*************************************************************************/
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params);

		/*!***********************************************************************
			\brief
				Get the class
			\return
				The class
		*************************************************************************/
		MonoClass* GetMonoClass() const;

		bool IsNativeComponent() const;

		bool IsAssetField() const;

		int GetAssetType() const;

		/*!***********************************************************************
			\brief
				Get the name of the class
			\return
				The name of the class
		*************************************************************************/
		std::string GetKlassName() const;
		std::list<std::string> mOrder; // Order of the fields
		std::unordered_map<std::string, ScriptField> mFields; // Fields of the class

	private:
		void SetMonoClass(MonoClass* klass) { mMonoClass = klass; }
		std::string mClassName; // Name of the class
		std::string mNamespaceName; // Namespace of the class
		MonoClass* mMonoClass; // The class
	};
}
#endif