/******************************************************************************
/*!
\file       RenderGraphNodeEditor.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <RenderGraphEditor/RenderGraphNodeEditor.hpp>

#include <imgui_node_editor.h>
#include <imgui_internal.h>

namespace Borealis
{
    void RenderGraphNodeEditor::Render()
    {
        ImGui::Begin("RenderGraph");
        ShowEditor();
        ImGui::End();
    }

    namespace ed = ax::NodeEditor;

	void RenderGraphNodeEditor::ShowEditor()
	{
        // Create a context for the node editor
        static ed::EditorContext* context = ed::CreateEditor();
        ed::SetCurrentEditor(context);

        // Begin the node editor window
        ed::Begin("Node Editor");

        // Start the layout for the first node
        ed::BeginNode(1);
        ImGui::Text("Simple Node");
        ed::BeginPin(1, ed::PinKind::Input);  // Create input pin
        ImGui::Text("Input");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(2, ed::PinKind::Output); // Create output pin
        ImGui::Text("Output");
        ed::EndPin();
        ed::EndNode();

        // Layout another node
        ed::BeginNode(2);
        ImGui::Text("Another Node");
        ed::BeginPin(3, ed::PinKind::Input);
        ImGui::Text("Input");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(4, ed::PinKind::Output);
        ImGui::Text("Output");
        ed::EndPin();
        ed::EndNode();

        // Handle linking (for demo purposes, we manually add one link)
        ed::Link(1, 2, 3); // Link input pin of node 1 to output pin of node 2

        // End the node editor
        ed::End();

        // Destroy the context when done
        //ed::DestroyEditor(context);
	}
}

