/******************************************************************************
/*!
\file       LayerList.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef LayerList_HPP
#define LayerList_HPP
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <Core/UUID.hpp>
namespace Borealis
{
	class Entity;
	class LayerList
	{
	public:
		static void Init();
		static void SetLayer(int index, std::string layer);
		static bool HasLayer(std::string layerName);
		static bool HasIndex(int index);
		static std::string IndexToLayer(int index);
		static int LayerToIndex(std::string layerName);
		static void RemoveLayer(std::string tag);
		static void RemoveLayer(int index);
		static const std::unordered_map<int, std::string>& List();
		static void Reset();
		static void initializeEntity(Entity entity);
		static void resetEntities();
		static bool getEntitiesAtLayer(int index, std::unordered_set<UUID>* layer);
	private:
		static std::unordered_map<int, std::string> mIndexToLayer;
		static std::unordered_map<std::string, int> mLayerToIndex;
		static std::unordered_map<int, std::unordered_set<UUID>> mLayerEntities;
	};

	class TagList
	{
	public:
		static void AddEntity(std::string tag, UUID entity);
		static void AddTag(std::string tag);
		static void RemoveEntity(UUID entity, std::string inTag = "");
		static void RenameTag(std::string oldTag, std::string newTag);
		static void Clear();
		static void Clear(std::string tag);
		static std::unordered_set<UUID> getEntitiesAtTag(std::string tag);
		static const std::unordered_set<std::string>& getKeys();
	private:
		static std::unordered_map<std::string, std::unordered_set<UUID>> mTagEntities;
		static std::unordered_set<std::string> mKeys;
	};
}

#endif
