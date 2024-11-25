/******************************************************************************
/*!
\file       CubemapPanel.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <Panels/CubemapPanel.hpp>
#include <Assets/AssetManager.hpp>

#include <imgui.h>

#include <string>

namespace Borealis
{
	void CubemapPanel::RenderCubemapSetting()
	{
		if (ImGui::CollapsingHeader("Cubemap Settings"))
		{
            static std::string cubemapTextures[6] = {
                "Right (+X)", "Left (-X)", "Top (+Y)",
                "Bottom (-Y)", "Front (+Z)", "Back (-Z)"
            };

            static char textureName[6][256] = { "", "", "", "", "", "" };
            static std::array<std::filesystem::path, 6> textureAssetPath;

            for (int i = 0; i < 6; ++i)
            {
                ImGui::PushID(i);

                // Display a text box for each cubemap face
                ImGui::Text("%s:", cubemapTextures[i].c_str());
                ImGui::SameLine();

                // Drag-and-drop target and display the file path
                if (ImGui::InputText("##Path", textureName[i], sizeof(textureName[i]), ImGuiInputTextFlags_ReadOnly))
                {
                    // Optional: Handle manual file path input here (if not using drag-and-drop).
                }

                // Add a drag-and-drop target for each face
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                    {
                        AssetHandle data = *(const uint64_t*)payload->Data;

                        AssetMetaData metaData = AssetManager::GetMetaData(data);

                        strncpy(textureName[i], metaData.name.c_str(), sizeof(textureName[i]) - 1);
                        textureName[i][sizeof(textureName[i]) - 1] = '\0';
                        textureAssetPath[i] = metaData.CachePath;
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::PopID();
            }

            if (ImGui::Button("Set Cubemap"))
            {
                TextureConfig cubemapConfig;
                cubemapConfig.type = TextureType::_CUBE;
                
                // Use filePaths array to load the cubemap textures
                for (int i = 0; i < 6; ++i)
                {
                    if (textureAssetPath[i].empty()) break;
                }

                //temp use the first texture source path as source path of cubemap
                
            }
		}
	}
}

