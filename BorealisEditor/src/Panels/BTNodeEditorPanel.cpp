/******************************************************************************
/*!
\file       BTNodeEditorPanel.cpp
\author     Joey Chua Jun Yu
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
#include <Core/LoggerSystem.hpp>

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

        ImGui::Begin("Behavior Tree Editor", &m_ShowPanel, ImGuiWindowFlags_NoCollapse);

        // Add the Validate Tree button
        if (ImGui::Button("Validate Tree"))
        {
            ValidateBehaviorTree();
        }

        if (ImGui::Button("Save"))
        {
            ImGui::OpenPopup("SaveBehaviorTreePopup");
        }

        if (ImGui::BeginPopup("SaveBehaviorTreePopup"))
        {
            static char fileName[128] = "NewBehaviorTree";
            ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));

            if (ImGui::Button("Save"))
            {
                std::string savePath = "assets/BehaviourTrees/";
                std::string fullPath = savePath + fileName + ".yaml";
                SaveBehaviorTree(fullPath);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }


        // Split the window into left and right panes
        static float leftPaneWidth = 200.0f;
        ImGui::BeginChild("LeftPanel", ImVec2(leftPaneWidth, 0), true);

        // Render node creation buttons in the left panel
        RenderNodeCreationButtons();

        ImGui::EndChild();

        ImGui::SameLine();

        // Begin the node editor pane
        ImGui::BeginChild("NodeEditorPane", ImVec2(0, 0), false);

        ed::SetCurrentEditor(m_EditorContext);
        ed::Begin("Behavior Tree Editor");

        // Handle node creation via context menu
        HandleNodeCreation();

        // **Render nodes and links first**
        RenderNodes();
        RenderLinks();

        // Handle new links and deletions
        HandleNewLinks();

        ed::End();
        ed::SetCurrentEditor(nullptr);

        ImGui::EndChild();

        if (ImGui::BeginPopupModal("ValidationResult", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", m_ValidationMessage.c_str());

            if (ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::End();

    }
    
    void BTNodeEditorPanel::AddNewNode(const std::string& nodeName)
    {
        // Retrieve the prototype from the factory
        auto prototype = NodeFactory::CreateNodeByName(nodeName);
        if (!prototype)
            return;

        // Get the node type from the prototype
        NodeType nodeType = prototype->GetType();

        // Set the node color based on type (you can customize colors per type)
        ImColor nodeColor(255, 255, 255); // Default color

        // Create a new node
        int nodeId = GetNextId();
        auto newNode = std::make_shared<Node>(nodeId, nodeName, nodeType, nodeColor);

        // Set the node position
        newNode->Size = ImVec2(0, 0);

        // Initialize pins based on the node category
        std::string prefix = nodeName.substr(0, 2);
        if (prefix == "C_") // Control Flow Node
        {
            // Input pin
            {
                int pinId = GetNextId();
                auto inputPin = std::make_shared<Pin>(pinId, "In", PinType::Flow);
                inputPin->Node = newNode;
                inputPin->Kind = PinKind::Input;
                newNode->Inputs.push_back(inputPin);
            }

            // Output pin
            {
                int pinId = GetNextId();
                auto outputPin = std::make_shared<Pin>(pinId, "Out", PinType::Flow);
                outputPin->Node = newNode;
                outputPin->Kind = PinKind::Output;
                newNode->Outputs.push_back(outputPin);
            }
        }
        else if (prefix == "D_") // Decorator Node
        {
            // Input pin
            {
                int pinId = GetNextId();
                auto inputPin = std::make_shared<Pin>(pinId, "In", PinType::Flow);
                inputPin->Node = newNode;
                inputPin->Kind = PinKind::Input;
                newNode->Inputs.push_back(inputPin);
            }

            // Output pin
            {
                int pinId = GetNextId();
                auto outputPin = std::make_shared<Pin>(pinId, "Out", PinType::Flow);
                outputPin->Node = newNode;
                outputPin->Kind = PinKind::Output;
                newNode->Outputs.push_back(outputPin);
            }
        }
        else if (prefix == "L_") // Leaf Node
        {
            // Input pin only
            {
                int pinId = GetNextId();
                auto inputPin = std::make_shared<Pin>(pinId, "In", PinType::Flow);
                inputPin->Node = newNode;
                inputPin->Kind = PinKind::Input;
                newNode->Inputs.push_back(inputPin);
            }
            // Leaf nodes do not have output pins
        }
            
        // Add the new node to the list of nodes
        m_Nodes.push_back(newNode);

    }

    int BTNodeEditorPanel::GetNextId()
    {
        return m_NextId++;
    }
    void BTNodeEditorPanel::RenderNodes()
    {
        for (const auto& node : m_Nodes)
        {
            ed::BeginNode(node->ID);
            ImGui::PushID(node->ID.AsPointer());

            ImGui::Text("%s", node->Name.c_str());
            ImGui::Separator();

            // Input pins
            for (const auto& pin : node->Inputs)
            {
                ed::BeginPin(pin->ID, ed::PinKind::Input);
                ImGui::Text("-> %s", pin->Name.c_str());
                ed::EndPin();
            }

            // Output pins
            for (const auto& pin : node->Outputs)
            {
                ed::BeginPin(pin->ID, ed::PinKind::Output);
                ImGui::Text("%s ->", pin->Name.c_str());
                ed::EndPin();
            }

            ImGui::PopID();
            ed::EndNode();
            ImVec2 nodeSize = ed::GetNodeSize(node->ID);

            // If the node size is zero, the node might not have been positioned yet
            if (nodeSize.x == 0.0f && nodeSize.y == 0.0f)
            {
                // Convert screen position to canvas position
                ImVec2 canvasPos = ed::ScreenToCanvas(node->Position);
                ed::SetNodePosition(node->ID, canvasPos);
            }
        }
    }

    void BTNodeEditorPanel::RenderLinks()
    {
        ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f); // Straight lines
        for (auto& link : m_Links)
        {
            ed::Link(link->ID, link->StartPinID, link->EndPinID);
        }
    }

    void BTNodeEditorPanel::HandleNodeCreation()
    {
        // Suspend the node editor to interact with ImGui popups
        ed::Suspend();

        if (ed::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("NodeCreationMenu");
        }

        if (ImGui::BeginPopup("NodeCreationMenu"))
        {
            if (ImGui::BeginMenu("Control Flow Nodes"))
            {
                for (const auto& pair : NodeFactory::GetNodePrototypes())
                {
                    const std::string& nodeName = pair.first;
                    if (nodeName.substr(0, 2) == "C_")
                    {
                        std::string displayName = nodeName.substr(2); // Remove "C_" prefix
                        if (ImGui::MenuItem(displayName.c_str()))
                        {
                            AddNewNode(nodeName);
                        }
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Decorator Nodes"))
            {
                for (const auto& pair : NodeFactory::GetNodePrototypes())
                {
                    const std::string& nodeName = pair.first;
                    if (nodeName.substr(0, 2) == "D_")
                    {
                        std::string displayName = nodeName.substr(2); // Remove "D_" prefix
                        if (ImGui::MenuItem(displayName.c_str()))
                        {
                            AddNewNode(nodeName);
                        }
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Leaf Nodes"))
            {
                for (const auto& pair : NodeFactory::GetNodePrototypes())
                {
                    const std::string& nodeName = pair.first;
                    if (nodeName.substr(0, 2) == "L_")
                    {
                        std::string displayName = nodeName.substr(2); // Remove "L_" prefix
                        if (ImGui::MenuItem(displayName.c_str()))
                        {
                            AddNewNode(nodeName);
                        }
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }

        // Resume the node editor
        ed::Resume();
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
    void BTNodeEditorPanel::RenderNodeCreationButtons()
    {
        // Control Flow Nodes
        if (ImGui::CollapsingHeader("Control Flow Nodes"))
        {
            // List all control flow nodes
            for (const auto& pair : NodeFactory::GetNodePrototypes())
            {
                const std::string& nodeName = pair.first;
                // Check if nodeName starts with "C_"
                if (nodeName.rfind("C_", 0) == 0) // rfind returns 0 if "C_" is found at position 0
                {
                    if (ImGui::Button(nodeName.c_str()))
                    {
                        AddNewNode(nodeName);
                    }
                }
            }
        }

        // Decorator Nodes
        if (ImGui::CollapsingHeader("Decorator Nodes"))
        {
            // List all decorator nodes
            for (const auto& pair : NodeFactory::GetNodePrototypes())
            {
                const std::string& nodeName = pair.first;
                // Check if nodeName starts with "D_"
                if (nodeName.rfind("D_", 0) == 0)
                {
                    if (ImGui::Button(nodeName.c_str()))
                    {
                        AddNewNode(nodeName);
                    }
                }
            }
        }

        // Leaf Nodes
        if (ImGui::CollapsingHeader("Leaf Nodes"))
        {
            // List all leaf nodes
            for (const auto& pair : NodeFactory::GetNodePrototypes())
            {
                const std::string& nodeName = pair.first;
                // Check if nodeName starts with "L_"
                if (nodeName.rfind("L_", 0) == 0)
                {
                    if (ImGui::Button(nodeName.c_str()))
                    {
                        AddNewNode(nodeName);
                    }
                }
            }
        }
    }
    Ref<Node> BTNodeEditorPanel::FindRootNode()
    {
        for (const auto& node : m_Nodes)
        {
            // If the node has no input links, it's a root node
            bool hasInputLinks = false;
            for (const auto& inputPin : node->Inputs)
            {
                if (IsPinLinked(inputPin->ID))
                {
                    hasInputLinks = true;
                    break;
                }
            }

            if (!hasInputLinks)
            {
                return node;
            }
        }

        return nullptr; // No root node found
    }
    
    bool BTNodeEditorPanel::IsPinLinked(ed::PinId pinId)
    {
        for (const auto& link : m_Links)
        {
            if (link->StartPinID == pinId || link->EndPinID == pinId)
                return true;
        }
        return false;
    }

    void BTNodeEditorPanel::SaveBehaviorTree(const std::string& filename)
    {
        // Find the root node
        auto rootNode = FindRootNode();
        if (!rootNode)
        {
            BOREALIS_CORE_INFO("No root node found in the behavior tree.");
            return;
        }
        YAML::Emitter out;
        out << YAML::BeginMap; // Start BehaviourTreeComponent map
        out << YAML::Key << "BehaviourTreeComponent";
        out << YAML::Value << YAML::BeginMap; // Start BehaviourTree map
        out << YAML::Key << "BehaviourTree";
        out << YAML::Value << YAML::BeginMap;

        // Tree Name
        out << YAML::Key << "Tree Name" << YAML::Value << std::filesystem::path(filename).stem().string();;

        // Start serialization from the root node
        SerializeNode(out, rootNode, 0);

        out << YAML::EndMap; // End BehaviourTree map
        out << YAML::EndMap; // End BehaviourTreeComponent map
        std::cout << filename << std::endl;
        // Write to file
        std::ofstream fout(filename);
        if (!fout.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }
        else {
            fout << out.c_str();
        }

    }
    void BTNodeEditorPanel::SerializeNode(YAML::Emitter& out, Ref<Node> node, int depth)
    {
        out << YAML::Key << "name" << YAML::Value << node->Name;
        out << YAML::Key << "depth" << YAML::Value << depth;

        std::vector<Ref<Node>> childNodes = GetChildNodes(node);

        if (!childNodes.empty())
        {
            out << YAML::Key << "children" << YAML::Value << YAML::BeginSeq;

            for (const auto& child : childNodes)
            {
                out << YAML::BeginMap;
                SerializeNode(out, child, depth + 1);
                out << YAML::EndMap;
            }

            out << YAML::EndSeq;
        }
    }
    std::vector<Ref<Node>> BTNodeEditorPanel::GetChildNodes(Ref<Node> node)
    {
        std::vector<Ref<Node>> children;

        for (const auto& outputPin : node->Outputs)
        {
            for (const auto& link : m_Links)
            {
                if (link->StartPinID == outputPin->ID)
                {
                    auto endPin = FindPin(link->EndPinID);
                    if (endPin && endPin->Node)
                    {
                        // Avoid adding duplicate children
                        if (std::find(children.begin(), children.end(), endPin->Node) == children.end())
                        {
                            children.push_back(endPin->Node);
                        }
                    }
                }
            }
        }

        return children;
    }
    void BTNodeEditorPanel::ValidateBehaviorTree()
    {
        std::vector<std::string> validationErrors;

        // Check for a single root node
        auto rootNodes = FindRootNodes();
        if (rootNodes.size() == 0)
        {
            validationErrors.push_back("No root node found in the behavior tree.");
        }
        else if (rootNodes.size() > 1)
        {
            validationErrors.push_back("Multiple root nodes found. There should be only one root node.");
        }
        else
        {
            // Check if the root node is of type Control Flow
            auto rootNode = rootNodes.front();
            if (!IsControlFlowNode(rootNode))
            {
                validationErrors.push_back("The root node must be a Control Flow node.");
            }
        }

        // Check for unconnected nodes
        auto unconnectedNodes = FindUnconnectedNodes();
        if (!unconnectedNodes.empty())
        {
            validationErrors.push_back("There are unconnected nodes in the behavior tree:");
            for (const auto& node : unconnectedNodes)
            {
                validationErrors.push_back(" - " + node->Name);
            }
        }

        // Check that all maximum depth nodes are leaf nodes
        std::vector<Ref<Node>> maxDepthNodes = GetMaxDepthNodes(rootNodes);
        for (const auto& node : maxDepthNodes)
        {
            if (!IsLeafNode(node))
            {
                validationErrors.push_back("Node '" + node->Name + "' at maximum depth is not a leaf node.");
            }
        }

        // Display validation results
        if (validationErrors.empty())
        {
            m_ValidationMessage = "Behavior tree is valid.";
        }
        else
        {
            m_ValidationMessage = "Behavior tree validation failed:\n";
            for (const auto& error : validationErrors)
            {
                m_ValidationMessage += error + "\n";
            }
        }

        ImGui::OpenPopup("ValidationResult");
    }
    std::vector<Ref<Node>> BTNodeEditorPanel::FindRootNodes()
    {
        std::vector<Ref<Node>> rootNodes;
        for (const auto& node : m_Nodes)
        {
            bool hasInputLinks = false;
            for (const auto& inputPin : node->Inputs)
            {
                if (IsPinLinked(inputPin->ID))
                {
                    hasInputLinks = true;
                    break;
                }
            }

            if (!hasInputLinks)
            {
                rootNodes.push_back(node);
            }
        }
        return rootNodes;
    }
    bool BTNodeEditorPanel::IsControlFlowNode(Ref<Node> node)
    {
        return node->Type == NodeType::CONTROLFLOW || node->Name.substr(0, 2) == "C_";
    }
    std::vector<Ref<Node>> BTNodeEditorPanel::FindUnconnectedNodes()
    {
        std::vector<Ref<Node>> unconnectedNodes;
        for (const auto& node : m_Nodes)
        {
            bool isConnected = false;
            // Check if node has any input or output links
            for (const auto& pin : node->Inputs)
            {
                if (IsPinLinked(pin->ID))
                {
                    isConnected = true;
                    break;
                }
            }
            if (!isConnected)
            {
                for (const auto& pin : node->Outputs)
                {
                    if (IsPinLinked(pin->ID))
                    {
                        isConnected = true;
                        break;
                    }
                }
            }
            if (!isConnected)
            {
                unconnectedNodes.push_back(node);
            }
        }
        return unconnectedNodes;
    }
    std::vector<Ref<Node>> BTNodeEditorPanel::GetMaxDepthNodes(const std::vector<Ref<Node>>& rootNodes)
    {
        std::vector<Ref<Node>> maxDepthNodes;
        std::unordered_set<Ref<Node>> visited;

        for (const auto& rootNode : rootNodes)
        {
            GetMaxDepthNodesRecursive(rootNode, maxDepthNodes, visited);
        }

        return maxDepthNodes;
    }

    void BTNodeEditorPanel::GetMaxDepthNodesRecursive(Ref<Node> node, std::vector<Ref<Node>>& maxDepthNodes, std::unordered_set<Ref<Node>>& visited)
    {
        if (!node || visited.count(node))
            return;

        visited.insert(node);

        auto childNodes = GetChildNodes(node);
        if (childNodes.empty())
        {
            maxDepthNodes.push_back(node);
        }
        else
        {
            for (const auto& child : childNodes)
            {
                GetMaxDepthNodesRecursive(child, maxDepthNodes, visited);
            }
        }
    }
    bool BTNodeEditorPanel::IsLeafNode(Ref<Node> node)
    {
        return node->Type == NodeType::LEAF || node->Name.substr(0, 2) == "L_";
    }

}

