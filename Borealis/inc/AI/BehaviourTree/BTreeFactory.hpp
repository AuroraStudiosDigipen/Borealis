/******************************************************************************
/*!
\file       BTreeFactory.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       Nov 5, 2024
\brief      Declares the BTreeFactory class, which provides methods for loading,
            building, and cloning behavior trees from YAML configuration files.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef BTreeFactory_HPP
#define BTreeFactory_HPP

#include <BorealisPCH.hpp>
#include <string>
#include <Core/Core.hpp>
#include <AI/BehaviourTree/BehaviourNode.hpp>
#include <AI/BehaviourTree/BehaviourTree.hpp>
#include <Assets/Asset.hpp>
#include <yaml-cpp/yaml.h>

namespace Borealis
{
    struct NodeInfo
    {
        int ID;
        std::string Name;
        int Depth;
        std::vector<int> ChildrenIds;
    };

    struct BehaviourTreeData : public Asset
    {
        int RootNodeId;
        std::unordered_map<int, NodeInfo> NodesMap; // Map from node ID to NodeInfo
        std::vector<int> SortedNodeIds; // Node IDs sorted topologically
    };
    /*!***********************************************************************
    \class      BTreeFactory
    \brief      Singleton factory class responsible for extracting the data
                from the btree file
    *************************************************************************/
    class BTreeFactory : public Asset
    {
    public:

        /*!***********************************************************************
        \brief  Provides a singleton instance of the BTreeFactory.
        \return A reference to the BTreeFactory singleton instance.
        *************************************************************************/
        static BTreeFactory& Instance()
        {
            static BTreeFactory instance;
            return instance;
        }

        /*!***********************************************************************
        \brief  Builds a behavior tree from a YAML node, constructing the tree
                structure based on the configuration provided.
        \param  behaviourTreeNode The root YAML node containing the tree structure.
        \param  tree              Reference to the behavior tree to populate.
        *************************************************************************/
        void BuildBehaviourTree(const YAML::Node& behaviourTreeNode, Ref<BehaviourTreeData>& treeData);

        /*!***********************************************************************
        \brief  Loads a behavior tree from a specified YAML file.
        \param  filepath The path to the YAML file containing the behavior tree data.
        \return A reference to the loaded behavior tree. Returns nullptr if loading fails.
        *************************************************************************/
        Ref<Asset> LoadBehaviourTree(const std::string& filepath);

        void BuildTreeRecursive(
            BehaviourNode& currentNode,
            int nodeId,
            const std::unordered_map<int, NodeInfo>& nodesMap);
        std::unordered_set<std::string> mControlFlowNames;
        std::unordered_set<std::string> mDecoratorNames;
        std::unordered_set<std::string> mLeafNames;

        /*!***********************************************************************
        \brief  Private constructor to enforce singleton pattern.
        *************************************************************************/
        BTreeFactory() = default;

    };
}

#endif
