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
namespace Borealis
{
    static inline ImRect ImGui_GetItemRect()
    {
        return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }
    static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
    {
        auto result = rect;
        result.Min.x -= x;
        result.Min.y -= y;
        result.Max.x += x;
        result.Max.y += y;
        return result;
    }
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
        std::string Name;
        PinType Type;
        PinKind Kind;
        Node* ParentNode;

        Pin(int id, const std::string& name, PinType type, PinKind kind, Node* parentNode)
            : ID(id), Name(name), Type(type), Kind(kind), ParentNode(parentNode) {}
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
        void AddNewNode(NodeType type);

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
    };
}

#endif
