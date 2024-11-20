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
		static std::unordered_set<UUID>& getEntitiesAtLayer(int index);
	private:
		static std::unordered_map<int, std::string> mIndexToLayer;
		static std::unordered_map<std::string, int> mLayerToIndex;
		static std::unordered_map<int, std::unordered_set<UUID>> mLayerEntities;
	};
}

#endif
