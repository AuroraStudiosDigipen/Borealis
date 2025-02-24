/******************************************************************************
/*!
\file       SceneRendererPanel.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       February 24, 2025
\brief      Defines

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>

#include <Panels/SceneRendererPanel.hpp>

#include <Graphics/Texture.hpp>
#include "Assets/AssetManager.hpp"
#include "Assets/AssetConfigs.hpp"

#include <imgui.h>

namespace Borealis
{
	SceneRenderPanel::SceneRenderPanel() {

	}

	void SceneRenderPanel::ImGuiRender()
	{
		ImGui::Begin("Scene Render");

		static char skyboxImageName[256] = "";
		ImGui::InputText("Skybox Image", skyboxImageName, IM_ARRAYSIZE(skyboxImageName), ImGuiInputTextFlags_ReadOnly);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
			{
				AssetHandle data = *(const uint64_t*)payload->Data;
				AssetMetaData metaData = AssetManager::GetMetaData(data);
				if(GetConfig<TextureConfig>(metaData.Config).shape == TextureShape::_CUBE)
				{
					TextureCubeMap::SetDefaultCubeMap(metaData);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}
}

