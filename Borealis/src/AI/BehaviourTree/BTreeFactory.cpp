/******************************************************************************
/*!
\file       BTreeFactory.cpp
\author     Joey Chua
\par        email: c.zhengyang@digipen.edu
\date       Nov 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Core/LoggerSystem.hpp>
#include <AI/BehaviourTree/BTreeFactory.hpp>
#include <yaml-cpp/yaml.h>

namespace Borealis
{
    void BuildBehaviourTree(const YAML::Node& behaviourTreeNode, std::shared_ptr<BehaviourTreeData>& treeData) {
        // Extract the tree name
        treeData->TreeName = behaviourTreeNode["Tree Name"].as<std::string>();

        // Parse the nodes
        const YAML::Node& nodes = behaviourTreeNode["Nodes"];
        std::unordered_map<int, std::string> nodeIdToName;
        std::unordered_map<int, std::vector<int>> parentChildMap;

        for (const auto& node : nodes) {
            int id = node["ID"].as<int>();
            std::string name = node["Name"].as<std::string>();
            nodeIdToName[id] = name;

            if (node["Depth"].as<int>() == 0) {
                // The root node will have depth 0
                treeData->RootNodeName = name;
            }

            if (node["ChildrenIDs"]) {
                auto childrenIDs = node["ChildrenIDs"].as<std::vector<int>>();
                parentChildMap[id] = childrenIDs;
            }
        }

        // Build NodeRelationships
        for (const auto& [parentId, childrenIds] : parentChildMap) {
            std::vector<std::string> childNames;
            for (int childId : childrenIds) {
                childNames.push_back(nodeIdToName[childId]);
            }
            treeData->NodeRelationships[nodeIdToName[parentId]] = childNames;
        }
    }


    Ref<Asset> BTreeFactory::LoadBehaviourTree(const std::string& filepath) {
        Ref<BehaviourTreeData> treeData = std::make_shared<BehaviourTreeData>();

        // Load and parse the YAML file
        YAML::Node data;
        try {
            data = YAML::LoadFile(filepath);
        }
        catch (const YAML::Exception& e) {
            BOREALIS_CORE_ERROR("Failed to load behaviour tree from {}: {}", filepath, e.what());
            return treeData;
        }

        // Extract the BehaviourTreeComponent
        const YAML::Node& behaviourTreeComponent = data["BehaviourTreeComponent"];
        if (!behaviourTreeComponent) {
            BOREALIS_CORE_ERROR("Error: 'BehaviourTreeComponent' not found in {}", filepath);
            return treeData;
        }

        // Extract the BehaviourTree node
        const YAML::Node& behaviourTreeNode = behaviourTreeComponent["BehaviourTree"];
        if (!behaviourTreeNode) {
            BOREALIS_CORE_ERROR("Error: 'BehaviourTree' not found in {}", filepath);
            return treeData;
        }

        // Build the behaviour tree data structure
        BuildBehaviourTree(behaviourTreeNode, treeData);

        return treeData;
    }
    
    void BTreeFactory::PrintBehaviourTreeData(const std::shared_ptr<BehaviourTreeData>& treeData) 
    {
        std::cout << "Tree Name: " << treeData->TreeName << "\n";
        std::cout << "Root Node Name: " << treeData->RootNodeName << "\n";
        std::cout << "Node Relationships:\n";
        for (const auto& [parent, children] : treeData->NodeRelationships) {
            std::cout << "  Parent: " << parent << " -> Children: ";
            for (const auto& child : children) {
                std::cout << child << ", ";
            }
            std::cout << "\n";
        }
    }
    // Function to build the behavior tree using NodeRelationships
// Function to build the behavior tree using NodeRelationships
    void BTreeFactory::BuildBehaviourTreeFromData(const std::shared_ptr<BehaviourTreeData>& treeData, BehaviourNode& rootNode)
    {
        std::unordered_map<std::string, BehaviourNode> nodeMap;

        // Create the root node
        rootNode = BehaviourNode(treeData->RootNodeName);
        nodeMap[treeData->RootNodeName] = rootNode;

        // Build the tree
        for (const auto& [parentName, childNames] : treeData->NodeRelationships)
        {
            // Get or create parent node
            BehaviourNode parentNode;
            auto parentIt = nodeMap.find(parentName);
            if (parentIt != nodeMap.end())
            {
                parentNode = parentIt->second;
            }
            else
            {
                parentNode = BehaviourNode(parentName);
                nodeMap[parentName] = parentNode;
            }

            // For each child
            for (const auto& childName : childNames)
            {
                // Get or create child node
                BehaviourNode childNode;
                auto childIt = nodeMap.find(childName);
                if (childIt != nodeMap.end())
                {
                    childNode = childIt->second;
                }
                else
                {
                    childNode = BehaviourNode(childName);
                    nodeMap[childName] = childNode;
                }

                // Add child to parent
                parentNode.AddChild(childNode);
            }
        }

        // Update root node
        rootNode = nodeMap[treeData->RootNodeName];
    }
}

