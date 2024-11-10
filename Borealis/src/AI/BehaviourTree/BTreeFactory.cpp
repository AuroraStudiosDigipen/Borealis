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
#include <AI/BehaviourTree/RegisterNodes.hpp>
namespace Borealis
{
    void BTreeFactory::BuildBehaviourTree(const YAML::Node& behaviourTreeNode, Ref<BehaviourTree>& tree)
    {
        const YAML::Node& nodesNode = behaviourTreeNode["Nodes"];
        if (!nodesNode || !nodesNode.IsSequence())
        {
           BOREALIS_CORE_ERROR("Error: 'Nodes' not found or not a sequence in the YAML file.");
            return;
        }        
        std::unordered_map<int, NodeInfo> nodeMap;

        Ref<BehaviourNode> rootNode = nullptr;

        // Read all nodes and store them in the map
        for (const auto& nodeData : nodesNode)
        {
            int nodeId = nodeData["ID"].as<int>();
            std::string nodeName = nodeData["Name"].as<std::string>();
            std::string nodeTypeStr = nodeData["Type"].as<std::string>();
            int depth = nodeData["Depth"].as<int>();

            // Create the node using NodeFactory based on its name
            Ref<BehaviourNode> behaviourNode = NodeFactory::CreateNodeByName(nodeName);
            if (!behaviourNode)
            {
                BOREALIS_CORE_ERROR("Error: Unknown node {}", nodeName);
                continue;
            }
            behaviourNode->SetDepth(depth);

            // Check if this node is the root node (Depth == 0)
            if (depth == 0)
            {
                if (rootNode)
                {
                    BOREALIS_CORE_ERROR("Warning: Multiple root nodes found. Using the first one encountered.");
                        
                    return;
                }
                else
                {
                    rootNode = behaviourNode;
                }
            }

            // Store node and its data in the map
            nodeMap[nodeId] = { behaviourNode, nodeData };
        }

        if (!rootNode)
        {
            BOREALIS_CORE_ERROR("Error: No root node found (node with Depth 0)." );
            return;
        }

        // Build the tree recursively starting from the root node
        BuildTreeRecursive(rootNode, nodeMap);

        // Set the root node of the behavior tree
        tree->SetRootNode(rootNode);
        //m_BehaviourTrees.emplace(tree->GetBehaviourTreeName(), tree);
    }
    void BTreeFactory::BuildTreeRecursive(Ref<BehaviourNode> currentNode, const std::unordered_map<int, BTreeFactory::NodeInfo>& nodeMap)
    {
        // Find the node's ID
        int currentNodeId = -1;
        for (const auto& [nodeId, info] : nodeMap)
        {
            if (info.Node == currentNode)
            {
                currentNodeId = nodeId;
                break;
            }
        }
        if (currentNodeId == -1)
        {
            BOREALIS_CORE_ERROR("Error: Current node not found in node map." );
            return;
        }

        // Get the node's YAML data
        const YAML::Node& nodeData = nodeMap.at(currentNodeId).Data;

        // Get the children IDs
        const YAML::Node& childrenIDsNode = nodeData["ChildrenIDs"];
        if (childrenIDsNode && childrenIDsNode.IsSequence())
        {
            for (const auto& childIdNode : childrenIDsNode)
            {
                int childId = childIdNode.as<int>();
                auto it = nodeMap.find(childId);
                if (it != nodeMap.end())
                {
                    Ref<BehaviourNode> childNode = it->second.Node;

                    // Add the child node to the current node
                    currentNode->AddChild(childNode);

                    // Recursively build the child's subtree
                    BuildTreeRecursive(childNode, nodeMap);
                }
                else
                {
                    BOREALIS_CORE_ERROR("Warning: Child node with ID {} not found.", childId);
                }
            }
        }
    }

    Ref< BehaviourTree> BTreeFactory::LoadBehaviourTree(const std::string& filepath)
    {
        // Load and parse the YAML file
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filepath);
        }
        catch (const YAML::Exception& e)
        {
            BOREALIS_CORE_ERROR("Failed to load behaviour tree from {} : {} ", filepath,e.what());
            return nullptr;
        }

        // Extract the BehaviourTreeComponent
        const YAML::Node& behaviourTreeComponent = data["BehaviourTreeComponent"];
        if (!behaviourTreeComponent)
        {
            BOREALIS_CORE_ERROR("Error: 'BehaviourTreeComponent' not found in {}", filepath);
            return nullptr;
        }

        // Extract the BehaviourTree node
        const YAML::Node& behaviourTreeNode = behaviourTreeComponent["BehaviourTree"];
        if (!behaviourTreeNode)
        {
            BOREALIS_CORE_ERROR("Error: 'BehaviourTree' not found in {}", filepath);
            return nullptr;
        }

        // Extract the tree name
        std::string treeName = behaviourTreeNode["Tree Name"].as<std::string>();

        // Create a new BehaviourTree instance
        Ref<BehaviourTree> tree = std::make_shared<BehaviourTree>();
        tree->SetBehaviourTreeName(treeName);
        // Build the behaviour tree structure
        BuildBehaviourTree(behaviourTreeNode, tree);

        return tree;
    }
    Ref<BehaviourNode> BTreeFactory::CloneNodeRecursive(const Ref<BehaviourNode>& originalNode)
    {
        // Create a new node using NodeFactory
        std::string nodeName = originalNode->GetName();
        Ref<BehaviourNode> newNode = NodeFactory::CreateNodeByName(nodeName);

        if (!newNode)
        {
            BOREALIS_CORE_ERROR("Error: Could not clone node  {} .", nodeName);
            return nullptr;
        }

        // Recursively clone and add child nodes
        for (const auto& child : originalNode->GetChildrenNodes())
        {
            Ref<BehaviourNode> clonedChild = CloneNodeRecursive(child);
            if (clonedChild)
            {
                newNode->AddChild(clonedChild);
            }
        }

        return newNode;
    }

    Ref<BehaviourTree> BTreeFactory::CloneBehaviourTree(const Ref<BehaviourTree>& originalTree)
    {
        // Create a new BehaviourTree instance
        Ref<BehaviourTree> newTree = std::make_shared<BehaviourTree>();
        newTree->SetBehaviourTreeName(originalTree->GetBehaviourTreeName());

        // Clone the root node and its subtree
        if (originalTree->GetRootNode())
        {
            Ref<BehaviourNode> clonedRootNode = CloneNodeRecursive(originalTree->GetRootNode());
            newTree->SetRootNode(clonedRootNode);
        }

        return newTree;
    }

}

