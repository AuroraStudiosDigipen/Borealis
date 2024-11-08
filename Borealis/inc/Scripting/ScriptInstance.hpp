/******************************************************************************
/*!
\file       ScriptInstance.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares the Script Instance class which is an instance of a script
			component that is attached to an entity.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SCRIPT_INSTANCE_HPP
#define SCRIPT_INSTANCE_HPP
#include <Core/Core.hpp>
#include <Core/LoggerSystem.hpp>
#include <Scripting/ScriptClass.hpp>
#include <Scene/Entity.hpp>
namespace Borealis
{
	// Specifically meant for MonoBehaviours, not for other classes
	class ScriptInstance : public std::enable_shared_from_this<ScriptInstance>
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor for Script Instances
			\param[in] scriptClass
				Reference to the Script Class being created
		*************************************************************************/
		ScriptInstance(Ref<ScriptClass> scriptClass);
		
		void Init(uint64_t UUIDAddress);

		/*!***********************************************************************
			\brief
				Getter of the Script Class
			\return
				Reference to the Script Class
		*************************************************************************/
		Ref<ScriptClass> GetScriptClass();

		/*!***********************************************************************
			\brief
				Getter of the Underlying MonoClass
			\return
				MonoClass* of the Script Class
		*************************************************************************/
		MonoClass* GetMonoClass() const;

		/*!***********************************************************************
			\brief
				Getter of the MonoObject Instance
			\return
				MonoObject* of the Script Instance
		*************************************************************************/
		MonoObject* GetInstance();

		/*!***********************************************************************
			\brief
				Getter of the Class Name
			\return
				String of the class name
		*************************************************************************/
		std::string GetKlassName() const;

		/*!***********************************************************************
			\brief
				Getter of a field value
			\tparam
				T - Type of the field
			\param[in] name
				Name of the field
			\return
				Value of the field
		*************************************************************************/
		template <typename T>
		T GetFieldValue(const std::string& name)
		{
			if (GetFieldValueInternal(name, s_fieldValueBuffer))
				return *reinterpret_cast<T*>(s_fieldValueBuffer);
			else
			{
				BOREALIS_CORE_WARN("Failed to get field {0} in class {1}", name, mScriptClass->GetKlassName());
				return T();
			}
		}

		/*!***********************************************************************
			\brief
				Getter of a field value for strings
			\param[in] name
				Name of the field
			\return
				Value of the field
		*************************************************************************/
		template <>
		std::string GetFieldValue(const std::string& name)
		{
			std::string output;
			if (GetFieldValueString(name, output))
			{
				return output;
			}
			else
			{
				BOREALIS_CORE_WARN("Failed to get field {0} in class {1}", name, mScriptClass->GetKlassName());
			}
			return std::string();
		}

		/*!***********************************************************************
			\brief
				Setter of a field Value
			\param[in] name
				Name of the field
			\param[in] value
				Address of the value to set
		*************************************************************************/
		void SetFieldValue(const std::string& name, void* value)
		{
			if (!SetFieldValueInternal(name, value))
			{
				BOREALIS_CORE_WARN("Failed to set field {0} in class {1}", name, mScriptClass->GetKlassName());
			}
		}

		/*!***********************************************************************
			\brief
				Setter of a field Value for stirngs
			\param[in] name
				Name of the field
			\param[in] value
				Address of the value to set
		*************************************************************************/
		void SetFieldValue(const std::string& name, std::string* value)
		{
			if (!SetFieldValueInternal(name, *value))
			{
				BOREALIS_CORE_WARN("Failed to set field {0} in class {1}", name, mScriptClass->GetKlassName());
			}
		}

		void ReplaceFieldValue(ScriptInstance* otherInstance, const std::string& name);

		// MonoBehaviour Interface
		void Awake();
		void Start();
		void Update();
		void FixedUpdate();
		void LateUpdate();
		void OnEnable();
		void OnDisable();
		void OnDestroy();
		void OnApplicationQuit();
		void OnApplicationPause();
		void OnApplicationFocus();
		void OnGUI();
		void OnDrawGizmos();
		void OnDrawGizmosSelected();
		void OnValidate();
		void Reset();
		void OnBecameVisible();
		void OnBecameInvisible();
		void OnPreCull();
		void OnPreRender();
		void OnPostRender();
		void OnRenderObject();
		void OnWillRenderObject();
		void OnRenderImage();
		void OnAudioFilterRead();
		void OnParticleCollision();
		void OnJointBreak();
		void OnAnimatorMove();
		void OnAnimatorIK();

		bool IsActive();

	private:

		/*!***********************************************************************
			\brief
				Internal function for SetFieldValue to abstract it
			\param[in] name
				Name of the field
			\param[in] value
				Address of the value to set
			\return
				True if successful, false otherwise
		*************************************************************************/
		bool SetFieldValueInternal(const std::string& name, void* value);

		/*!***********************************************************************
			\brief
				Internal function for SetFieldValue to abstract it for strings
			\param[in] name
				Name of the field
			\param[in] value
				Address of the value to set
			\return
				True if successful, false otherwise
		*************************************************************************/
		bool SetFieldValueInternal(const std::string& name, std::string value);
		/*!***********************************************************************
			\brief
				Internal function for GetFieldValue to abstract it
			\param[in] name
				Name of the field
			\param[out] value
				Address of the value to get
			\return
				True if successful, false otherwise
		*************************************************************************/
		bool GetFieldValueInternal(const std::string& name, const void* value);

		/*!***********************************************************************
			\brief
				Internal function for GetFieldValue to abstract it for strings
			\param[in] name
				Name of the field
			\param[out] value
				Address of the value to get
			\return
				True if successful, false otherwise
		*************************************************************************/
		bool GetFieldValueString(const std::string& name, std::string& output);

		

		Ref<ScriptClass> mScriptClass;				// Script Class
		MonoObject* mInstance;						// MonoObject Instance
		inline static char s_fieldValueBuffer[24];	// Field Value Buffer
	};
}

#endif
