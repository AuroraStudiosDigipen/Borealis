/******************************************************************************/
/*!
\file		ConsolePanel.cpp
\author 	Liu Chengrong
\par    	email: chengrong.liu\@digipen.edu
\date   	October 30, 2024
\brief		Implements the imgui interface that log messages 

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/


#include <Panels/ConsolePanel.hpp>
#include <Debugging/Console.hpp>
#include <imgui.h>

namespace Borealis
{
    void ConsolePanel::ImGuiRender()
    {
        Console& console = Console::GetInstance();
        static std::string currentTab = "Info"; // Track the active tab
        static char fileNameBuffer[128] = "console_log";

        ImGui::Begin("Console");

        if (ImGui::Button("Save to File"))
        {
            ImGui::OpenPopup("Save Log File");; // Specify the file name
        }

        // Save Log File Popup
        if (ImGui::BeginPopupModal("Save Log File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter file name: ");
            ImGui::InputText("##FileName", fileNameBuffer, IM_ARRAYSIZE(fileNameBuffer)); // Input field for file name

            if (ImGui::Button("Save"))
            {
                std::string fileName = fileNameBuffer;

                // Remove any existing ".txt" extension if present
                if (fileName.length() >= 4 && fileName.substr(fileName.length() - 4) == ".txt")
                {
                    fileName = fileName.substr(0, fileName.length() - 4);
                }

                // Append ".txt" extension
                fileName += ".txt";

                // Save the messages to the specified file
                console.SaveMessagesToFile(fileName);
                ImGui::CloseCurrentPopup(); // Close the popup
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup(); // Close the popup without saving
            }

            ImGui::EndPopup();
        }

        ImGui::SameLine();

        // Clear button with confirmation popup
        if (ImGui::Button("Clear"))
        {
            ImGui::OpenPopup("Clear Confirmation");
        }

        if (ImGui::BeginPopupModal("Clear Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to clear the messages on the current tab?");
            if (ImGui::Button("Yes"))
            {
                // Clear messages based on the current tab
                if (currentTab == "Info")
                    console.ClearInfoMessages();
                else if (currentTab == "Warnings")
                    console.ClearWarningMessages(); 
                else if (currentTab == "Errors")
                    console.ClearErrorMessages();

                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear all tabs"))
            {
                console.Clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        if (ImGui::BeginTabBar("MessageTabs"))
        {
            // Info Tab
            if (ImGui::BeginTabItem(("Info (" + std::to_string(console.GetInfoMessages().size()) + ")").c_str()))
            {
                currentTab = "Info"; // Set current tab to Info
                ImGui::BeginChild("InfoMessagesRegion", ImVec2(0, 0), true); // Scrollable
                int messageId = 0; // Unique ID for each message

                for (const auto& msg : console.GetInfoMessages())
                {
                    ImGui::PushID(messageId++); // Assign a unique ID for each message

                    if (ImGui::Selectable(msg.c_str())){}

                    // Right-click context menu
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Copy"))
                        {
                            ImGui::SetClipboardText(msg.c_str()); // Copy message to clipboard
                        }
                        if (ImGui::MenuItem("Delete"))
                        {
                            // Remove the message from the vector
                            console.DeleteInfoMessage(messageId - 1); // Implement this method in your Console class
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::PopID(); // End unique ID scope
                }
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            // Warnings Tab
            if (ImGui::BeginTabItem(("Warnings (" + std::to_string(console.GetWarningMessages().size()) + ")").c_str()))
            {
                currentTab = "Warnings"; // Set current tab to Warnings
                ImGui::BeginChild("WarningMessagesRegion", ImVec2(0, 0), true); // Scrollable
                int messageId = 0; // Unique ID for each message

                for (const auto& msg : console.GetWarningMessages())
                {
                    ImGui::PushID(messageId++); // Assign a unique ID for each message

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

                    if (ImGui::Selectable(msg.c_str())){}

                    ImGui::PopStyleColor();

                    // Right-click context menu
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Copy"))
                        {
                            ImGui::SetClipboardText(msg.c_str()); // Copy message to clipboard
                        }
                        if (ImGui::MenuItem("Delete"))
                        {
                            // Remove the message from the vector
                            console.DeleteWarningMessage(messageId - 1); // Implement this method in your Console class
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::PopID(); // End unique ID scope
                }
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            // Errors Tab
            if (ImGui::BeginTabItem(("Errors (" + std::to_string(console.GetErrorMessages().size()) + ")").c_str()))
            {
                currentTab = "Errors"; // Set current tab to Errors
                ImGui::BeginChild("ErrorMessagesRegion", ImVec2(0, 0), true); // Scrollable
                int messageId = 0; // Unique ID for each message

                for (const auto& msg : console.GetErrorMessages())
                {
                    ImGui::PushID(messageId++); // Assign a unique ID for each message

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

                    if (ImGui::Selectable(msg.c_str())) {}

                    ImGui::PopStyleColor();

                    // Right-click context menu
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Copy"))
                        {
                            ImGui::SetClipboardText(msg.c_str()); // Copy message to clipboard
                        }
                        if (ImGui::MenuItem("Delete"))
                        {
                            // Remove the message from the vector
                            console.DeleteErrorMessage(messageId - 1); // Implement this method in your Console class
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::PopID(); // End unique ID scope
                }
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }


}