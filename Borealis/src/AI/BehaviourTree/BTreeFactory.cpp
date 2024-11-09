/******************************************************************************
/*!
\file       BTreeFactory.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <AI/BehaviourTree/BTreeFactory.hpp>
#include <AI/BehaviourTree/RegisterNodes.hpp>
namespace Borealis
{
    void BTreeFactory::BuildBehaviourTree(const YAML::Node& behaviourTreeNode, Ref<BehaviourTree>& tree)
    {
        const YAML::Node& nodesNode = behaviourTreeNode["Nodes"];
        if (!nodesNode || !nodesNode.IsSequence())
        {
            std::cerr << "Error: 'Nodes' not found or not a sequence in the YAML file." << std::endl;
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
                std::cerr << "Error: Unknown node '" << nodeName << "'" << std::endl;
                continue;
            }
            behaviourNode->SetDepth(depth);

            // Check if this node is the root node (Depth == 0)
            if (depth == 0)
            {
                if (rootNode)
                {
                    std::cerr << "Warning: Multiple root nodes found. Using the first one encountered." << std::endl;
                        
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
            std::cerr << "Error: No root node found (node with Depth 0)." << std::endl;
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
            std::cerr << "Error: Current node not found in node map." << std::endl;
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
                    std::cerr << "Warning: Child node with ID " << childId << " not found." << std::endl;
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
            std::cerr << "Failed to load behaviour tree from '" << filepath << "': " << e.what() << std::endl;
            return nullptr;
        }

        // Extract the BehaviourTreeComponent
        const YAML::Node& behaviourTreeComponent = data["BehaviourTreeComponent"];
        if (!behaviourTreeComponent)
        {
            std::cerr << "Error: 'BehaviourTreeComponent' not found in '" << filepath << "'" << std::endl;
            return nullptr;
        }

        // Extract the BehaviourTree node
        const YAML::Node& behaviourTreeNode = behaviourTreeComponent["BehaviourTree"];
        if (!behaviourTreeNode)
        {
            std::cerr << "Error: 'BehaviourTree' not found in '" << filepath << "'" << std::endl;
            return nullptr;
        }

        // Extract the tree name
        std::string treeName = behaviourTreeNode["Tree Name"].as<std::string>();

        // Create a new BehaviourTree instance
        Ref<BehaviourTree> tree = std::make_shared<BehaviourTree>();
        std::cout << "Created BehaviourTree at address: " << tree.get() << std::endl;
        tree->SetBehaviourTreeName(treeName);
        // Build the behaviour tree structure
        BuildBehaviourTree(behaviourTreeNode, tree);

        // Add the tree to the map
        //m_BehaviourTrees[treeName] = tree;

        return tree;
    }

    Ref<BehaviourTree> BTreeFactory::GetBehaviourTree(const std::string& name)
    {
        auto it = m_BehaviourTrees.find(name);
        if (it != m_BehaviourTrees.end())
        {
            return it->second;
        }
        else
        {
            std::cerr << "Warning: BehaviourTree '" << name << "' not found." << std::endl;
            return nullptr;
        }
    }

    const std::unordered_map<std::string, Ref<BehaviourTree>>& BTreeFactory::GetAllBehaviourTrees() const
    {
        return m_BehaviourTrees;
    }
    Ref<BehaviourNode> BTreeFactory::CloneNodeRecursive(const Ref<BehaviourNode>& originalNode)
    {
        // Create a new node using NodeFactory
        std::string nodeName = originalNode->GetName();
        Ref<BehaviourNode> newNode = NodeFactory::CreateNodeByName(nodeName);

        if (!newNode)
        {
            std::cerr << "Error: Could not clone node '" << nodeName << "'." << std::endl;
            return nullptr;
        }

        // Copy necessary properties from the original node to the new node
        // Assuming you have methods to get and set properties
        //newNode->SetPropertiesFrom(originalNode);

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
    // BTreeFactory.cpp
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

