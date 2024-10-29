/******************************************************************************
/*!
\file       RegisterNodes.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares the class for node factory

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef REGISTER_NODES_HPP
#define REGISTER_NODES_HPP
#include <unordered_map>
#include <string>
#include <AI/BehaviourTree/BehaviourNode.hpp>
#include <Scene/Entity.hpp>
namespace Borealis
{

    // Macro definitions to register nodes dynamically
    /*!***********************************************************************
        \brief
            Registers a control flow node prototype with the factory.
        \param[in] name
            The name of the control flow node class.
    *************************************************************************/
#define REGISTER_CONTROLFLOW(Name) \
    NodeFactory::RegisterNodePrototype(#Name, MakeRef<Name>());

    /*!***********************************************************************
        \brief
            Registers a decorator node prototype with the factory.
        \param[in] name
            The name of the decorator node class.
    *************************************************************************/
#define REGISTER_DECORATOR(Name) \
    NodeFactory::RegisterNodePrototype(#Name, MakeRef<Name>());

    /*!***********************************************************************
        \brief
            Registers a leaf node prototype with the factory.
        \param[in] name
            The name of the laef node class.
    *************************************************************************/
#define REGISTER_LEAF(Name) \
    NodeFactory::RegisterNodePrototype(#Name,  MakeRef<Name>());

    class NodeFactory
    {
    public:
        /*!***********************************************************************
            \brief
                Initializes the node factory with the default node prototypes.
        *************************************************************************/
        static void Init();

        /*!***********************************************************************
            \brief
                Registers a node prototype in the internal prototype map.
            \param[in] nodeName
                The name of the node to register.
            \param[in] prototype
				A reference to the node prototype.
        *************************************************************************/
        inline static void RegisterNodePrototype(const std::string& nodeName, Ref<BehaviourNode> prototype) 
        { 
            sNodePrototypes[nodeName] = prototype; 
        }

        /*!***********************************************************************
            \brief
                Creates a node by its registered name.
            \param[in] nodeName
                The name of the node to create.
            \return
                A reference to the created node.
        *************************************************************************/
        static Ref<BehaviourNode> CreateNodeByName(const std::string& nodeName);

        /*!***********************************************************************
            \brief
                Converts a string representation of a node type into a NodeType 
                enum.
            \param[in] typeStr
                The string representing the node type.
            \return
                The corresponding NodeType enum.
        *************************************************************************/
        static NodeType StringToNodeType(const std::string& typeStr);

        static std::unordered_map<std::string, Ref<BehaviourNode>> GetNodePrototypes();

    private:
        /*!***********************************************************************
            \brief
                Recursively adds child nodes to the parent node to build the 
                tree structure.
            \param[in] parent
                The parent node.
            \param[in] child
                The child node to add.
        *************************************************************************/
        static void RecursiveAdd(Ref<BehaviourNode> parent, Ref<BehaviourNode> child);

        /*!***********************************************************************
            \brief
                Finds the correct parent node based on the depth of the node.
            \param[in] depth
                The depth of the node in the tree.
            \return
                A reference to the parent node.
        *************************************************************************/
        static Ref<BehaviourNode> FindParentNode(int depth);

        /*!***********************************************************************
            \brief
                Gets the name of the current behavior tree.
            \return
                The name of the behavior tree.
        *************************************************************************/
        inline static std::string GetTreeName() { return sTreeName; }

        /*!***********************************************************************
            \brief
                Sets the name of the behavior tree.
            \param[in] name
                The name to set for the tree.
        *************************************************************************/
        inline static void SetTreeName(const char* name) { sTreeName = name; }

        // Internal storage for registered node prototypes
        static std::unordered_map<std::string, Ref<BehaviourNode>> sNodePrototypes;

        // Name of the current behavior tree
        static std::string sTreeName;

        // Root node of the tree
        static Ref<BehaviourNode> sRootNode;

        // Tracks the most recently added node
        static Ref<BehaviourNode> sPreviousNode;
    };
}

#endif
