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

namespace Borealis
{
    void BTreeFactory::BuildBehaviourTree(const YAML::Node& behaviourTreeNode, Ref<BehaviourTreeData>& treeData)
    {
        const YAML::Node& nodesNode = behaviourTreeNode["Nodes"];
        if (!nodesNode || !nodesNode.IsSequence())
        {
            BOREALIS_CORE_ERROR("Error: 'Nodes' not found or not a sequence in the YAML file.");
            return;
        }

        int rootNodeId = -1;

        for (const auto& nodeData : nodesNode)
        {
            NodeInfo nodeInfo;
            nodeInfo.ID = nodeData["ID"].as<int>();
            nodeInfo.Name = nodeData["Name"].as<std::string>();
            nodeInfo.Depth = nodeData["Depth"].as<int>();

            // Identify the root node (Depth == 0)
            if (nodeInfo.Depth == 0)
            {
                if (rootNodeId != -1)
                {
                    BOREALIS_CORE_WARN("Warning: Multiple root nodes found. Using the first one encountered.");
                }
                else
                {
                    rootNodeId = nodeInfo.ID;
                }
            }

            // Extract child IDs
            const YAML::Node& childrenIDsNode = nodeData["ChildrenIDs"];
            if (childrenIDsNode && childrenIDsNode.IsSequence())
            {
                for (const auto& childIDNode : childrenIDsNode)
                {
                    int childId = childIDNode.as<int>();
                    nodeInfo.ChildrenIds.push_back(childId);
                }
            }

            // Store the node information
            treeData->NodesMap[nodeInfo.ID] = nodeInfo;
        }

        if (rootNodeId == -1)
        {
            BOREALIS_CORE_ERROR("Error: No root node found (node with Depth 0).");
            return;
        }

        treeData->RootNodeId = rootNodeId;
    }

    Ref<Asset> BTreeFactory::LoadBehaviourTree(const std::string& filepath)
    {
        Ref<BehaviourTreeData> treeData = std::make_shared<BehaviourTreeData>();

        // Load and parse the YAML file
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filepath);
        }
        catch (const YAML::Exception& e)
        {
            BOREALIS_CORE_ERROR("Failed to load behaviour tree from {}: {}", filepath, e.what());
            return treeData;
        }

        // Extract the BehaviourTreeComponent
        const YAML::Node& behaviourTreeComponent = data["BehaviourTreeComponent"];
        if (!behaviourTreeComponent)
        {
            BOREALIS_CORE_ERROR("Error: 'BehaviourTreeComponent' not found in {}", filepath);
            return treeData;
        }

        // Extract the BehaviourTree node
        const YAML::Node& behaviourTreeNode = behaviourTreeComponent["BehaviourTree"];
        if (!behaviourTreeNode)
        {
            BOREALIS_CORE_ERROR("Error: 'BehaviourTree' not found in {}", filepath);
            return treeData;
        }

        // Build the behaviour tree data structure
        BuildBehaviourTree(behaviourTreeNode, treeData);

        return treeData;
    }
    void BTreeFactory::BuildTreeRecursive(
        BehaviourNode& currentNode,
        int nodeId,
        const std::unordered_map<int, NodeInfo>& nodesMap)
    {
        // Find the node in the nodesMap
        auto it = nodesMap.find(nodeId);
        if (it == nodesMap.end())
        {
            std::cerr << "Node with ID " << nodeId << " not found.\n";
            return;
        }

        const NodeInfo& nodeInfo = it->second;

        // Create the current node with the node's name
        currentNode = BehaviourNode(nodeInfo.Name);

        // Recursively build and add child nodes
        for (int childId : nodeInfo.ChildrenIds)
        {
            // Find the child node's info
            auto childIt = nodesMap.find(childId);
            if (childIt == nodesMap.end())
            {
                std::cerr << "Child node with ID " << childId << " not found.\n";
                continue;
            }

            const NodeInfo& childNodeInfo = childIt->second;

            // Create a child node with the child's name
            BehaviourNode childNode(childNodeInfo.Name);

            // Recursively build the child node
            BuildTreeRecursive(childNode, childId, nodesMap);

            // Add the child node to the current node
            currentNode.AddChild(childNode);
        }
    }
}

