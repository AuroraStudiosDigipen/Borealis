/******************************************************************************/
/*!
\file		Scripting System.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	September 06, 2024
\brief		Declares the class for the Scripting System: Singleton Class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SCRIPTING_SYSTEM_HPP
#define SCRIPTING_SYSTEM_HPP
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <Core/Core.hpp>
#include <Scene/Entity.hpp>
namespace Borealis
{
	class ScriptClass;
	class ScriptingSystem
	{
	public:
		friend class ScriptClass;
		/*!***********************************************************************
			\brief
				Init function for the Scripting System
		*************************************************************************/
		static void Init();

		/*!***********************************************************************
			\brief
				Free function for the Scripting System
		*************************************************************************/
		static void Free();

		/*!***********************************************************************
			\brief
				Update function for the Scripting System
			\param[in] deltaTime
				The time between frames
		*************************************************************************/
		static void Update(float deltaTime);

		/*!***********************************************************************
			\brief
				Register a C# class to the scripting system
			\param[in] klass
				The class to register
		*************************************************************************/
		static void RegisterCSharpClass(ScriptClass klass);

		// Temp
		/*!***********************************************************************
			\brief
				Iniitalise the core assembly
		*************************************************************************/
		static void* InitCoreAssembly();

		/*!***********************************************************************
			\brief
				Get the script class from the library
			\param[in] className
				The name of the class to get
		*************************************************************************/
		static Ref<ScriptClass> GetScriptClass(const std::string& className) { return mScriptClasses[className]; }

		static void CompileCSharpQueue(std::string pathToSave);
		static void CompileCSharpQueueNonThreaded(std::string pathToSave);

		static void PushCSharpQueue(std::string filepath);

		static void LoadScriptAssemblies(std::string filepath);
		static void LoadScriptAssembliesNonThreaded(std::string filepath);


		static void AttachAppDomain();
		static void DetachAppDomain();


		static bool GetEnabled(Ref<ScriptInstance> instance);
		static void SetEnabled(Ref<ScriptInstance>, bool enabled);

		static std::unordered_map<std::string, Ref<ScriptClass>> mScriptClasses; // Library of available scripts
		static std::unordered_map<std::string, std::unordered_set<UUID>> mEntityScriptMap; // Scripts attached to entities

		static void Reload(AssetMetaData const& assetMetaData);

		static void* GetScriptDomain();

		static void ReloadAllEntities();

	private:

		/*!***********************************************************************
			\brief
				Initialises the Mono Runtime
		*************************************************************************/
		static void InitMono();

		/*!***********************************************************************
			\brief
				Frees the Mono Runtime
		*************************************************************************/
		static void FreeMono();

	};
}
#endif

