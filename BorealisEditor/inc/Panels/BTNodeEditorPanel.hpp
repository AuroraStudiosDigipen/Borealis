/******************************************************************************
/*!
\file       BTNodeEditorPanel.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares

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
        ed::PinId ID;
        Ref<Borealis::Node> Node;
        std::string Name;
        PinType Type;
        PinKind Kind;

        Pin(int id, const std::string& name, PinType type)
            : ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input) {}
        Pin(int id, const std::string& name, PinType type, Ref<Borealis::Node> node, PinKind kind)
            : ID(id), Node(node), Name(name), Type(type), Kind(kind) {}
    };


    // Definition of the Node class
    class Node
    {
    public:
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

        Node(ed::NodeId id, const std::string& name, NodeType type)
            : ID(id), Name(name), Type(type), Color(255, 255, 255), Size(0, 0) {}
        Node(int id, const std::string& name, NodeType type, const ImColor& color)
            : ID(id), Name(name), Type(type), Color(color), Size(0, 0) {}
    };

    // Definition of the Link class
    class Link
    {
    public:
        ed::LinkId ID;
        ed::PinId StartPinID;
        ed::PinId EndPinID;

        Link(int id, ed::PinId startPinId, ed::PinId endPinId)
            : ID(id), StartPinID(startPinId), EndPinID(endPinId) {}
        Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId)
            : ID(id), StartPinID(startPinId), EndPinID(endPinId) {}
    };

    // Definition of the BTNodeEditorPanel class
    class BTNodeEditorPanel
    {
    public:
        BTNodeEditorPanel();
        ~BTNodeEditorPanel();

        // Render function to be called in the main loop
        void ImGuiRender();

        // Functions to show or hide the panel
        void ShowPanel() { m_ShowPanel = true; }
        void HidePanel() { m_ShowPanel = false; }
        Ref<Pin> FindPin(ed::PinId id);
        // Function to handle adding new nodes
        void AddNewNode(const std::string& nodeName);
        void RenderNodeCreationButtons();
        Ref<Node> FindRootNode();
        bool IsPinLinked(ed::PinId pinId);
        bool IsControlFlowNode(Ref<Node> node);
        bool IsLeafNode(Ref<Node> node);
        void SaveBehaviorTree(const std::string& filename);
        void SerializeNode(YAML::Emitter& out, Ref<Node> node, int depth);
        std::vector<Ref<Node>> GetChildNodes(Ref<Node> node);
        void ValidateBehaviorTree();
        std::vector<Ref<Node>> FindRootNodes();
        std::vector<Ref<Node>> FindUnconnectedNodes();
        std::vector<Ref<Node>> GetMaxDepthNodes(const std::vector<Ref<Node>>& rootNodes);
        void GetMaxDepthNodesRecursive(Ref<Node> node, std::vector<Ref<Node>>& maxDepthNodes, std::unordered_set<Ref<Node>>& visited);
        std::vector<std::string> GetAvailableBehaviorTrees();
        void CreateNewNodeType(int nodeTypeIndex, const std::string& nodeName);
        void DeserializeNodes(const YAML::Node& nodesNode);
        void LoadBehaviorTree(const std::string& filepath);
        NodeType FindNodeType(const std::string& nodeName);
        void DeleteSelectedNodes();
        void DeleteSelectedLinks();
        std::string NodeTypeToString(NodeType type);
        Ref<Node> FindNodeByInputPinID(ed::PinId pinID);
        void InitializeNodePins(Ref<Node> node);
        NodeType StringToNodeType(const std::string& typeStr);
        void AssignPositionsRecursive(
            Ref<Node> node,
            int depth,
            float& currentX,
            float nodeSpacing,
            float levelSpacing,
            std::unordered_map<int, Ref<Node>>& nodeMap,
            std::unordered_map<int, std::vector<int>>& nodeChildrenMap);
        Ref<Node> FindNode(ed::NodeId id);

    private:
        // Helper functions
        void RenderNodes();
        void RenderLinks();
        void HandleNodeCreation();
        void HandleNewLinks();
        bool CanCreateLink(Ref<Pin> a, Ref<Pin>b);
        int GetNextId();

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
        std::string m_NodeCreationMessage;
        bool m_OpenTreeNameModal;
    };
}

#endif
