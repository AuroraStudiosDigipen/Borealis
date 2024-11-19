/******************************************************************************
/*!
\file       BehaviourTree.cpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Defines the class for behaviour tree

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include "BorealisPCH.hpp"
#include "AI/BehaviourTree/BehaviourNode.hpp"
#include "AI/BehaviourTree/BehaviourTree.hpp"
#include "Core/LoggerSystem.hpp"
#include <Scene/Entity.hpp>
namespace Borealis
{
    BehaviourTree::BehaviourTree() : mRootNode(nullptr), mTreeName("")
    {}

    BehaviourTree::~BehaviourTree(){}

    void BehaviourTree::Update(float dt, Entity& entity)
    {
        mRootNode.Tick(dt,entity);

        // if the root node is complete in any way, restart it
        if (mRootNode.IsRunning() == false)
        {
            mRootNode.SetStatus(NodeStatus::READY);
        }
    }
    void BehaviourTree::AddNode(BehaviourNode parent, BehaviourNode child, int depth)
    {
        if (depth == 0) {
            // The root node is being added at depth 0
            if (!mRootNode.mInstance->GetInstance()) {
                mRootNode = child;  // If no root node exists, make the child the root
                mRootNode.SetDepth(0);  // Set depth for root node
                mPreviousNode = mRootNode;  // Set previousNode to rootNode after it is added
            }
            else {
                BOREALIS_CORE_ERROR("Error: Root node already exists!");
            }
        }
        else 
        {
            if (!parent.mInstance->GetInstance()) {
                // If no parent is provided, assume we're continuing from the last added node
                parent = mPreviousNode;
            }

            if (!parent.mInstance->GetInstance()) {
                BOREALIS_CORE_ERROR("Error: No valid parent for depth {0}.", depth);
                return;
            }

            if (depth > parent.GetDepth()) {
                // Add child to the parent if the depth is greater than the parent's depth
                child.SetDepth(depth);
                //parent->AddChild(child);
                mPreviousNode = child;  // Update previousNode to the newly added child
            }
            else {
                // Traverse up the tree to find the appropriate parent for the current depth
                BehaviourNode current = parent;
                while (current.GetDepth() >= depth) {
                    // Safely convert weak_ptr to shared_ptr using lock()
                    auto parentPtr = current.GetParent();  // Convert weak_ptr to shared_ptr
                    if (!parentPtr.mInstance->GetInstance()) {
                        BOREALIS_CORE_ERROR("Error: Parent node is no longer valid.");
                        return;
                    }
                    current = parentPtr;  // Update current to the parent
                }

                // Add child to the appropriate parent node
                child.SetDepth(depth);
                //current->AddChild(child);
                mPreviousNode = child;  // Update previousNode to the newly added child
            }
        }
    }
    void BehaviourTree::SetRootNode(BehaviourNode root)
    {
        root.SetDepth(0);
        mRootNode = root;
    } 
    BehaviourNode BehaviourTree::GetRootNode() const 
    {
        return mRootNode;
    }
    void BehaviourTree::SetBehaviourTreeName(std::string&& name)
    {
        mTreeName = name;
    }
    void BehaviourTree::SetBehaviourTreeName(std::string& name)
    {
        mTreeName = name;
    }
    std::string BehaviourTree::GetBehaviourTreeName() const
    {
        return mTreeName;
    }
}