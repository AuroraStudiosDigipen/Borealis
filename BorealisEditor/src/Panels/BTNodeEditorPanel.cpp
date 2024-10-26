/******************************************************************************
/*!
\file       BTNodeEditorPanel.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include "Panels/BTNodeEditorPanel.hpp"

namespace Borealis
{
    Ref<Pin> BTNodeEditorPanel::FindPin(ed::PinId id)
    {
        if (!id)
            return nullptr;

        for (auto& node : m_Nodes)
        {
            for (auto& pin : node->Inputs)
                if (pin->ID == id)
                    return pin;

            for (auto& pin : node->Outputs)
                if (pin->ID == id)
                    return pin;
        }

        return nullptr;
    }

    BTNodeEditorPanel::BTNodeEditorPanel()
        : m_ShowPanel(false), m_NextId(1)
    {
        // Initialize the node editor context
        m_EditorContext = ed::CreateEditor();

        // Optionally, initialize default nodes
        // For example, create a root node
        AddNewNode(NodeType::ROOT);
    }

    BTNodeEditorPanel::~BTNodeEditorPanel()
    {
        // Destroy the node editor context
        ed::DestroyEditor(m_EditorContext);
    }

    void BTNodeEditorPanel::ImGuiRender()
    {
        if (!m_ShowPanel)
            return;

        // Begin the ImGui window first
        ImGui::Begin("Behavior Tree Editor", &m_ShowPanel, ImGuiWindowFlags_NoCollapse);

        // Set the current node editor context
        ed::SetCurrentEditor(m_EditorContext);

        // Begin the node editor inside the ImGui window
        ed::Begin("Behavior Tree Editor");

        // Toolbar for adding new nodes
        if (ImGui::Button("Add Condition Node"))
        {
            AddNewNode(NodeType::DECORATOR);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Selector Node"))
        {
            AddNewNode(NodeType::CONTROLFLOW);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Sequence Node"))
        {
            AddNewNode(NodeType::LEAF);
        }

        // Render nodes and links
        RenderNodes();
        RenderLinks();

        // Handle node creation and linking
        HandleNodeCreation();
        HandleNewLinks();

        // End the node editor
        ed::End();
        ed::SetCurrentEditor(nullptr);

        // End the ImGui window
        ImGui::End();
    }

    void BTNodeEditorPanel::AddNewNode(NodeType type)
    {
        int nodeId = GetNextId();
        std::string nodeName;
        ImColor color;

        switch (type)
        {
        case NodeType::DECORATOR:
            nodeName = "Action Node";
            color = ImColor(255, 128, 128);
            break;
        case NodeType::CONTROLFLOW:
            nodeName = "Condition Node";
            color = ImColor(128, 255, 128);
            break;
        case NodeType::LEAF:
            nodeName = "Selector Node";
            color = ImColor(128, 128, 255);
            break;
        default:
            nodeName = "Unknown Node";
            color = ImColor(255, 255, 255);
            break;
        }

        auto node = std::make_shared<Node>(nodeId, nodeName, type, color);

        // Add pins based on node type
        if (type != NodeType::ROOT)
        {
            // Add input pin
            int inputPinId = GetNextId();
            auto inputPin = std::make_shared<Pin>(inputPinId, "In", PinType::Flow, PinKind::Input, node.get());
            node->Inputs.push_back(inputPin);
        }

        // Nodes that can have children will have output pins
        if (type == NodeType::CONTROLFLOW  || type == NodeType::DECORATOR || type == NodeType::ROOT || type == NodeType::LEAF)
        {
            // Add output pin
            int outputPinId = GetNextId();
            auto outputPin = std::make_shared<Pin>(outputPinId, "Out", PinType::Flow, PinKind::Output, node.get());
            node->Outputs.push_back(outputPin);
        }

        m_Nodes.push_back(node);
    }

    void BTNodeEditorPanel::RenderNodes()
    {
        for (auto& node : m_Nodes)
        {
            ed::BeginNode(node->ID);
            ImGui::PushID(node->ID.AsPointer());

            ImGui::Text("%s", node->Name.c_str());
            ImGui::Separator();

            // Input pins
            for (auto& pin : node->Inputs)
            {
                ed::BeginPin(pin->ID, ed::PinKind::Input);
                ImGui::Text("-> %s", pin->Name.c_str());
                ed::EndPin();
            }

            // Output pins
            for (auto& pin : node->Outputs)
            {
                ed::BeginPin(pin->ID, ed::PinKind::Output);
                ImGui::Text("%s ->", pin->Name.c_str());
                ed::EndPin();
            }

            ImGui::PopID();
            ed::EndNode();
        }
    }

    void BTNodeEditorPanel::RenderLinks()
    {
        for (auto& link : m_Links)
        {
            ed::Link(link->ID, link->StartPinID, link->EndPinID);
        }
    }

    void BTNodeEditorPanel::HandleNodeCreation()
    {
        // Implement logic for context menu to create nodes
        // For example, right-click on background to open a menu
        if (ed::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("NodeCreationMenu");
        }

        if (ImGui::BeginPopup("NodeCreationMenu"))
        {
            if (ImGui::MenuItem("Add Control Flow Node"))
            {
                AddNewNode(NodeType::CONTROLFLOW);
            }
            if (ImGui::MenuItem("Add Decorator Node"))
            {
                AddNewNode(NodeType::DECORATOR);
            }
            if (ImGui::MenuItem("Add Leaf Node"))
            {
                AddNewNode(NodeType::LEAF);
            }
            if (ImGui::MenuItem("Add Root Node"))
            {
                AddNewNode(NodeType::ROOT);
            }
            ImGui::EndPopup();
        }
    }

    void BTNodeEditorPanel::HandleNewLinks()
    {
        if (ed::BeginCreate())
        {
            ed::PinId startPinId, endPinId;
            if (ed::QueryNewLink(&startPinId, &endPinId))
            {
                auto startPin = FindPin(startPinId);
                auto endPin = FindPin(endPinId);

                if (startPin && endPin)
                {
                    if (CanCreateLink(startPin, endPin))
                    {
                        if (ed::AcceptNewItem())
                        {
                            // Create a new link
                            m_Links.push_back(std::make_shared<Link>(GetNextId(), startPinId, endPinId));
                        }
                    }
                    else
                    {
                        ed::RejectNewItem();
                    }
                }
                else
                {
                    ed::RejectNewItem();
                }
            }
        }
        ed::EndCreate(); // Ensure this is called outside the if block

        if (ed::BeginDelete())
        {
            ed::LinkId linkId;
            while (ed::QueryDeletedLink(&linkId))
            {
                if (ed::AcceptDeletedItem())
                {
                    // Remove the link
                    m_Links.erase(std::remove_if(m_Links.begin(), m_Links.end(),
                        [linkId](const std::shared_ptr<Link>& link) { return link->ID == linkId; }), m_Links.end());
                }
            }
        }
        ed::EndDelete();
    }


    bool BTNodeEditorPanel::CanCreateLink(Ref<Pin> a, Ref<Pin> b)
    {
        // Prevent linking pins of the same kind
        if (a->Kind == b->Kind)
            return false;

        // Prevent linking pins of different types
        if (a->Type != b->Type)
            return false;

        // Additional logic can be added here
        return true;
    }

    int BTNodeEditorPanel::GetNextId()
    {
        return m_NextId++;
    }
}

