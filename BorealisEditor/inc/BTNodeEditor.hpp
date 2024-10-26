/******************************************************************************
/*!
\file       BTNodeEditor.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef BTNodeEditor_HPP
#define BTNodeEditor_HPP
#include <AI/BehaviourTree/RegisterNodes.hpp>
#include <AI/BehaviourTree/BehaviourTree.hpp>
#include <imgui_node_editor.h>
#include <imgui_internal.h>
#include "utilities/builders.h"
#include "utilities/widgets.h"
namespace BorealisEditor
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
    enum class PinKind
    {
        Output,
        Input
    };
    enum class NodeType
    {
        Tree,
        Comment
    };
    class EditorBTNode;
    class Pin
    {
    public:
        ax::NodeEditor::PinId   ID;
        Borealis::Ref<EditorBTNode> Node;
        std::string Name;
        PinType     Type;
        PinKind     Kind;

        Pin(int id, const char* name, PinType type) :
            ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
        {
        }
    };
    class EditorBTNode
    {
    public:
        ax::NodeEditor::NodeId ID;
        std::string Name;
        Borealis::Ref<Pin> parent;
        std::vector<Pin> childNodes;
        ImColor Color;
        NodeType Type;
        ImVec2 Size;
        Borealis::Ref<Borealis::BehaviourTree> mTree;
        Borealis::Ref<Borealis::BehaviourNode> mNode;

        std::string State;
        std::string SavedState;

        EditorBTNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
            ID(id), Name(name), Color(color), Type(NodeType::Tree), Size(0, 0)
        {
            parent = nullptr;
            mTree = nullptr;
            mNode = nullptr;
            //ax::NodeEditor::Utilities
            
        }
    };
}

#endif
