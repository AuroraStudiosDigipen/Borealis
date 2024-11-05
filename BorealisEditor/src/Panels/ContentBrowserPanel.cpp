/******************************************************************************/
/*!
\file		ContentBrowserPanel.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 13, 2024
\brief		Defines the class Content Browser in Level Editor

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/
#include <imgui.h>
#include <imgui_internal.h>
#include <Panels/ContentBrowserPanel.hpp>
#include <Core/LoggerSystem.hpp>
#include <Scene/SceneManager.hpp>
#include <Scene/Serialiser.hpp>
#include <Scene/Scene.hpp>
#include <Prefab.hpp>
#include <Assets/AssetMetaData.hpp>
#include <Assets/AssetManager.hpp>
#include <EditorAssets/MetaSerializer.hpp>
#include <ResourceManager.hpp>

#include "EditorAssets/MaterialEditor.hpp"
#include <EditorAssets/AssetImporter.hpp>

namespace ImGui
{
	static bool BeginDrapDropTargetWindow(const char* payload_type)
	{
		using namespace ImGui;
		ImRect inner_rect = GetCurrentWindow()->InnerRect;
		if (BeginDragDropTargetCustom(inner_rect, GetID("##WindowBgArea")))
			if (const ImGuiPayload* payload = AcceptDragDropPayload(payload_type, ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				if (payload->IsPreview())
				{
					ImDrawList* draw_list = GetForegroundDrawList();
					draw_list->AddRectFilled(inner_rect.Min, inner_rect.Max, GetColorU32(ImGuiCol_DragDropTarget, 0.05f));
					draw_list->AddRect(inner_rect.Min, inner_rect.Max, GetColorU32(ImGuiCol_DragDropTarget), 0.0f, 0, 2.0f);
				}
				if (payload->IsDelivery())
					return true;
				EndDragDropTarget();
			}
		return false;
	}
}
namespace Borealis
{
	UUID ContentBrowserPanel::sSelectedAsset = 0;
	static ImVec2 latestMousePos;
	ContentBrowserPanel::ContentBrowserPanel() : mCurrDir("assets")
	{
		// Load by serialisation in the future
		mAssetsDir = "assets";
	}


	void ContentBrowserPanel::ImGuiRender() 
	{
		ImGui::Begin("Content Browser");



		ImVec2 windowSize = ImGui::GetWindowSize();
		float scrollableHeight = windowSize.y - 100; // Adjust for the fixed bottom row
		ImGui::BeginChild("ScrollableRegion", ImVec2(windowSize.x, scrollableHeight), true);
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropEntity"))
			{
				const char* data = (const char*)payload->Data;
				
			}

			ImGui::EndDragDropTarget();
		}

		// Begin the upper scrollable section


		if (mCurrDir != std::filesystem::path(mAssetsDir))
		{
			if (ImGui::Button("<-"))
			{
				mCurrDir = mCurrDir.parent_path();
			}
		}

		// Right click
		if (Project::GetProjectPath() != "")
		{
			ImGuiPopupFlags popupFlagsItem = ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_NoOpenOverExistingPopup;
			if (ImGui::BeginPopupContextWindow(0, popupFlagsItem))
			{
				if (ImGui::MenuItem("Create Folder"))
				{
					// Create a folder
				}
				if (ImGui::MenuItem("Create Scene"))
				{
					// Create a Scene
					isCreatingScene = true;
					latestMousePos = ImGui::GetMousePos();
				}
				if (ImGui::MenuItem("Create New Material"))
				{
					isCreatingMaterial = true;
					latestMousePos = ImGui::GetMousePos();

					//std::filesystem::path materialPath = mCurrDir;
					//materialPath /= "NewMaterial.mat";
					//Ref<Material> material = Material::CreateNewMaterial(materialPath);
					////TEMP
					//AssetMetaData data = MetaFileSerializer::CreateAssetMetaFile(materialPath);
					//AssetManager::InsertMetaData(data);
					//AssetImporter::InsertAssetHandle(materialPath, data.Handle);
					//MaterialEditor::SetMaterial(data.Handle);
				}

				ImGui::EndPopup();
			}
		}

		if (isCreatingScene)
		{
			ImGui::SetNextWindowPos(latestMousePos);
			ImGui::Begin("Create File", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::InputText("##Filename", textBuffer, sizeof(textBuffer));

			if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
				isCreatingScene = false; // Exit rename mode
				// Create a Scene
				SceneManager::CreateScene(textBuffer, mCurrDir.string());
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
				isCreatingScene = false; // Cancel rename
			}
			ImGui::End();
		}

		if (isCreatingMaterial)
		{
			ImGui::SetNextWindowPos(latestMousePos);
			ImGui::Begin("Create File ##material", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::InputText("##MaterialFilename", textBuffer, sizeof(textBuffer));

			if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
				isCreatingMaterial = false;

				std::filesystem::path materialPath = mCurrDir;
				materialPath /= std::string(textBuffer) + ".mat";
				//creating it like this will call the filewatcher as well, making two asset handle
				//undefined behaviour
				//move creation of assets to assets importer?
				//AssetHandle AsseetImporter::CreateAsset(AssetType Material, Path materialPath)
				Ref<Material> material = Material::CreateNewMaterial(materialPath);
				AssetMetaData data = MetaFileSerializer::CreateAssetMetaFile(materialPath);
				AssetManager::InsertMetaData(data);
				AssetImporter::InsertAssetHandle(materialPath, data.Handle);
				MaterialEditor::SetMaterial(data.Handle);
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
				isCreatingMaterial = false; 
			}
			ImGui::End();
		}


		float panelWidth = ImGui::GetContentRegionAvail().x;
		int cellSize = mThumbnailSize + mPadding;
		int columnCount = (int)(panelWidth / cellSize);
		columnCount = columnCount < 1 ? 1 : columnCount;
		float printedThumbnailSize = (float)mThumbnailSize;
		bool dragDrop = false;

		if (mThumbnailSize != mMinThumbnailSize)
		{
			ImGui::Columns(columnCount);
			ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
		}
		if (Project::GetProjectPath() != "")
		for (auto& entry : std::filesystem::directory_iterator(mCurrDir))
		{

			const std::filesystem::path& path = entry.path();
			std::string filenameStr = path.filename().string();
			std::string extension = path.extension().string();
			if (extension == ".meta") // Skip meta files
			{
				continue;
			}
			ImGui::PushID(filenameStr.c_str());
			uint64_t screenID = 0;
			if (entry.is_directory())
			{
				screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Directory)->GetRendererID());
			}
			else
			{
				if (extension == ".meta")
				{
					ImGui::PopID();
					continue;
				}
				else if (extension == ".png" || extension == ".jpg" || extension == ".tiff" || extension == ".jpeg")
				{
					screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Texture)->GetRendererID());
				}
				else if (extension == ".txt")
				{
					screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Text)->GetRendererID());
				}
				else if (extension == ".sc")
				{
					screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Scene)->GetRendererID());
				}
				else if (extension == ".glsl")
				{
					screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Shader)->GetRendererID());
				}
				else if (extension == ".ttf")
				{
					screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Font)->GetRendererID());
				}
				else if (extension == ".cs")
				{
					screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Script)->GetRendererID());
				}
				else if (extension == ".fbx" || extension == ".obj")
				{
					screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Mesh)->GetRendererID());
				}
				else if (extension == ".mat")
				{
					screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Material)->GetRendererID());
				}
				//else if (extension == ".mp3" || extension == ".wav")
				//{
				//	screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Audio)->GetRendererID());
				//}
				else
				{
					screenID = static_cast<uint64_t>(ResourceManager::GetFileIcon(FileIcon::Unknown)->GetRendererID());
				}
			}
			
			if (mThumbnailSize == mMinThumbnailSize)
			{
				printedThumbnailSize = 0;
				ImGui::Image((ImTextureID)screenID, { ImGui::GetFontSize(), ImGui::GetFontSize() });
				ImGui::SameLine();
				if (ImGui::Button(filenameStr.c_str()))
				{
					sSelectedAsset = AssetImporter::GetAssetHandle(path);
				}
			}
			else
			{
				if (ImGui::ImageButton((ImTextureID)screenID, { printedThumbnailSize, printedThumbnailSize }, { 0,1 }, { 1,0 }, -1, { 0,0,0,0 }, { 1,1,1,1 }))
				{
					sSelectedAsset = AssetImporter::GetAssetHandle(path);
				}
				if (sSelectedAsset == AssetImporter::GetAssetHandle(path))
				{
					ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));
				}
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete"))
					{
					}

					if (ImGui::MenuItem("Rename"))
					{
					}
					ImGui::EndPopup();
				}
			}
			if (ImGui::BeginDragDropSource())
			{
				//auto relativePath = std::filesystem::relative(path, mAssetsDir);
				//std::string itemPath = relativePath.string();
				std::string itemPath = std::filesystem::path(path).string();

				//AssetMetaData metaData = MetaFileSerializer::GetAssetMetaDataFile(std::filesystem::path(path).replace_extension(".meta"));
				AssetHandle assetHandle = AssetImporter::GetAssetHandle(std::filesystem::path(path));
				//get the meta file instead and pass the handle as the data?
				const char* itemPathcStr = itemPath.c_str();

				std::string payloadName;
				if (entry.is_directory())
				{

				}
				else
				{
					// convert extension to all lower case
					std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
					if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
					{
						payloadName = "DragDropImageItem";
					}
					else if (extension == ".sc")
					{
						payloadName = "DragDropSceneItem";
					}
					else if (extension == ".fbx" || extension == ".obj")
					{
						payloadName = "DragDropMeshItem";
					}
					else if (extension == ".anim")
					{
						payloadName = "DragDropAnimationItem";
					}
					else if (extension == ".prefab")
					{
						// Correct the assignment of payloadName
						payloadName = "DragDropEntityItem";
					}
					else if (extension == ".mp3" || extension == ".wav")
					{
						payloadName = "DragDropAudioItem";
					}
					else if (extension == ".mat")
					{
						payloadName = "DragDropMaterialItem";
					}
				}

				ImGui::SetDragDropPayload(payloadName.c_str(), &assetHandle, sizeof(AssetHandle), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}
			
			
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.is_directory())
				{
					mCurrDir /= path.filename();
				}
				else
				{

				}
			}

			if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				sSelectedAsset = 0;
			}

			if (mThumbnailSize != mMinThumbnailSize)
			{
				ImGui::TextWrapped(filenameStr.c_str());
				ImGui::NextColumn();
			}
			ImGui::PopID();
		}
		if (mThumbnailSize != mMinThumbnailSize)
		{
			ImGui::PopStyleColor();
			ImGui::Columns(1);
		}

		//WORK IN PROGRESS
//Dragged Prefab
// Content Browser panel drop target for creating prefabs
		if (ImGui::BeginDrapDropTargetWindow("DragCreatePrefab"))
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragCreatePrefab"))
			{
				UUID droppedEntityID = *(const UUID*)payload->Data; // Retrieve the dragged entity ID

				// Retrieve the entity using the dropped entity ID
				Entity droppedEntity = SceneManager::GetActiveScene()->GetEntityByUUID(droppedEntityID); // Assuming you have a function to get an entity by UUID

				Ref<Prefab> prefab = MakeRef<Prefab>(droppedEntity);
				prefab->AddChild(MakeRef<Entity>(droppedEntity));

				Entity makePrefab(prefab->GetPrefabID(), PrefabManager::GetScenePtr());
				std::string dir = mCurrDir.string();
				dir += +"\\" + droppedEntity.GetName() + ".prefab";
				Serialiser::SerialisePrefab(dir.c_str(), makePrefab);
				PrefabManager::Register(prefab);

				AssetManager::InsertMetaData(MetaFileSerializer::CreateAssetMetaFile(dir, prefab->GetUUID()));

				// You might want to add some feedback or a log message here to indicate success
				std::cout << "Prefab created at: " << mCurrDir.string() << std::endl;
			}
			ImGui::EndDragDropTarget();
		}

		// End the upper scrollable section
		ImGui::EndChild();
		if (ImGui::SliderInt("##Thumbnail Size", &mThumbnailSize, mMinThumbnailSize, 512, "", ImGuiSliderFlags_NoInput))
		{
			mPadding = mThumbnailSize/10;
		}
		ImGui::End();
	}
	void ContentBrowserPanel::SetCurrDir(std::string path)
	{
		mCurrDir = std::filesystem::path(path);
		mAssetsDir = path;
	}
}
