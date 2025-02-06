/******************************************************************************
/*!
\file       Components.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       November 03, 2024
\brief      Defines the component structs and classes

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Scene/SceneManager.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Components.hpp>
#include <Core/Utils.hpp>

namespace Borealis
{
	glm::mat4 TransformComponent::GetGlobalTransform()
	{
		std::stack<UUID> parentList;
		UUID currID = ParentID;
		while (currID != 0)
		{
			parentList.push(currID); // CBA
			currID = SceneManager::GetActiveScene()->GetEntityByUUID(currID).GetComponent<TransformComponent>().ParentID;
		}
		glm::mat4 globalTransform(1.f);

		while (parentList.size() > 0)
		{
			auto& tc = SceneManager::GetActiveScene()->GetEntityByUUID(parentList.top()).GetComponent<TransformComponent>();
			glm::mat4 localTransform = tc.GetTransform();
			globalTransform = globalTransform * localTransform;
			parentList.pop();
		}
		return globalTransform * GetTransform();
	}

	glm::vec3 TransformComponent::GetGlobalTranslate()
	{
		glm::vec3 globalTranslate(0.f);
		glm::vec3 globalRotation(0.0f);
		glm::vec3 globalScale(1.0f);
		glm::mat4 matrix = GetGlobalTransform();
		Math::MatrixDecomposition(&matrix, &globalTranslate, &globalRotation, &globalScale);
		return globalTranslate;
	}

	glm::vec3 TransformComponent::GetGlobalRotation()
	{
		glm::vec3 globalTranslate(0.f);
		glm::vec3 globalRotation(0.0f);
		glm::vec3 globalScale(1.0f);
		glm::mat4 matrix = GetGlobalTransform();
		Math::MatrixDecomposition(&matrix, &globalTranslate, &globalRotation, &globalScale);
		return globalRotation;
	}

	glm::vec3 TransformComponent::GetGlobalScale()
	{
		glm::vec3 globalTranslate(0.f);
		glm::vec3 globalRotation(0.0f);
		glm::vec3 globalScale(1.0f);
		glm::mat4 matrix = GetGlobalTransform();
		Math::MatrixDecomposition(&matrix, &globalTranslate, &globalRotation, &globalScale);
		return globalScale;
	}

	void TransformComponent::GetGlobalTransformComp(glm::vec3* translate, glm::vec3* rotate, glm::vec3* scale)
	{
		glm::mat4 matrix = GetGlobalTransform();
		Math::MatrixDecomposition(&matrix, translate, rotate, scale);
	}

	void TransformComponent::SetGlobalTransform(glm::mat4 transform)
	{
		if (ParentID != 0)
		{
			Entity parent = SceneManager::GetActiveScene()->GetEntityByUUID(ParentID);
			glm::mat4 parentInverse = glm::inverse(parent.GetComponent<TransformComponent>().GetGlobalTransform());
			glm::mat4 childRelativeTransform = parentInverse * transform;
			Math::MatrixDecomposition(&childRelativeTransform, &Translate, &Rotation, &Scale);
		}
		else
		{
			Math::MatrixDecomposition(&transform , &Translate, &Rotation, &Scale);
		}
	}

	 void TransformComponent::SetParent(Entity entity, Entity parent)
	{
		auto globalMat = GetGlobalTransform();
		ResetParent(entity);
		ParentID = parent.GetUUID();
		SetGlobalTransform(globalMat);
		parent.GetComponent<TransformComponent>().ChildrenID.insert(entity.GetUUID());
	}

	 void TransformComponent::ResetParent(Entity entity)
	{
		auto mat = GetGlobalTransform();
		Math::MatrixDecomposition(&mat, &Translate, &Rotation, &Scale);

		if (ParentID != 0)
		{
			auto parent = SceneManager::GetActiveScene()->GetEntityByUUID(ParentID);
			auto& parentTC = parent.GetComponent<TransformComponent>();
			parentTC.ChildrenID.erase(entity.GetUUID());
		}

		ParentID = 0;
	}

	 void ButtonComponent::onClick()
	 {
		auto entity = SceneManager::GetActiveScene()->GetEntityByUUID(onClickEntity);
		if (!entity.HasComponent<ScriptComponent>())
			 return;

		auto& sc = entity.GetComponent<ScriptComponent>();
		auto iter = sc.mScripts.find(onClickClass);
		if (iter == sc.mScripts.end())
			 return; // script class not found

		auto scriptInstance = iter->second;
		auto scriptClass = scriptInstance->GetScriptClass();
		auto method = scriptClass->GetMethod(onClickFunctionName, 0);
		if (method)
			 scriptClass->InvokeMethod(scriptInstance->GetInstance(), method, nullptr);
	 }

	 void ButtonComponent::onRelease()
	 {
		 if (onReleaseEntity == 0) return;
		 auto entity = SceneManager::GetActiveScene()->GetEntityByUUID(onReleaseEntity);
		 if (!entity.HasComponent<ScriptComponent>()) return;

		 auto& sc = entity.GetComponent<ScriptComponent>();
		 auto iter = sc.mScripts.find(onReleaseClass);
		 if (iter == sc.mScripts.end()) return;

		 auto scriptInstance = iter->second;
		 auto scriptClass = scriptInstance->GetScriptClass();
		 auto method = scriptClass->GetMethod(onReleaseFunctionName, 0);
		 if (method)
			 scriptClass->InvokeMethod(scriptInstance->GetInstance(), method, nullptr);
	 }

	 void ButtonComponent::onHover()
	 {
		 if (onHoverEntity == 0) return;
		 auto entity = SceneManager::GetActiveScene()->GetEntityByUUID(onHoverEntity);
		 if (!entity.HasComponent<ScriptComponent>()) return;

		 auto& sc = entity.GetComponent<ScriptComponent>();
		 auto iter = sc.mScripts.find(onHoverClass);
		 if (iter == sc.mScripts.end()) return;

		 auto scriptInstance = iter->second;
		 auto scriptClass = scriptInstance->GetScriptClass();
		 auto method = scriptClass->GetMethod(onHoverFunctionName, 0);
		 if (method)
			 scriptClass->InvokeMethod(scriptInstance->GetInstance(), method, nullptr);
	 }

}

