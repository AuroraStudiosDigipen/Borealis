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
	glm::mat4 TransformComponent::GetGlobalTransform(Entity entity)
	{
		std::vector<UUID> parentList;
		UUID currID = entity.GetComponent<IDComponent>().ID;
		while (currID != 0)
		{
			parentList.push_back(currID);
			currID = SceneManager::GetActiveScene()->GetEntityByUUID(currID).GetComponent<TransformComponent>().ParentID;
		}
		glm::mat4 globalTransform(1.f);

		// iterate in reverse order
		for (auto it = parentList.begin(); it != parentList.end(); ++it)
		{
			auto& tc = SceneManager::GetActiveScene()->GetEntityByUUID(*it).GetComponent<TransformComponent>();
			glm::mat4 localTransform = tc.GetTransform();
			globalTransform = localTransform * globalTransform;
		}

		return globalTransform;
	}

	glm::vec3 TransformComponent::GetGlobalTranslate(Entity entity)
	{
		glm::vec3 globalTranslate(0.f);
		glm::vec3 globalRotation(0.0f);
		glm::vec3 globalScale(1.0f);
		glm::mat4 matrix = TransformComponent::GetGlobalTransform(entity);
		Math::MatrixDecomposition(&matrix, &globalTranslate, &globalRotation, &globalScale);
		return globalTranslate;
	}

	glm::vec3 TransformComponent::GetGlobalRotation(Entity entity)
	{
		glm::vec3 globalTranslate(0.f);
		glm::vec3 globalRotation(0.0f);
		glm::vec3 globalScale(1.0f);
		glm::mat4 matrix = TransformComponent::GetGlobalTransform(entity);
		Math::MatrixDecomposition(&matrix, &globalTranslate, &globalRotation, &globalScale);
		return globalRotation;
	}

	glm::vec3 TransformComponent::GetGlobalScale(Entity entity)
	{
		glm::vec3 globalTranslate(0.f);
		glm::vec3 globalRotation(0.0f);
		glm::vec3 globalScale(1.0f);
		glm::mat4 matrix = TransformComponent::GetGlobalTransform(entity);
		Math::MatrixDecomposition(&matrix, &globalTranslate, &globalRotation, &globalScale);
		return globalScale;
	}

	void TransformComponent::GetGlobalTransformComp(Entity entity, glm::vec3* translate, glm::vec3* rotate, glm::vec3* scale)
	{
		glm::mat4 matrix = TransformComponent::GetGlobalTransform(entity);
		Math::MatrixDecomposition(&matrix, translate, rotate, scale);
	}

	void TransformComponent::SetGlobalTransform(Entity entity, glm::mat4 transform)
	{
		if (entity.GetComponent<TransformComponent>().ParentID != 0)
		{
			auto& tc = entity.GetComponent<TransformComponent>();
			Entity parent = SceneManager::GetActiveScene()->GetEntityByUUID(entity.GetComponent<TransformComponent>().ParentID);
			glm::mat4 parentInverse = glm::inverse(GetGlobalTransform(parent));
			glm::mat4 childRelativeTransform = parentInverse * transform;
			Math::MatrixDecomposition(&childRelativeTransform, &tc.Translate, &tc.Rotation, &tc.Scale);
		}
		else
		{
			auto& tc = entity.GetComponent<TransformComponent>();
			Math::MatrixDecomposition(&transform , &tc.Translate, &tc.Rotation, &tc.Scale);
		}
	}

	 void TransformComponent::SetParent(Entity entity, Entity parent)
	{
		entity.GetComponent<TransformComponent>().ParentID = parent.GetUUID();
		SetGlobalTransform(entity, GetGlobalTransform(entity));
		parent.GetComponent<TransformComponent>().ChildrenID.insert(entity.GetUUID());
	}

	 void TransformComponent::ResetParent(Entity entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		auto mat = tc.GetGlobalTransform(entity);
		Math::MatrixDecomposition(&mat, &tc.Translate, &tc.Rotation, &tc.Scale);

		if (tc.ParentID != 0)
		{
			auto parent = SceneManager::GetActiveScene()->GetEntityByUUID(tc.ParentID);
			auto& parentTC = parent.GetComponent<TransformComponent>();
			parentTC.ChildrenID.erase(entity.GetUUID());
		}

		tc.ParentID = 0;
	}
}

