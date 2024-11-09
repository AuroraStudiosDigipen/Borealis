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
#include <yaml-cpp/yaml.h>

namespace Borealis
{
    /*!***********************************************************************
    \class      BTreeFactory
    \brief      Singleton factory class responsible for creating, building, and
                cloning behavior trees from YAML files.
    *************************************************************************/
    class BTreeFactory
    {
    public:
        /*!***********************************************************************
        \struct NodeInfo
        \brief  Structure to hold a behavior node and its corresponding YAML data.
        *************************************************************************/
        struct NodeInfo
        {
            Ref<BehaviourNode> Node; /*!< Reference to the behavior node. */
            YAML::Node Data; /*!< YAML data associated with the node. */
        };

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
        void BuildBehaviourTree(const YAML::Node& behaviourTreeNode, Ref<BehaviourTree>& tree);

        /*!***********************************************************************
        \brief  Recursively builds a behavior tree from a map of node data.
        \param  currentNode   Reference to the current behavior node to build.
        \param  nodeMap       Map of node information, indexed by node ID.
        *************************************************************************/
        void BuildTreeRecursive(Ref<BehaviourNode> currentNode, const std::unordered_map<int, BTreeFactory::NodeInfo>& nodeMap);

        /*!***********************************************************************
        \brief  Loads a behavior tree from a specified YAML file.
        \param  filepath The path to the YAML file containing the behavior tree data.
        \return A reference to the loaded behavior tree. Returns nullptr if loading fails.
        *************************************************************************/
        Ref<BehaviourTree> LoadBehaviourTree(const std::string& filepath);

        /*!***********************************************************************
        \brief  Creates a clone of a specified behavior tree, providing a unique
                instance with the same structure and nodes as the original.
        \param  originalTree The behavior tree to clone.
        \return A reference to the newly cloned behavior tree.
        *************************************************************************/
        Ref<BehaviourTree> CloneBehaviourTree(const Ref<BehaviourTree>& originalTree);

    private:
        /*!***********************************************************************
        \brief  Recursively clones a behavior node and all of its children.
        \param  originalNode The behavior node to clone.
        \return A reference to the newly cloned behavior node.
        *************************************************************************/
        Ref<BehaviourNode> CloneNodeRecursive(const Ref<BehaviourNode>& originalNode);

        /*!***********************************************************************
        \brief  Private constructor to enforce singleton pattern.
        *************************************************************************/
        BTreeFactory() = default;
    };
}

#endif
