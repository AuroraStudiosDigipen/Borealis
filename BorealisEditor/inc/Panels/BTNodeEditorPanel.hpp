/******************************************************************************
/*!
\file       BTNodeEditorPanel.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares the BTNodeEditorPanel class, which provides a visual
            editor interface for creating, editing, and managing nodes in
            a behavior tree. The panel allows users to add, delete, link,
            and configure nodes using ImGui and ImGuiNodeEditor, as well as
            serialize and deserialize behavior trees to and from YAML files.
            It includes functions to render nodes and links, manage node
            properties, and handle panel visibility and editor events.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef BTNodeEditorPanel_HPP
#define BTNodeEditorPanel_HPP
#include <AI/BehaviourTree/RegisterNodes.hpp>
#include <AI/BehaviourTree/BehaviourTree.hpp>
#include <imgui_node_editor.h>
#include <imgui_internal.h>
#include <yaml-cpp/yaml.h>

namespace Borealis
{
    // Alias for node editor namespace
    namespace ed = ax::NodeEditor;
    // Enumeration for pin types
    enum class PinType
    {
        Flow,
        Bool,
        Int,
        Float,
        String,
        Object,
        Function,
        Delegate,
    };

    // Enumeration for pin kinds (input or output)
    enum class PinKind
    {
        Output,
        Input
    };

    class Pin;
    class Node;
    class Link;

    // Definition of the Pin class
    class Pin
    {
    public:
        /*!***********************************************************************
        \brief  Constructs a Pin with default Input kind.
        \param  id      Unique identifier for the pin.
        \param  name    Name label for the pin.
        \param  type    Data type of the pin.
        *************************************************************************/
        Pin(int id, const std::string& name, PinType type)
            : ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input) {}
        /*!***********************************************************************
        \brief  Constructs a Pin with specified kind (input or output).
        \param  id      Unique identifier for the pin.
        \param  name    Name label for the pin.
        \param  type    Data type of the pin.
        \param  node    Reference to the node this pin is part of.
        \param  kind    Specifies if the pin is an input or output.
        *************************************************************************/
        Pin(int id, const std::string& name, PinType type, Ref<Borealis::Node> node, PinKind kind)
            : ID(id), Node(node), Name(name), Type(type), Kind(kind) {}

        ed::PinId ID;
        Ref<Borealis::Node> Node;
        std::string Name;
        PinType Type;
        PinKind Kind;
    };


    // Definition of the Node class
    class Node
    {
    public:
        /*!***********************************************************************
        \brief  Constructs a Node with a default white color and size.
        \param  id      Unique identifier for the node.
        \param  name    Name of the node.
        \param  type    Type of behavior node.
        *************************************************************************/
        Node(ed::NodeId id, const std::string& name, NodeType type)
            : ID(id), Name(name), Type(type), Color(255, 255, 255), Size(0, 0) {}
        /*!***********************************************************************
        \brief  Constructs a Node with a specified color.
        \param  id      Unique identifier for the node.
        \param  name    Name of the node.
        \param  type    Type of behavior node.
        \param  color   Color of the node in the editor.
        *************************************************************************/
        Node(int id, const std::string& name, NodeType type, const ImColor& color)
            : ID(id), Name(name), Type(type), Color(color), Size(0, 0) {}

        ed::NodeId ID;
        std::string Name;
        NodeType Type;
        std::vector<std::shared_ptr<Pin>> Inputs;
        std::vector<std::shared_ptr<Pin>> Outputs;
        ImColor Color;
        ImVec2 Size;
        ImVec2 Position;
        int Depth = 0;
        bool PositionSet = false; // Add this flag
    };

    // Definition of the Link class
    class Link
    {
    public:
        /*!***********************************************************************
        \brief  Constructs a Link between two pins.
        \param  id          Unique identifier for the link.
        \param  startPinId  ID of the starting pin.
        \param  endPinId    ID of the ending pin.
        *************************************************************************/
        Link(int id, ed::PinId startPinId, ed::PinId endPinId)
            : ID(id), StartPinID(startPinId), EndPinID(endPinId) {}
        /*!***********************************************************************
        \brief  Constructs a Link with an editor-specific ID between two pins.
        \param  id          Editor-specific identifier for the link.
        \param  startPinId  ID of the starting pin.
        \param  endPinId    ID of the ending pin.
        *************************************************************************/
        Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId)
            : ID(id), StartPinID(startPinId), EndPinID(endPinId) {}

        ed::LinkId ID;
        ed::PinId StartPinID;
        ed::PinId EndPinID;
    };

    // Definition of the BTNodeEditorPanel class
    class BTNodeEditorPanel
    {
    public:
        BTNodeEditorPanel();
        ~BTNodeEditorPanel();

        // Rendering Functions
 // ---------------------------------------------------------------------

 /*!***********************************************************************
 \brief  Renders the ImGui-based node editor panel.
 *************************************************************************/
        void ImGuiRender();

        /*!***********************************************************************
        \brief  Renders buttons for creating new nodes in the editor panel.
        *************************************************************************/
        void RenderNodeCreationButtons();

        /*!***********************************************************************
        \brief  Renders all nodes within the node editor.
        *************************************************************************/
        void RenderNodes();

        /*!***********************************************************************
        \brief  Renders all links between nodes within the node editor.
        *************************************************************************/
        void RenderLinks();

        // Panel Visibility Functions
        // ---------------------------------------------------------------------

        /*!***********************************************************************
        \brief  Sets the visibility of the editor panel to true, showing it on screen.
        *************************************************************************/
        void ShowPanel() { m_ShowPanel = true; }

        /*!***********************************************************************
        \brief  Sets the visibility of the editor panel to false, hiding it from view.
        *************************************************************************/
        void HidePanel() { m_ShowPanel = false; }

        // Node and Pin Management Functions
        // ---------------------------------------------------------------------

        /*!***********************************************************************
        \brief  Finds a pin by its unique ID.
        \param  id The unique identifier of the pin.
        \return A reference to the pin if found; otherwise, nullptr.
        *************************************************************************/
        Ref<Pin> FindPin(ed::PinId id);

        /*!***********************************************************************
        \brief  Adds a new node to the editor by specifying its name.
        \param  nodeName The name of the node to be added.
        *************************************************************************/
        void AddNewNode(const std::string& nodeName);

        /*!***********************************************************************
        \brief  Finds the root node within the behavior tree.
        \return A reference to the root node if found; otherwise, nullptr.
        *************************************************************************/
        Ref<Node> FindRootNode();

        /*!***********************************************************************
        \brief  Checks if a specified pin is linked to another pin.
        \param  pinId The unique identifier of the pin to check.
        \return True if the pin is linked; otherwise, false.
        *************************************************************************/
        bool IsPinLinked(ed::PinId pinId);

        /*!***********************************************************************
        \brief  Checks if a specified node is a control flow node.
        \param  node A reference to the node to check.
        \return True if the node is a control flow node; otherwise, false.
        *************************************************************************/
        bool IsControlFlowNode(Ref<Node> node);

        /*!***********************************************************************
        \brief  Checks if a specified node is a leaf node.
        \param  node A reference to the node to check.
        \return True if the node is a leaf node; otherwise, false.
        *************************************************************************/
        bool IsLeafNode(Ref<Node> node);

        /*!***********************************************************************
        \brief  Initializes the input and output pins for a specified node.
        \param  node A reference to the node to initialize pins for.
        *************************************************************************/
        void InitializeNodePins(Ref<Node> node);

        /*!***********************************************************************
        \brief  Finds a node that contains an input pin with the specified ID.
        \param  pinID The unique identifier of the input pin.
        \return A reference to the node if found; otherwise, nullptr.
        *************************************************************************/
        Ref<Node> FindNodeByInputPinID(ed::PinId pinID);

        /*!***********************************************************************
        \brief  Finds a node by its unique ID.
        \param  id The unique identifier of the node.
        \return A reference to the node if found; otherwise, nullptr.
        *************************************************************************/
        Ref<Node> FindNode(ed::NodeId id);

        /*!***********************************************************************
        \brief  Deletes the currently selected nodes within the editor.
        *************************************************************************/
        void DeleteSelectedNodes();

        /*!***********************************************************************
        \brief  Deletes the currently selected links between nodes in the editor.
        *************************************************************************/
        void DeleteSelectedLinks();

        // Behavior Tree Serialization and Validation
        // ---------------------------------------------------------------------

        /*!***********************************************************************
        \brief  Saves the current behavior tree to a specified file.
        \param  filename The file path where the behavior tree will be saved.
        *************************************************************************/
        void SaveBehaviorTree(const std::string& filename);

        /*!***********************************************************************
        \brief  Serializes a specified node to a YAML emitter.
        \param  out     The YAML emitter to write node data to.
        \param  node    A reference to the node to serialize.
        \param  depth   The depth level of the node in the tree.
        *************************************************************************/
        void SerializeNode(YAML::Emitter& out, Ref<Node> node, int depth);

        /*!***********************************************************************
        \brief  Validates the structure and connections of the behavior tree.
        *************************************************************************/
        void ValidateBehaviorTree();

        /*!***********************************************************************
        \brief  Loads a behavior tree from a specified file.
        \param  filepath The file path from which to load the behavior tree.
        *************************************************************************/
        void LoadBehaviorTree(const std::string& filepath);

        /*!***********************************************************************
        \brief  Deserializes nodes from a YAML node structure.
        \param  nodesNode The YAML node containing serialized nodes data.
        *************************************************************************/
        void DeserializeNodes(const YAML::Node& nodesNode);

        // Utility Functions
        // ---------------------------------------------------------------------

        /*!***********************************************************************
        \brief  Retrieves all child nodes of a specified node.
        \param  node A reference to the node whose children are being retrieved.
        \return A vector containing references to the child nodes.
        *************************************************************************/
        std::vector<Ref<Node>> GetChildNodes(Ref<Node> node);

        /*!***********************************************************************
        \brief  Finds all root nodes in the behavior tree.
        \return A vector containing references to the root nodes.
        *************************************************************************/
        std::vector<Ref<Node>> FindRootNodes();

        /*!***********************************************************************
        \brief  Finds all nodes that are not connected to other nodes.
        \return A vector containing references to the unconnected nodes.
        *************************************************************************/
        std::vector<Ref<Node>> FindUnconnectedNodes();

        /*!***********************************************************************
        \brief  Retrieves nodes with the maximum depth from a set of root nodes.
        \param  rootNodes A vector containing references to the root nodes.
        \return A vector containing references to the nodes at maximum depth.
        *************************************************************************/
        std::vector<Ref<Node>> GetMaxDepthNodes(const std::vector<Ref<Node>>& rootNodes);

        /*!***********************************************************************
        \brief  Recursively retrieves nodes with the maximum depth, updating the list.
        \param  node           A reference to the node to start from.
        \param  maxDepthNodes  A vector to store nodes at maximum depth.
        \param  visited        A set of visited nodes to avoid cycles.
        *************************************************************************/
        void GetMaxDepthNodesRecursive(Ref<Node> node, std::vector<Ref<Node>>& maxDepthNodes, std::unordered_set<Ref<Node>>& visited);

        /*!***********************************************************************
        \brief  Retrieves the names of available behavior trees in the system.
        \return A vector containing the names of available behavior trees.
        *************************************************************************/
        std::vector<std::string> GetAvailableBehaviorTrees();

        /*!***********************************************************************
        \brief  Creates a new node type by specifying its index and name.
        \param  nodeTypeIndex The index of the node type.
        \param  nodeName      The name of the new node type.
        *************************************************************************/
        void CreateNewNodeType(int nodeTypeIndex, const std::string& nodeName);

        /*!***********************************************************************
        \brief  Finds the type of a node by its name.
        \param  nodeName The name of the node to search for.
        \return The node type if found; otherwise, a default type.
        *************************************************************************/
        NodeType FindNodeType(const std::string& nodeName);

        /*!***********************************************************************
        \brief  Converts a NodeType to its string representation.
        \param  type The NodeType to convert.
        \return The string representation of the NodeType.
        *************************************************************************/
        std::string NodeTypeToString(NodeType type);

        /*!***********************************************************************
        \brief  Converts a string representation to a NodeType.
        \param  typeStr The string representation of the node type.
        \return The corresponding NodeType.
        *************************************************************************/
        NodeType StringToNodeType(const std::string& typeStr);

        /*!***********************************************************************
        \brief  Generates and retrieves the next unique ID for nodes and links.
        \return The next available unique ID.
        *************************************************************************/
        int GetNextId();

        // Link and Node Interaction Functions
        // ---------------------------------------------------------------------

        /*!***********************************************************************
        \brief  Handles the creation of new nodes in response to user actions.
        *************************************************************************/
        void HandleNodeCreation();

        /*!***********************************************************************
        \brief  Handles the creation of new links between pins in the editor.
        *************************************************************************/
        void HandleNewLinks();

        /*!***********************************************************************
        \brief  Checks if a link can be created between two pins.
        \param  a A reference to the first pin.
        \param  b A reference to the second pin.
        \return True if a link can be created; otherwise, false.
        *************************************************************************/
        bool CanCreateLink(Ref<Pin> a, Ref<Pin> b);


    private:

        // Member variables
        bool m_ShowPanel;
        ed::EditorContext* m_EditorContext;
        std::vector<std::shared_ptr<Node>> m_Nodes;
        std::vector<std::shared_ptr<Link>> m_Links;
        int m_NextId;
        std::vector<std::pair<Ref<Node>, ImVec2>> m_NewNodes;
        std::string m_TreeName;
        bool m_IsEditingExistingTree = false; 
        std::string m_ValidationMessage;
        std::string m_TreeFileName = "Untitled-No-Name-Entered";
        bool m_OpenTreeNameModal;
        bool m_ShouldNavigateToContent;
    };
}

#endif
