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

#include <Assets/AssetMetaData.hpp>

namespace Borealis
{
    struct BehaviourTreeData : public Asset 
    {
        std::string TreeName; // Name of the behavior tree
        int RootNodeID; // ID of the root node
        std::unordered_map<int, std::string> NodeNames; // Map of node ID to node name
        std::unordered_map<int, std::vector<int>> NodeRelationships; // Parent ID to child IDs
    };


    /*!***********************************************************************
    \class      BTreeFactory
    \brief      Singleton factory class responsible for extracting the data
                from the btree file
    *************************************************************************/
    class BTreeFactory 
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
        //void BuildBehaviourTree(const YAML::Node& behaviourTreeNode, Ref<BehaviourTreeData>& treeData);

        /*!***********************************************************************
        \brief  Loads a behavior tree from a specified YAML file.
        \param  filepath The path to the YAML file containing the behavior tree data.
        \return A reference to the loaded behavior tree. Returns nullptr if loading fails.
        *************************************************************************/
        Ref<Asset> LoadBehaviourTree(const std::string& filepath);
        
        void PrintTree(const Ref<BehaviourTreeData>& treeData, int nodeID, int depth=0);
        void BuildBehaviourTreeFromData(const std::shared_ptr<BehaviourTreeData>& treeData, BehaviourNode& rootNode);


        std::unordered_set<std::string> mControlFlowNames;
        std::unordered_set<std::string> mDecoratorNames;
        std::unordered_set<std::string> mLeafNames;
        static Ref<Asset> Load(std::filesystem::path const& cachePath, AssetMetaData const& assetMetaData);

    private:
        /*!***********************************************************************
        \brief  Private constructor to enforce singleton pattern.
        *************************************************************************/
        BTreeFactory() = default;

    };
}

#endif
