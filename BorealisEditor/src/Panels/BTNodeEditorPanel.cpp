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
#include <AI/BehaviourTree/NodeHeaderCodeFormat.hpp>
#include <Core/Project.hpp>
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
        ImGui::Text("Editing Tree: ");
        ImGui::SameLine();
        // Optionally, allow the user to edit the tree name
        static char treeNameTextBuffer[256];
        strcpy(treeNameTextBuffer, m_TreeName.c_str());
        if (ImGui::InputText("Tree Name", treeNameTextBuffer, ImGuiInputTextFlags_Multiline,sizeof(treeNameTextBuffer)))
        {
            m_TreeName = treeNameTextBuffer;
        }
        ImGui::SameLine();
        if (ImGui::Button("New Node"))
        {
            ImGui::OpenPopup("NewNodePopup");
        }

        if (ImGui::BeginPopup("NewNodePopup"))
        {
            static int selectedNodeType = 0; // 0: Leaf, 1: Decorator
            static char nodeName[128] = "";

            const char* nodeTypes[] = { "Leaf", "Decorator" };
            ImGui::Text("Select Node Type:");
            ImGui::Combo("##NodeType", &selectedNodeType, nodeTypes, IM_ARRAYSIZE(nodeTypes));

            ImGui::Text("Enter Node Name:");
            ImGui::InputText("##NodeName", nodeName, IM_ARRAYSIZE(nodeName));

            if (ImGui::Button("Create"))
            {
                CreateNewNodeType(selectedNodeType, nodeName);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            ImGui::OpenPopup("LoadBehaviorTreePopup");
        }

        if (ImGui::BeginPopup("LoadBehaviorTreePopup"))
        {
            static int selectedIndex = -1;
            auto behaviorTrees = GetAvailableBehaviorTrees();
            std::vector<const char*> behaviorTreeNames;
            for (const auto& name : behaviorTrees)
                behaviorTreeNames.push_back(name.c_str());

            ImGui::Text("Select Behavior Tree:");
            ImGui::ListBox("##BehaviorTreeList", &selectedIndex, behaviorTreeNames.data(), behaviorTreeNames.size(), 5);

            if (ImGui::Button("Open") && selectedIndex >= 0)
            {
                std::filesystem::path directoryPath(".");
                std::string selectedTree = behaviorTrees[selectedIndex] + ".btree";
                LoadBehaviorTree(selectedTree);

                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // Add the Validate Tree button
        ImGui::SameLine();
        if (ImGui::Button("Validate Tree"))
        {
            ValidateBehaviorTree();
        }
        ImGui::SameLine();
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
                
                std::string savePath = Project::GetAssetsPath();
                std::string fullPath = savePath + fileName + ".btree";
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

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
        {
            DeleteSelectedNodes();
            DeleteSelectedLinks();
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            DeleteSelectedNodes();
            DeleteSelectedLinks();
        }
        // Static buffer for the tree name input
        static char treeNameBuffer[256] = "";

        // Check if we need to open the modal
        if (m_OpenTreeNameModal)
        {
            ImGui::OpenPopup("Enter Tree Name");

            // Initialize the buffer with the current tree name
            strcpy(treeNameBuffer, m_TreeName.c_str());

            m_OpenTreeNameModal = false;
        }

        // Render the modal
        if (ImGui::BeginPopupModal("Enter Tree Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Please enter a name for the behavior tree.");
            ImGui::InputText("##TreeNameInput", treeNameBuffer, IM_ARRAYSIZE(treeNameBuffer));

            // Disable the "OK" button if the name is empty
            bool nameEntered = strlen(treeNameBuffer) > 0;
            if (!nameEntered)
                ImGui::BeginDisabled();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                m_TreeName = treeNameBuffer;
                ImGui::CloseCurrentPopup();

                // Now proceed to save the tree with the new name
                SaveBehaviorTree(m_TreeFileName);
            }

            if (!nameEntered)
                ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
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
        //ed::SetCurrentEditor(nullptr);

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

        if (ImGui::BeginPopupModal("NodeCreationResult", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextWrapped("%s", m_NodeCreationMessage.c_str());

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
        {
            return;
        }
        // Get the node type from the prototype
        NodeType nodeType = FindNodeType(nodeName);

        // Set the node color based on type (you can customize colors per type)
        ImColor nodeColor(0, 0, 0); // Default color
        if (nodeType == NodeType::CONTROLFLOW)
        {
            nodeColor = { 255, 234,0 };
        }
        else if (nodeType == NodeType::DECORATOR)
        {
            nodeColor = { 0,0, 255 };
        }
        else if (nodeType == NodeType::LEAF)
        {
            nodeColor = { 0,255,0 };
        }
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
        if (ed::BeginCreate(ImVec4(1,0.8,0,1)),100.f)
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
        if (m_TreeName == "Untitled-No-Name-Entered" || m_TreeName.empty())
        {
            m_OpenTreeNameModal = true;
            return; // Wait for the user to enter a name
        }

        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "BehaviourTreeComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "BehaviourTree" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Tree Name" << YAML::Value << m_TreeName;

        // Build parent-child relationships
        std::unordered_map<int, std::vector<int>> nodeChildrenMap;
        std::unordered_set<int> childNodeIds;
        for (const auto& link : m_Links)
        {
            int startPinId = link->StartPinID.Get();
            int endPinId = link->EndPinID.Get();

            auto startPin = FindPin(ed::PinId(startPinId));
            auto endPin = FindPin(ed::PinId(endPinId));

            if (startPin && endPin)
            {
                int parentNodeId = startPin->Node->ID.Get();
                int childNodeId = endPin->Node->ID.Get();

                nodeChildrenMap[parentNodeId].push_back(childNodeId);
                childNodeIds.insert(childNodeId);
            }
        }

        // Identify root nodes
        std::vector<Ref<Node>> rootNodes;
        for (const auto& node : m_Nodes)
        {
            int nodeId = node->ID.Get();
            if (childNodeIds.find(nodeId) == childNodeIds.end())
            {
                rootNodes.push_back(node);
            }
        }

        // Assign depths to nodes
        std::unordered_set<int> visitedNodeIds;
        std::function<void(Ref<Node>, int)> AssignDepthsRecursive = [&](Ref<Node> node, int depth)
            {
                if (!node || visitedNodeIds.count(node->ID.Get()) > 0)
                    return;

                visitedNodeIds.insert(node->ID.Get());
                node->Depth = depth;

                int nodeId = node->ID.Get();
                const auto& childrenIds = nodeChildrenMap[nodeId];
                for (int childId : childrenIds)
                {
                    auto childNode = FindNode(ed::NodeId(childId));
                    if (childNode)
                    {
                        AssignDepthsRecursive(childNode, depth + 1);
                    }
                }
            };

        for (const auto& rootNode : rootNodes)
        {
            AssignDepthsRecursive(rootNode, 0);
        }

        // Serialize nodes
        out << YAML::Key << "Nodes" << YAML::Value << YAML::BeginSeq;
        for (const auto& node : m_Nodes)
        {
            int nodeId = node->ID.Get();
            int depth = node->Depth;

            out << YAML::BeginMap;
            out << YAML::Key << "ID" << YAML::Value << nodeId;
            out << YAML::Key << "Name" << YAML::Value << node->Name;
            out << YAML::Key << "Type" << YAML::Value << NodeTypeToString(node->Type);
            out << YAML::Key << "Depth" << YAML::Value << depth;

            std::vector<int> childrenIDs = nodeChildrenMap[nodeId];
            out << YAML::Key << "ChildrenIDs" << YAML::Value << childrenIDs;

            // Serialize Position
            out << YAML::Key << "Position" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "x" << YAML::Value << ed::GetNodePosition(node->ID.Get()).x;
            out << YAML::Key << "y" << YAML::Value << ed::GetNodePosition(node->ID.Get()).y;
            out << YAML::EndMap;

            out << YAML::EndMap; // End of node
        }
        out << YAML::EndSeq; // End of Nodes sequence

        out << YAML::EndMap; // End of BehaviourTree
        out << YAML::EndMap; // End of BehaviourTreeComponent

        out << YAML::EndMap; // End of YAML document

        // Write to file
        std::ofstream fout(filename);
        fout << out.c_str();
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
    std::vector<std::string> BTNodeEditorPanel::GetAvailableBehaviorTrees()
    {
        std::vector<std::string> behaviorTrees;
        std::filesystem::path directoryPath(".");
        try {
            if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
                std::cerr << "Directory does not exist or is not a directory: " << directoryPath << std::endl;
                return behaviorTrees;  // Return empty vector
            }

            for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".btree")
                {
                    behaviorTrees.push_back(entry.path().stem().string());
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }

        return behaviorTrees;
    }
    void BTNodeEditorPanel::CreateNewNodeType(int nodeTypeIndex, const std::string& nodeName)
    {
        std::string nodePrefix;
        std::string baseClassName;
        std::string srcfolderPath;
        std::string incfolderPath;

        if (nodeTypeIndex == 0) // CONTROL FLOW
        {
            nodePrefix = "C_";
            baseClassName = "BaseNode<" + nodePrefix + nodeName + ">";
            srcfolderPath = "../Borealis/src/AI/BehaviourTree/ControlFLow/";
            incfolderPath = "../Borealis/inc/AI/BehaviourTree/ControlFlow/";
        }
        else if (nodeTypeIndex == 1) // Decorator
        {
            nodePrefix = "D_";
            baseClassName = "BaseNode<" + nodePrefix + nodeName + ">";
            srcfolderPath = "../Borealis/src/AI/BehaviourTree/Decorator/";
            incfolderPath = "../Borealis/inc/AI/BehaviourTree/Decorator/";
        }
        else if (nodeTypeIndex == 2) //LEAF
        {
            nodePrefix = "L_";
            baseClassName = "BaseNode<" + nodePrefix + nodeName + ">";
            srcfolderPath = "../Borealis/src/AI/BehaviourTree/Leaf/";
            incfolderPath = "../Borealis/inc/AI/BehaviourTree/Leaf/";
        }

        std::string className = nodePrefix + nodeName;

        // Generate file paths
        std::string headerFilePath = incfolderPath + className + ".hpp";
        std::string sourceFilePath = srcfolderPath + className + ".cpp";
        // Check if files already exist
        if (std::filesystem::exists(headerFilePath) || std::filesystem::exists(sourceFilePath))
        {
            m_NodeCreationMessage = "Node files already exist.";
        }
        else
        {
            // Generate the code using templates
            std::string headerCode = NodeHeaderCodeFormat::GenerateHeaderCode(className, baseClassName, nodeTypeIndex);
            std::string sourceCode = NodeHeaderCodeFormat::GenerateSourceCode(className, nodeTypeIndex);

            // Write the files
            NodeHeaderCodeFormat::WriteToFile(headerFilePath, headerCode);
            NodeHeaderCodeFormat::WriteToFile(sourceFilePath, sourceCode);

            // Register the new node in NodeFactory
            //NodeFactory::RegisterNodePrototype(className,MakeRef<BehaviourNode>(className));

            m_NodeCreationMessage = "Node " + className + " created successfully.";
        }

        ImGui::OpenPopup("NodeCreationResult");
    }
    void BTNodeEditorPanel::DeserializeNodes(const YAML::Node& rootNode)
    {
        // Map to store nodes by ID
        std::unordered_map<int, Ref<Node>> nodeMap;

        // Map to store children IDs for each node
        std::unordered_map<int, std::vector<int>> nodeChildrenMap;

        // First pass: Create nodes
        for (const auto& nodeData : rootNode)
        {
            int nodeId = nodeData["ID"].as<int>();
            std::string nodeName = nodeData["Name"].as<std::string>();
            std::string nodeTypeStr = nodeData["Type"].as<std::string>();

            NodeType nodeType = StringToNodeType(nodeTypeStr);

            // Create the node with the given ID
            auto node = std::make_shared<Node>(ed::NodeId(nodeId), nodeName, nodeType);
            InitializeNodePins(node);

            // Add node to the list and map
            m_Nodes.push_back(node);
            nodeMap[nodeId] = node;

            // Store children IDs
            const YAML::Node& childrenIDsNode = nodeData["ChildrenIDs"];
            if (childrenIDsNode && childrenIDsNode.IsSequence())
            {
                for (const auto& childIdNode : childrenIDsNode)
                {
                    int childId = childIdNode.as<int>();
                    nodeChildrenMap[nodeId].push_back(childId);
                }
            }
        }

        // Second pass: Create links and build parent-child relationships
        for (const auto& [parentId, childrenIds] : nodeChildrenMap)
        {
            auto parentNode = nodeMap[parentId];
            for (int childId : childrenIds)
            {
                auto childNode = nodeMap[childId];

                // Create link between parent and child
                if (!parentNode->Outputs.empty() && !childNode->Inputs.empty())
                {
                    auto link = std::make_shared<Link>(ed::LinkId(GetNextId()), parentNode->Outputs.front()->ID, childNode->Inputs.front()->ID);
                    m_Links.push_back(link);
                }
            }
        }

        // Identify root nodes (nodes with no incoming links)
        std::unordered_set<int> nodesWithIncomingLinks;
        for (const auto& link : m_Links)
        {
            auto endPin = FindPin(link->EndPinID);
            if (endPin && endPin->Node)
            {
                nodesWithIncomingLinks.insert(endPin->Node->ID.Get());
            }
        }

        std::vector<Ref<Node>> rootNodes;
        for (const auto& node : m_Nodes)
        {
            if (nodesWithIncomingLinks.find(node->ID.Get()) == nodesWithIncomingLinks.end())
            {
                rootNodes.push_back(node);
            }
        }

        // Assign positions based on depth
        const float levelSpacing = 300.0f; // Vertical spacing between levels
        const float nodeSpacing = 300.0f;  // Horizontal spacing between nodes
        float currentX = 0.0f;             // Starting X position

        for (auto& rootNode : rootNodes)
        {
            AssignPositionsRecursive(rootNode, 0, currentX, nodeSpacing, levelSpacing, nodeMap, nodeChildrenMap);
        }
    }
    void BTNodeEditorPanel::LoadBehaviorTree(const std::string& filepath)
    {
        // Clear existing nodes and links
        m_Nodes.clear();
        m_Links.clear();
        m_NextId = 1;

        // Load and parse the YAML file
        YAML::Node data;
        try {
            data = YAML::LoadFile(filepath);
        }
        catch (const YAML::ParserException& e) {
            std::cerr << "YAML Parsing Error: " << e.what() << std::endl;
            return;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return;
        }

        const YAML::Node& behaviourTreeComponent = data["BehaviourTreeComponent"];
        if (!behaviourTreeComponent)
        {
            std::cerr << "Error: 'BehaviourTreeComponent' not found in the YAML file." << std::endl;
            return;
        }

        const YAML::Node& behaviourTreeNode = behaviourTreeComponent["BehaviourTree"];
        if (!behaviourTreeNode)
        {
            std::cerr << "Error: 'BehaviourTree' not found in the YAML file." << std::endl;
            return;
        }

        // Extract the tree name
        m_TreeName = behaviourTreeNode["Tree Name"].as<std::string>();

        // Deserialize nodes
        const YAML::Node& nodesNode = behaviourTreeNode["Nodes"];
        if (nodesNode && nodesNode.IsSequence())
        {
            std::unordered_map<int, Ref<Node>> nodeMap;            // Node ID -> Node object
            std::unordered_map<int, std::vector<int>> nodeChildrenMap; // Node ID -> List of Child IDs

            int maxNodeId = 0;

            // First pass: Create nodes and collect max node ID
            for (const auto& nodeData : nodesNode)
            {
                int nodeId = nodeData["ID"].as<int>();
                if (nodeId > maxNodeId)
                    maxNodeId = nodeId;

                std::string nodeName = nodeData["Name"].as<std::string>();
                std::string nodeTypeStr = nodeData["Type"].as<std::string>();
                int depth = nodeData["Depth"].as<int>();

                NodeType nodeType = StringToNodeType(nodeTypeStr);

                // Create the node with the given ID
                auto node = std::make_shared<Node>(ed::NodeId(nodeId), nodeName, nodeType);
                node->Depth = depth;

                // Read Position
                const YAML::Node& positionNode = nodeData["Position"];
                if (positionNode)
                {
                    float x = positionNode["x"].as<float>();
                    float y = positionNode["y"].as<float>();
                    node->Position = ImVec2(x, y);
                }
                else
                {
                    node->Position = ImVec2(0.0f, depth * 150.0f);
                }

                // Store node in map
                m_Nodes.push_back(node);
                nodeMap[nodeId] = node;

                // Store children IDs
                const YAML::Node& childrenIDsNode = nodeData["ChildrenIDs"];
                if (childrenIDsNode && childrenIDsNode.IsSequence())
                {
                    std::vector<int> childrenIDs;
                    for (const auto& childIdNode : childrenIDsNode)
                    {
                        int childId = childIdNode.as<int>();
                        childrenIDs.push_back(childId);
                    }
                    nodeChildrenMap[nodeId] = childrenIDs;
                }
            }

            // **Set m_NextId before initializing pins**
            m_NextId = maxNodeId + 1;

            // Initialize pins (after m_NextId is updated)
            for (const auto& [nodeId, node] : nodeMap)
            {
                InitializeNodePins(node);
                ed::SetNodePosition(node->ID, node->Position);
            }

            // Second pass: Create links based on children IDs
            for (const auto& [parentId, childrenIds] : nodeChildrenMap)
            {
                auto parentNodeIt = nodeMap.find(parentId);
                if (parentNodeIt == nodeMap.end())
                {
                    std::cerr << "Warning: Parent node with ID " << parentId << " not found." << std::endl;
                    continue;
                }
                Ref<Node> parentNode = parentNodeIt->second;

                for (int childId : childrenIds)
                {
                    auto childNodeIt = nodeMap.find(childId);
                    if (childNodeIt == nodeMap.end())
                    {
                        std::cerr << "Warning: Child node with ID " << childId << " not found for parent node '" << parentNode->Name << "'." << std::endl;
                        continue;
                    }
                    Ref<Node> childNode = childNodeIt->second;

                    // Create link between output and input pins
                    if (!parentNode->Outputs.empty() && !childNode->Inputs.empty())
                    {
                        auto link = std::make_shared<Link>(ed::LinkId(GetNextId()), parentNode->Outputs.front()->ID, childNode->Inputs.front()->ID);
                        m_Links.push_back(link);
                    }
                }
            }

            // Update m_NextId after creating pins and links
            int maxId = m_NextId - 1;

            // Find the maximum pin ID
            for (const auto& node : m_Nodes)
            {
                for (const auto& pin : node->Inputs)
                {
                    int pinId = pin->ID.Get();
                    if (pinId > maxId)
                        maxId = pinId;
                }
                for (const auto& pin : node->Outputs)
                {
                    int pinId = pin->ID.Get();
                    if (pinId > maxId)
                        maxId = pinId;
                }
            }

            // Find the maximum link ID
            for (const auto& link : m_Links)
            {
                int linkId = link->ID.Get();
                if (linkId > maxId)
                    maxId = linkId;
            }

            // Set m_NextId to be greater than the maximum ID found
            m_NextId = maxId + 1;
        }
        else
        {
            std::cerr << "Error: 'Nodes' not found or not a sequence in the YAML file." << std::endl;
            return;
        }

        // Set editing flags
        m_IsEditingExistingTree = true;
        m_TreeFileName = std::filesystem::path(filepath).stem().string();
    }
    NodeType BTNodeEditorPanel::FindNodeType(const std::string& nodeName)
    {

        if (nodeName.rfind("C_", 0) == 0)
        {
            return NodeType::CONTROLFLOW;
        }
        else if (nodeName.rfind("D_",0) == 0)
        {
            return NodeType::DECORATOR;
        }
        else if (nodeName.rfind("L_",0) == 0)
        {
            return NodeType::LEAF;
        }
        else 
        {
            return NodeType::UNKNOWN;
        }


    }
    void BTNodeEditorPanel::DeleteSelectedNodes()
    {
        // Get selected node IDs
        int selectedNodeCount = ed::GetSelectedObjectCount();
        if (selectedNodeCount > 0)
        {
            std::vector<ed::NodeId> selectedNodes(selectedNodeCount);
            ed::GetSelectedNodes(selectedNodes.data(), selectedNodeCount);

            // Delete nodes
            for (const auto& nodeId : selectedNodes)
            {
                // Use ed::DeleteNode
                if (ed::DeleteNode(nodeId))
                {
                    // Remove node from your internal data structures
                    auto it = std::find_if(m_Nodes.begin(), m_Nodes.end(), [&](const auto& node) {
                        return node->ID == nodeId;
                        });
                    if (it != m_Nodes.end())
                    {
                        m_Nodes.erase(it);
                    }
                }
            }
        }
    }
    void BTNodeEditorPanel::DeleteSelectedLinks()
    {
        int selectedLinkCount = ed::GetSelectedObjectCount();
        if (selectedLinkCount > 0)
        {
            std::vector<ed::LinkId> selectedLinks(selectedLinkCount);
            ed::GetSelectedLinks(selectedLinks.data(), selectedLinkCount);

            // Delete links
            for (const auto& linkId : selectedLinks)
            {
                // Use ed::DeleteLink
                if (ed::DeleteLink(linkId))
                {
                    // Remove link from your internal data structures
                    auto it = std::find_if(m_Links.begin(), m_Links.end(), [&](const auto& link) {
                        return link->ID == linkId;
                        });
                    if (it != m_Links.end())
                    {
                        m_Links.erase(it);
                    }
                }
            }
        }
    }
    std::string BTNodeEditorPanel::NodeTypeToString(NodeType type)
    {
        switch (type)
        {
        case NodeType::CONTROLFLOW: return "ControlFlow";
        case NodeType::DECORATOR: return "Decorator";
        case NodeType::LEAF: return "Leaf";
        default: return "Unknown";
        }
    }

    Ref<Node> BTNodeEditorPanel::FindNodeByInputPinID(ed::PinId pinID)
    {
        for (const auto& node : m_Nodes)
        {
            for (const auto& inputPin : node->Inputs)
            {
                if (inputPin->ID == pinID)
                    return node;
            }
        }
        return nullptr;
    }
    void BTNodeEditorPanel::InitializeNodePins(const Ref<Node> node)
    {
        switch (node->Type)
        {
        case NodeType::CONTROLFLOW:
            // Control flow nodes generally have both input and output pins
            node->Inputs.push_back(std::make_shared<Pin>(GetNextId(), "In", PinType::Flow, node, PinKind::Input));
            node->Outputs.push_back(std::make_shared<Pin>(GetNextId(), "Out", PinType::Flow, node, PinKind::Output));
            break;

        case NodeType::DECORATOR:
            // Decorator nodes generally have one input and one output
            node->Inputs.push_back(std::make_shared<Pin>(GetNextId(), "In", PinType::Flow, node, PinKind::Input));
            node->Outputs.push_back(std::make_shared<Pin>(GetNextId(), "Out", PinType::Flow, node, PinKind::Output));
            break;

        case NodeType::LEAF:
            // Leaf nodes typically only have an input pin and no output pin
            node->Inputs.push_back(std::make_shared<Pin>(GetNextId(), "In", PinType::Flow, node, PinKind::Input));
            break;

        default:
            // Unknown types may have no pins, or you could choose a default configuration
            std::cerr << "Unknown node type: " << static_cast<int>(node->Type) << std::endl;
            break;
        }
    }
    // Add this function to your BTNodeEditorPanel class
    NodeType BTNodeEditorPanel::StringToNodeType(const std::string& typeStr)
    {
        if (typeStr == "ControlFlow")
            return NodeType::CONTROLFLOW;
        else if (typeStr == "Decorator")
            return NodeType::DECORATOR;
        else if (typeStr == "Leaf")
            return NodeType::LEAF;
        else
            return NodeType::UNKNOWN;
    }

    void BTNodeEditorPanel::AssignPositionsRecursive(
        Ref<Node> node,
        int depth,
        float& currentX,
        float nodeSpacing,
        float levelSpacing,
        std::unordered_map<int, Ref<Node>>& nodeMap,
        std::unordered_map<int, std::vector<int>>& nodeChildrenMap)
    {
        if (!node)
            return;

        // Set node position
        node->Position = ImVec2(currentX, depth * levelSpacing);
        // Save the starting X position for this node's children
        float startX = currentX;

        // Get the children of this node
        const auto& childIds = nodeChildrenMap[node->ID.Get()];
        int childCount = static_cast<int>(childIds.size());

        // If the node has children, process them
        if (childCount > 0)
        {
            // For centering the parent node over its children
            float childrenXSum = 0.0f;

            for (int childId : childIds)
            {
                auto childNode = nodeMap[childId];
                AssignPositionsRecursive(childNode, depth + 1, currentX, nodeSpacing, levelSpacing, nodeMap, nodeChildrenMap);
                childrenXSum += childNode->Position.x;
            }

            // Center this node over its children
            node->Position.x = childrenXSum / childCount;
        }
        else
        {
            // Leaf node, move to next position
            currentX += nodeSpacing;
        }
    }
    Ref<Node> BTNodeEditorPanel::FindNode(ed::NodeId id)
    {
        for (const auto& node : m_Nodes)
        {
            if (node->ID == id)
                return node;
        }
        return nullptr;
    }

}
