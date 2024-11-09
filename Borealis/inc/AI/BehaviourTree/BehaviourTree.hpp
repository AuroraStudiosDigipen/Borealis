/******************************************************************************
/*!
\file       BehaviourTree.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares the class for behaviour tree
Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef BEHAVIOURTREE_HPP
#define BEHAVIOURTREE_HPP

#include <string>
#include <Core/Core.hpp>
#include <Assets/Asset.hpp>
#include <AI/BehaviourTree/Blackboard.hpp>

namespace Borealis
{
    // Forward declaration
    class BehaviourNode;
    class Entity;

    class BehaviourTree : public Asset
    {
    public:
        BehaviourTree();
        BehaviourTree(const BehaviourTree& other);

        ~BehaviourTree();

        /*!***********************************************************************
            \brief
                Adds a child node to the given parent node.
            \param[in] parent
                The parent node to which the child will be added.
            \param[in] child
                The child node to add.
            \param[in] depth
                The depth of the child node.
        *************************************************************************/
        void AddNode(Ref<BehaviourNode> parent, Ref<BehaviourNode> child, int depth);

        /*!***********************************************************************
            \brief
                Sets the root node of the behavior tree.
            \param[in] root
                The root node to be set.
        *************************************************************************/
        void SetRootNode(Ref<BehaviourNode> root);

        /*!***********************************************************************
            \brief
                Retrieves the root node of the behavior tree.
            \return
                A reference to the root node.
        *************************************************************************/
        Ref<BehaviourNode> GetRootNode() const;

        /*!***********************************************************************
            \brief
                Updates the behavior tree by calling the update function on the 
                root node.
            \param[in] dt
                Delta time for updating the nodes.
        *************************************************************************/
        void Update(float dt, Entity& entity);

        /*!***********************************************************************
            \brief
                Sets the name of the behavior tree using an rvalue reference.
            \param[in] name
                The name to set (rvalue).
        *************************************************************************/

        void SetBehaviourTreeName(std::string&& name);

        /*!***********************************************************************
            \brief
                Sets the name of the behavior tree using an lvalue reference.
            \param[in] name
                The name to set (lvalue).
        *************************************************************************/
        void SetBehaviourTreeName(std::string& name);

        /*!***********************************************************************
            \brief
                Retrieves the name of the behavior tree.
            \return
                The name of the tree as a string.
        *************************************************************************/
        std::string GetBehaviourTreeName() const;

    private:
        Ref<BehaviourNode> mRootNode;  // The root node of the behavior tree
        Ref<BehaviourNode> mPreviousNode;  // Keeps track of the previously added node
        std::string mTreeName;  // Name of the behavior tree
        Ref<Blackboard> mBlackboard;
    };
}

#endif // BEHAVIOURTREE_HPP