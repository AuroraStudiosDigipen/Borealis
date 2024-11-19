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
#include <Scripting/ScriptInstance.hpp>


namespace Borealis
{
    class Entity;
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
        friend class BehaviourTree;


    public:
        BehaviourNode(MonoObject* objectInstance); // For getparent use only

        BehaviourNode() = default;

        BehaviourNode(std::string klassName);

        std::vector<BehaviourNode> GetChildrenNodes() const;

        BehaviourNode GetParent() const;
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
                Adds a child to this node
            \param[in] child 
                The child node to add
        *************************************************************************/
        void AddChild(BehaviourNode child);

        /*!***********************************************************************
           \brief
               Checks if the node is currently running.
           \return
               True if the node is RUNNING, false otherwise.
        *************************************************************************/
        bool IsRunning() const;

        /*!***********************************************************************
           \brief
               Sets the status of this node.
           \param[in] newStatus
               The status to set.
        *************************************************************************/
        void SetStatus(NodeStatus newStatus);

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
        BehaviourNode Clone();

        std::string GetName() const;
    protected:
        Ref<ScriptInstance> mInstance;
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