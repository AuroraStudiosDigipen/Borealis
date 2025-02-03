/******************************************************************************
/*!
\file       TagList.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include "Core/LayerList.hpp"
#include <Scene/Entity.hpp>
#include <Scene/SceneManager.hpp>

namespace Borealis
{
	std::unordered_map<int, std::string> LayerList::mIndexToLayer = std::unordered_map<int, std::string>();
	std::unordered_map<std::string, int> LayerList::mLayerToIndex = std::unordered_map<std::string, int>();
	std::unordered_map<int, std::unordered_set<UUID>> LayerList::mLayerEntities;
	std::unordered_map<std::string, std::unordered_set<UUID>> TagList::mTagEntities;
	std::unordered_set<std::string> TagList::mKeys;
	void LayerList::Init()
	{
		mLayerToIndex["Default"] = 0;
		mIndexToLayer[0] = "Default";

		mLayerToIndex["TransparentFX"] = 1;
		mIndexToLayer[1] = "TransparentFX";

		mLayerToIndex["Ignore Raycast"] = 2;
		mIndexToLayer[2] = "Ignore Raycast";

		mLayerToIndex["Water"] = 3;
		mIndexToLayer[3] = "Water";

		mLayerToIndex["UI"] = 4;
		mIndexToLayer[4] = "UI";

		mLayerToIndex["PostProcessing"] = 5;
		mIndexToLayer[5] = "PostProcessing";
	}
	void LayerList::SetLayer(int index, std::string layer)
	{
		mLayerToIndex[layer] = index;
		mIndexToLayer[index] = layer;
	}
	bool LayerList::HasLayer(std::string layerName)
	{
		return mLayerToIndex.contains(layerName);
	}
	bool LayerList::HasIndex(int index)
	{
		return mIndexToLayer.contains(index);
	}
	std::string LayerList::IndexToLayer(int index)
	{
		return mIndexToLayer[index];
	}
	int LayerList::LayerToIndex(std::string layerName)
	{
		return mLayerToIndex[layerName];
	}
	void LayerList::RemoveLayer(std::string tag)
	{
		int id = mLayerToIndex[tag];
		mLayerToIndex.erase(tag);
		mIndexToLayer.erase(id);
	}
	void LayerList::RemoveLayer(int index)
	{
		std::string layer = mIndexToLayer[index];
		mIndexToLayer.erase(index);
		mLayerToIndex.erase(layer);
	}
	const std::unordered_map<int, std::string>& LayerList::List()
	{
		return mIndexToLayer;
	}
	void LayerList::Reset()
	{
		mLayerToIndex.clear();
		mIndexToLayer.clear();

		mLayerToIndex["Default"] = 0;
		mIndexToLayer[0] = "Default";

		mLayerToIndex["TransparentFX"] = 1;
		mIndexToLayer[1] = "TransparentFX";

		mLayerToIndex["Ignore Raycast"] = 2;
		mIndexToLayer[2] = "Ignore Raycast";

		mLayerToIndex["Water"] = 3;
		mIndexToLayer[3] = "Water";

		mLayerToIndex["UI"] = 4;
		mIndexToLayer[4] = "UI";

		mLayerToIndex["PostProcessing"] = 5;
		mIndexToLayer[5] = "PostProcessing";
	}
	void LayerList::initializeEntity(Entity entity)
	{
		for (int i = 0; i < 32; i ++)
		{
			if (entity.GetComponent<TagComponent>().mLayer.test(i))
			{
				mLayerEntities[i].insert(entity.GetComponent<IDComponent>().ID);
			}
		}
	}
	void LayerList::resetEntities()
	{
		mLayerEntities.clear();
	}
	bool LayerList::getEntitiesAtLayer(int index, std::unordered_set<UUID>* ptr)
	{
		if (mLayerEntities.find(index) != mLayerEntities.end())
		{
			*ptr = mLayerEntities[index];
			return true;
		}
		else
			return false;
	}


	void TagList::AddEntity(std::string tag, UUID entity)
	{
		if (tag == "")
		{
			return;
		}
		RemoveEntity(entity, tag);

		if (mTagEntities.find(tag) == mTagEntities.end())
		{
			mTagEntities[tag] = std::unordered_set<UUID>();
			mKeys.insert(tag);
		}

		mTagEntities[tag].insert(entity);
	}
	void TagList::AddTag(std::string tag)
	{
		if (tag == "") return;
		if (mTagEntities.find(tag) == mTagEntities.end())
		{
			mTagEntities[tag] = std::unordered_set<UUID>();
			mKeys.insert(tag);
		}
	}

	void TagList::RemoveEntity(UUID entity, std::string inTag)
	{
		std::string tag;
		inTag == "" ? tag = SceneManager::GetActiveScene()->GetEntityByUUID(entity).GetComponent<TagComponent>().Tag : tag = inTag;
		if (mTagEntities.find(tag) != mTagEntities.end())
		{
			if (mTagEntities[tag].find(entity) != mTagEntities[tag].end())
			{
				mTagEntities[tag].erase(entity);
			}
		}
	}

	void TagList::RenameTag(std::string oldTag, std::string newTag)
	{
		if (newTag == "") return;
		if (mTagEntities.find(oldTag) != mTagEntities.end())
		{
			mTagEntities[newTag] = mTagEntities[oldTag];
			mTagEntities.erase(oldTag);
			mKeys.erase(oldTag);
			mKeys.insert(newTag);

			for (auto id : mTagEntities[newTag])
			{
				SceneManager::GetActiveScene()->GetEntityByUUID(id).GetComponent<TagComponent>().Tag = newTag;
			}
		}
	}

	void TagList::Clear()
	{
		for (auto& [tag, entities] : mTagEntities)
		{
			for (auto id : entities)
			{
				SceneManager::GetActiveScene()->GetEntityByUUID(id).GetComponent<TagComponent>().Tag = "";
			}
		}
		mTagEntities.clear();
		mKeys.clear();
	}

	void TagList::Clear(std::string tag)
	{
		for (auto id : mTagEntities[tag])
		{
			SceneManager::GetActiveScene()->GetEntityByUUID(id).GetComponent<TagComponent>().Tag = "";
		}

		mTagEntities.erase(tag);
		mKeys.erase(tag);
	}

	std::unordered_set<UUID> TagList::getEntitiesAtTag(std::string tag)
	{
		if (mTagEntities.find(tag) != mTagEntities.end())
			return mTagEntities[tag];
		else
			return std::unordered_set<UUID>();
	}

	const std::unordered_set<std::string>& TagList::getKeys()
	{
		return mKeys;
	}

}

