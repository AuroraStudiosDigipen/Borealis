/******************************************************************************
/*!
\file       BehaviourNode.cpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Defines the class for behaviour node

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include "BorealisPCH.hpp"
#include "AI/BehaviourTree/BehaviourNode.hpp"
#include "AI/BehaviourTree/BehaviourTree.hpp"
#include <Core/LoggerSystem.hpp>

namespace Borealis
{

    /*
        @brief Retrieves the node type (e.g., control flow, leaf, etc.).
        @return The NodeType of the node.
    */
    NodeType BehaviourNode::GetType() const
    {
        return mNodeType;
    }

    /*
        @brief Retrieves the depth of the node in the behavior tree.
        @return The depth as an integer.
    */
    int BehaviourNode::GetDepth() const
    {
        return mDepth;
    }

    /*
        @brief Retrieves the parent node.
        @return A weak reference to the parent BehaviourNode.
    */
    WeakRef<BehaviourNode> BehaviourNode::GetParent() const
    {
        return mParent;
    }

    /*
        @brief Sets the depth of the node.
        @param depth The depth to be set.
    */
    void BehaviourNode::SetDepth(unsigned int depth) 
    {
        mDepth = depth;
    }

    /*
        @brief Retrieves the name of the node.
        @return The name of the node as a string.
    */
    std::string BehaviourNode::GetName() const
    {
        return mName;
    }

    /*
        @brief Sets the name of the node.
        @param setName The name to be assigned to the node.
    */
    void BehaviourNode::SetName(const std::string& setName)
    {
        mName = setName;
    }

    /*
        @brief Adds a child node to the current node.
        @param child The BehaviourNode to add as a child.
    */
    void BehaviourNode::AddChild(Ref<BehaviourNode> child)
    {
        mChildren.emplace_back(child);
        child->mParent = shared_from_this();
    }

    /*
        @brief Checks if the node is in the READY state.
        @return True if the node is READY, otherwise false.
    */
    bool BehaviourNode::IsReady() const
    {
        return mStatus == NodeStatus::READY;
    }

    /*
        @brief Checks if the node succeeded.
        @return True if the node's result is SUCCESS, otherwise false.
    */
    bool BehaviourNode::HasSucceeded() const
    {
        return mResult == NodeResult::SUCCESS;
    }

    /*
        @brief Checks if the node failed.
        @return True if the node's result is FAILURE, otherwise false.
    */
    bool BehaviourNode::HasFailed() const
    {
        return mResult == NodeResult::FAILURE;
    }

    /*
        @brief Checks if the node is currently running.
        @return True if the node is RUNNING, otherwise false.
    */
    bool BehaviourNode::IsRunning() const
    {
        return mStatus == NodeStatus::RUNNING;
    }

    /*
        @brief Checks if the node is suspended.
        @return True if the node is SUSPENDED, otherwise false.
    */
    bool BehaviourNode::IsSuspended() const
    {
        return mStatus == NodeStatus::SUSPENDED;
    }

    /*
        @brief Sets the node's status.
        @param newStatus The new status to set.
    */
    void BehaviourNode::SetStatus(NodeStatus newStatus)
    {
        mStatus = newStatus;
    }

    /*
        @brief Recursively sets the status for the node and all of its children.
        @param newStatus The new status to set for the node and its children.
    */
    void BehaviourNode::SetStatusAll(NodeStatus newStatus)
    {
        mStatus = newStatus;
        for (auto&& child : mChildren)
        {
            child->SetStatusAll(newStatus);
        }
    }

    /*
        @brief Sets the status of all the node's children.
        @param newStatus The new status to set for all child nodes.
    */
    void BehaviourNode::SetStatusChildren(NodeStatus newStatus)
    {
        for (auto&& child : mChildren)
        {
            child->SetStatus(newStatus);
        }
    }

    /*
        @brief Sets the result of the node.
        @param r The result to assign to the node.
    */
    void BehaviourNode::SetResult(NodeResult r)
    {
        mResult = r;
    }

    /*
        @brief Sets the result of all the node's children.
        @param result The result to assign to all child nodes.
    */
    void BehaviourNode::SetResultChildren(NodeResult result)
    {
        for (auto&& child : mChildren)
        {
            child->SetResult(result);
        }
    }

    /*
        @brief Retrieves the node's current status.
        @return The node's status.
    */
    NodeStatus BehaviourNode::GetStatus() const
    {
        return mStatus;
    }

    /*
        @brief Retrieves the node's result.
        @return The result of the node.
    */
    NodeResult BehaviourNode::GetResult() const
    {
        return mResult;
    }

    /*
        @brief Updates the node based on its current status.
        @param dt The delta time for updating the node.
    */
    void BehaviourNode::Tick(float dt, Entity& entity)
    {
        if (mStatus == NodeStatus::READY)
        {
            OnEnter();
        }

        if (mStatus == NodeStatus::RUNNING)
        {
            OnUpdate(dt, entity);
        }

        if (mStatus == NodeStatus::EXITING)
        {
            OnExit();
        }
    }

    /*
        @brief Handles the logic when a node enters execution.
    */
    void BehaviourNode::OnEnter()
    {
        SetStatus(NodeStatus::RUNNING);
        SetResult(NodeResult::IN_PROGRESS);
        SetStatusChildren(NodeStatus::READY);
        SetResultChildren(NodeResult::IN_PROGRESS);
    }

    /*
        @brief Handles the logic when a leaf node enters execution.
    */
    void BehaviourNode::OnLeafEnter()
    {
        SetStatus(NodeStatus::RUNNING);
        SetResult(NodeResult::IN_PROGRESS);
    }

    /*
        @brief Handles the node's update logic.
        @param dt The delta time (not used in base logic).
    */
    void BehaviourNode::OnUpdate(float, Entity& entity)
    {
        // No base logic implemented
    }

    /*
        @brief Handles the logic when a node exits execution.
    */
    void BehaviourNode::OnExit()
    {
        SetStatus(NodeStatus::SUSPENDED);
    }

    /*
        @brief Marks the node as successful and exits execution.
    */
    void BehaviourNode::OnSuccess()
    {
        SetStatus(NodeStatus::EXITING);
        SetResult(NodeResult::SUCCESS);
    }

    /*
        @brief Marks the node as failed and exits execution.
    */
    void BehaviourNode::OnFailure()
    {
        SetStatus(NodeStatus::EXITING);
        SetResult(NodeResult::FAILURE);
    }

    std::vector<Ref<BehaviourNode>> BehaviourNode::GetChildrenNodes() const
    {
        return mChildren;
    }
}
