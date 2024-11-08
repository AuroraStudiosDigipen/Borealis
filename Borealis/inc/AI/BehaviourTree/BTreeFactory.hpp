/******************************************************************************
/*!
\file       BTreeFactory.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares

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

    class BTreeFactory
    {
    public:
        struct NodeInfo
        {
            Ref<BehaviourNode> Node;
            YAML::Node Data;
        };
        static BTreeFactory& Instance()
        {
            static BTreeFactory instance;
            return instance;
        }

        void BuildBehaviourTree(const YAML::Node& behaviourTreeNode, Ref<BehaviourTree>& tree);
        void BuildTreeRecursive(Ref<BehaviourNode> currentNode, const std::unordered_map<int, BTreeFactory::NodeInfo>& nodeMap);
        Ref<BehaviourTree> GetBehaviourTree(const std::string& name);
        const std::unordered_map<std::string, Ref<BehaviourTree>>& GetAllBehaviourTrees() const;
        Ref< BehaviourTree>  LoadBehaviourTree(const std::string& filepath);

    private:
        BTreeFactory() = default;

        // Helper function to load a single tree

        // Map from tree name to Behaviour tree instance
        std::unordered_map<std::string, Ref<BehaviourTree>> m_BehaviourTrees;
    };

}

#endif
