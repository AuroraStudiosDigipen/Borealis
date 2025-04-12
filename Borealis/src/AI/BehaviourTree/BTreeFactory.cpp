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
#include <Assets/AssetManager.hpp>
namespace Borealis
{
    void BuildBehaviourTree(const YAML::Node& behaviourTreeNode, std::shared_ptr<BehaviourTreeData>& treeData) {
        // Extract the tree name
        treeData->TreeName = behaviourTreeNode["Tree Name"].as<std::string>();

        // Parse the nodes
        const YAML::Node& nodes = behaviourTreeNode["Nodes"];

        for (const auto& node : nodes) {
            int id = node["ID"].as<int>();
            std::string name = node["Name"].as<std::string>();

            // Store the node name in the NodeNames map
            treeData->NodeNames[id] = name;

            // Check if this node is the root node (Depth 0)
            if (node["Depth"].as<int>() == 0) {
                treeData->RootNodeID = id;
            }

            // Store the children relationships
            if (node["ChildrenIDs"]) {
                auto childrenIDs = node["ChildrenIDs"].as<std::vector<int>>();
                treeData->NodeRelationships[id] = childrenIDs;
            }
            else {
                // Ensure the node exists in the relationships map, even if it has no children
                treeData->NodeRelationships[id] = {};
            }
        }
    }


    Ref<Asset> BTreeFactory::LoadBehaviourTree(const std::string& filepath) {
        Ref<BehaviourTreeData> treeData = std::make_shared<BehaviourTreeData>();

        // Load and parse the YAML file
        YAML::Node data;
        if (AssetManager::IsPakLoaded())
        {
            char* buffer;
            uint64_t size;
            std::string subPath = filepath.substr(filepath.find_last_of("/\\") + 1);
            AssetManager::RetrieveFromPak(std::stoull(subPath), buffer, size);
            std::string yamlContent(buffer, size);
            data = YAML::Load(yamlContent);
            delete[] buffer;
        }
        else
        {
            try {
                data = YAML::LoadFile(filepath);
            }
            catch (const YAML::Exception& e) {
                BOREALIS_CORE_ERROR("Failed to load behaviour tree from {}: {}", filepath, e.what());
                return treeData;
            }
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

    void BTreeFactory::PrintTree(const Ref<BehaviourTreeData>& treeData, int nodeID, int depth) {
        // Indentation for hierarchy
        std::string indent(depth * 2, ' ');
        std::cout << indent << treeData->NodeNames.at(nodeID) << " (ID: " << nodeID << ")\n";

        // Recursively print children
        if (treeData->NodeRelationships.count(nodeID)) {
            for (int childID : treeData->NodeRelationships.at(nodeID)) {
                PrintTree(treeData, childID, depth + 1);
            }
        }
    }

    // Function to build the behavior tree using NodeRelationships
    void BTreeFactory::BuildBehaviourTreeFromData(const std::shared_ptr<BehaviourTreeData>& treeData, BehaviourNode& rootNode) 
    {
        std::unordered_map<int, BehaviourNode> nodeMap;

        // Create the root node
        rootNode = BehaviourNode(treeData->NodeNames.at(treeData->RootNodeID));
        nodeMap[treeData->RootNodeID] = rootNode;

        // Build the tree
        for (const auto& [parentID, childIDs] : treeData->NodeRelationships) {
            // Get or create the parent node
            BehaviourNode parentNode;
            auto parentIt = nodeMap.find(parentID);
            if (parentIt != nodeMap.end()) {
                parentNode = parentIt->second;
            }
            else {
                parentNode = BehaviourNode(treeData->NodeNames.at(parentID));
                nodeMap[parentID] = parentNode;
            }

            // For each child
            for (int childID : childIDs) {
                // Get or create the child node
                BehaviourNode childNode;
                auto childIt = nodeMap.find(childID);
                if (childIt != nodeMap.end()) {
                    childNode = childIt->second;
                }
                else {
                    childNode = BehaviourNode(treeData->NodeNames.at(childID));
                    nodeMap[childID] = childNode;
                }

                // Add child to parent
                parentNode.AddChild(childNode);
            }
        }

        // Update the root node
        rootNode = nodeMap[treeData->RootNodeID];
    }

    Ref<Asset> BTreeFactory::Load(std::filesystem::path const& cachePath, AssetMetaData const& assetMetaData)
    {
        return Instance().LoadBehaviourTree((cachePath / std::to_string(assetMetaData.Handle)).string());
    }

}

