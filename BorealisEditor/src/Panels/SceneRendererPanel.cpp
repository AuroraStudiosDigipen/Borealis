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

#include "Scene/SceneManager.hpp"

#include <imgui.h>

namespace Borealis
{
	SceneRenderPanel::SceneRenderPanel() {

	}

	void SceneRenderPanel::ImGuiRender()
	{
		RenderGraph::SceneRenderConfig& config = SceneManager::GetActiveScene()->GetSceneRenderConfig();
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

		if (ImGui::SliderFloat("Exposure", &config.ubo.exposure, 0.0f, 10.0f, "%.2f"))
		{
			config.ubo.exposure = round(config.ubo.exposure * 100.0f) / 100.0f;
		}

		if (ImGui::SliderFloat("Gamma", &config.ubo.gamma, 0.0f, 10.0f, "%.2f"))
		{
			config.ubo.gamma = round(config.ubo.gamma * 100.0f) / 100.0f;
		}

		ImGui::Text("Bloom Options");

		ImGui::Checkbox("Enable Bloom", &config.bloom);
		
		if (ImGui::SliderFloat("Threshold", &config.ubo.threshold, 0.0f, 10.0f, "%.2f"))
		{
			config.ubo.threshold = round(config.ubo.threshold * 100.0f) / 100.0f;
		}
		if (ImGui::SliderFloat("Knee", &config.ubo.knee, 0.0f, 10.0f, "%.4f"))
		{
			config.ubo.knee = round(config.ubo.knee * 10000.0f) / 10000.0f;
		}
		if (ImGui::SliderFloat("Bloom Scale", &config.ubo.bloomScale, 0.0f, 10.0f, "%.4f"))
		{
			config.ubo.bloomScale = round(config.ubo.bloomScale * 10000.0f) / 10000.0f;
		}


		ImGui::End();
	}
}

