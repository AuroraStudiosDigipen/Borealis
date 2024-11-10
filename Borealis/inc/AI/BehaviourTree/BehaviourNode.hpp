/******************************************************************************
/*!
\file       BehaviurNode.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares the class for behaviour node, NodeType, NodeResult and NodeStatus

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef BEHAVIOURNODE_HPP
#define BEHAVIOURNODE_HPP

#include <string>
#include <memory>
#include <vector>
#include <Core/Core.hpp>

#include <Scene/Entity.hpp>

namespace Borealis
{
    class Serialiser; // Forward declaration
    enum class NodeType
    {
        CONTROLFLOW,
        DECORATOR,
        LEAF,
        ROOT,
        UNKNOWN
    };

    enum class NodeStatus
    {
        READY, // node is enterable
        RUNNING, // node is currently running
        EXITING, // node has succeeded or failed
        SUSPENDED // node won't exceute anything
    };

    enum class NodeResult
    {
        IN_PROGRESS, // still being run 
        SUCCESS, // node succeeded
        FAILURE // node failed
    };

    class BehaviourNode : public std::enable_shared_from_this<BehaviourNode>
    {
        friend class NodeFactory;
        friend class Serialiser;

    public:
        BehaviourNode() = default;

        std::vector<Ref<BehaviourNode>> GetChildrenNodes() const;

        /*!***********************************************************************
            \brief
                Gets the type of the node.
            \return
                The NodeType of the node.
        *************************************************************************/
        NodeType GetType() const;

        /*!***********************************************************************
           \brief
               Sets the depth of the node in the behavior tree.
           \param[in] depth
               The depth to set.
       *************************************************************************/
        void SetDepth(unsigned int depth);

        /*!***********************************************************************
            \brief
                Gets the depth of the node in the behavior tree.
            \return
                The depth as an integer.
        *************************************************************************/
        int GetDepth() const;

        /*!***********************************************************************
            \brief
                Gets the name of the node
            \return
                The node as a string
        *************************************************************************/
        std::string GetName() const;

        /*!***********************************************************************
            \brief
                Retrieves the parent node
            \return
                A weak reference to the parent node
        *************************************************************************/
        WeakRef<BehaviourNode> GetParent() const;

        /*!***********************************************************************
            \brief
                Sets the name of the node
            \param[in] setName
                The name to set for the node
        *************************************************************************/
        void SetName(const std::string& setName);

        /*!***********************************************************************
            \brief
                Adds a child to this node
            \param[in] child 
                The child node to add
        *************************************************************************/
        void AddChild(Ref<BehaviourNode> child);

        /*!***********************************************************************
            \brief
                Checks if the node is in the READY state.
            \return
                True if the node is READY, false otherwise.
        *************************************************************************/
        bool IsReady() const;

        /*!***********************************************************************
            \brief
                Checks if the node has succeeded.
            \return
                True if the node's result is SUCCESS, false otherwise.
        *************************************************************************/
        bool HasSucceeded() const;

        /*!***********************************************************************
            \brief
                Checks if the node has failed.
            \return
                True if the node's result is FAILURE, false otherwise.
        *************************************************************************/
        bool HasFailed() const;

        /*!***********************************************************************
           \brief
               Checks if the node is currently running.
           \return
               True if the node is RUNNING, false otherwise.
        *************************************************************************/
        bool IsRunning() const;
        /*!***********************************************************************
           \brief
               Checks if the node is suspended.
           \return
               True if the node is SUSPENDED, false otherwise.
        *************************************************************************/
        bool IsSuspended() const;
        /*!***********************************************************************
           \brief
               Sets the status of this node.
           \param[in] newStatus
               The status to set.
        *************************************************************************/
        void SetStatus(NodeStatus newStatus);

        /*!***********************************************************************
    	    \brief
           	    Recursively sets the status for this node and all of its children.
            \param[in] newStatus
                The status to set for the node and its children.
        *************************************************************************/
        void SetStatusAll(NodeStatus newStatus);

        /*!***********************************************************************
            \brief
                Sets the status for all the node's immediate children.
            \param[in] newStatus
                The status to set for the children.
        *************************************************************************/
        void SetStatusChildren(NodeStatus newStatus);

        /*!***********************************************************************
            \brief
                Sets the result for this node.
            \param[in] result
                The result to set.
        *************************************************************************/
        void SetResult(NodeResult result);

        /*!***********************************************************************
            \brief
                Sets the result for all the node's children.
            \param[in] result
                The result to set for the children.
        *************************************************************************/
        void SetResultChildren(NodeResult result);

        /*!***********************************************************************
            \brief
                Gets the current status of the node.
            \return
                The node's status.
        *************************************************************************/
        NodeStatus GetStatus() const;

        /*!***********************************************************************
            \brief
                Gets the result of the node.
            \return
                The node's result.
        *************************************************************************/
        NodeResult GetResult() const;

        /*!***********************************************************************
            \brief
                Updates the node's state based on its status.
            \param[in] dt
                The delta time for updating the node.
        *************************************************************************/
        void Tick(float dt, Entity& entity);

        /*!***********************************************************************
            \brief
                Clones the node to create a new instance of the same type.
            \return
                A shared reference to the cloned node.
        *************************************************************************/
        virtual Ref<BehaviourNode> Clone() = 0;

    protected:

        /*!***********************************************************************
            \brief
                Called when a leaf node starts execution.
       *************************************************************************/
        void OnLeafEnter();

        /*!***********************************************************************
            \brief
                Called when the node enters execution.
        *************************************************************************/
        virtual void OnEnter();

        /*!***********************************************************************
            \brief
                Called when the node is updated.
            \param[in] dt
                The delta time for updating the node.
        *************************************************************************/
        virtual void OnUpdate(float dt, Entity& entity);

        /*!***********************************************************************
            \brief
                Called when the node exits execution.
        *************************************************************************/
        virtual void OnExit();

        /*!***********************************************************************
            \brief
                Marks the node as successful and exits execution.
        *************************************************************************/
        void OnSuccess();

        /*!***********************************************************************
            \brief
                Marks the node as failed and exits execution.
        *************************************************************************/
        void OnFailure();
    protected:
        WeakRef<BehaviourNode> mParent;  // Parent node reference
        std::vector<Ref<BehaviourNode>> mChildren;  // Child nodes
    private:
        unsigned int mDepth;  // Depth of the node in the behavior tree

        // Node type, status, and result
        NodeType mNodeType;
        NodeStatus mStatus;
        NodeResult mResult;
        
        std::string mName;  // Name of the node
    };

    // Simple CRTP intermediary for cloning derived types
    template <typename T>
    class BaseNode : public BehaviourNode
    {
    public:
        /*!***********************************************************************
            \brief
                Clones the node to create a new instance of the derived type.
            \return
                A shared reference to the cloned node.
        *************************************************************************/
        virtual Ref<BehaviourNode> Clone()
        {
            return std::make_shared<T>(static_cast<const T&>(*this));
        }
    };

}
#endif