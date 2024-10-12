/******************************************************************************/
/*!
\file		SceneHierarchyPanel.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 13, 2024
\brief		Defines the class Scene Hierarchy in Level Editor

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/ImGuiFontLib.hpp>
#include <Scene/Components.hpp>
#include <Scene/SceneManager.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <Panels/SceneHierarchyPanel.hpp>
#include <Panels/ContentBrowserPanel.hpp>
#include <Physics/PhysicsSystem.hpp>
#include <PrefabManager.hpp>
#include <Prefab.hpp>
#include <PrefabComponent.hpp>

#include <EditorAssets/MeshImporter.hpp>
#include <EditorAssets/FontImporter.hpp>
#include <EditorAssets/AssetImporter.hpp>
#include <Assets/AssetManager.hpp>
//#include <Assets/MeshImporter.hpp>
//#include <Assets/FontImporter.hpp>
#include <EditorLayer.hpp>

#include <Core/Project.hpp>

#include "EditorAssets/MaterialEditor.hpp"


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
	static void DrawVec3Controller(const std::string& label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 100.f)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* bold = io.Fonts->Fonts[ImGuiFonts::bold];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::CalcItemWidth()/3);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10, 0 }); // Spacing between Items

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.f,lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.05f, 0.1f, 1.0f));
		ImGui::PushFont(bold);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(ImGui::CalcItemWidth()/3);

		ImGui::PushFont(bold);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(ImGui::CalcItemWidth()/3);

		ImGui::PushFont(bold);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.2f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.1f, 0.7f, 1.0f));
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	template <typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool allowDelete = true)
	{
		if (entity.HasComponent<T>())
		{
			ImGui::Spacing();
			auto& component = entity.GetComponent<T>();

			bool deleteComponent = false;
			bool open;

			if (allowDelete)
			{
				auto ContentRegionAvailable = ImGui::GetContentRegionAvail();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
				float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
				ImGui::Separator();
				open = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
				ImGui::PopStyleVar();
				ImGui::SameLine(ContentRegionAvailable.x - lineHeight * 0.5f); // Align to right (Button)
				if (ImGui::Button(("+##" + name).c_str(), ImVec2{lineHeight,lineHeight}))
				{
					ImGui::OpenPopup(("ComponentSettingsPopup##" + name).c_str());
				}
				

				if (ImGui::BeginPopup(("ComponentSettingsPopup##" + name).c_str()))
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						deleteComponent = true;
					}

					ImGui::EndPopup();
				}
			}
			else
			{
				open = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			}

			if (open)
			{
				ImGui::Spacing();
				uiFunction(component);
			}

			if (deleteComponent)
			{
				if (typeid(T) == typeid(RigidBodyComponent))
				{
					PhysicsSystem::FreeRigidBody(entity.GetComponent<RigidBodyComponent>());
				}
				entity.RemoveComponent<T>();
			}
		}
	}


	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		mContext = scene;
		mSelectedEntity = {};
	}
	void SceneHierarchyPanel::ImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		ImGuiIO& io = ImGui::GetIO();
		ImFont* bold = io.Fonts->Fonts[ImGuiFonts::bold];
		ImGui::PushFont(bold);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 0.4f));
		for (auto& [name, path] : SceneManager::GetSceneLibrary())
		{
			
			if (SceneManager::GetActiveScene()->GetName() == name)
			{
				ImGui::PopStyleColor();
				ImGui::MenuItem(name.c_str());
				ImGui::PopFont();
				for (auto& item : mContext->mRegistry.view<entt::entity>())
				{
					
					Entity entity{ item, mContext.get() };
					DrawEntityNode(entity);
				}
				ImGui::PushFont(bold);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 0.4f));
			}
			else
			{
				ImGui::MenuItem(name.c_str());
				ImGui::PopStyleColor();
				ImGui::PopFont();
				if (ImGui::BeginPopupContextItem())
				{
					if (EditorLayer::mSceneState == EditorLayer::SceneState::Edit)
					{
						if (ImGui::MenuItem("Load Scene"))
						{
							SceneManager::SaveActiveScene();
							SceneManager::SetActiveScene(name);
							mContext = SceneManager::GetActiveScene();
							mSelectedEntity = {};
						}
					}
					ImGui::EndPopup();
				}
				ImGui::PushFont(bold);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 0.4f));
			}
		}
		ImGui::PopStyleColor();
		ImGui::PopFont();

		if (Project::GetProjectPath() != "")
		{
			ImGuiIO& io = ImGui::GetIO();
			ImFont* bold = io.Fonts->Fonts[ImGuiFonts::bold];
			ImGui::PushFont(bold);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 0.4f));
			for (auto& [name, path] : SceneManager::GetSceneLibrary())
			{

				if (SceneManager::GetActiveScene()->GetName() == name)
				{
					ImGui::PopStyleColor();
					ImGui::MenuItem(name.c_str());
					ImGui::PopFont();
					for (auto& item : mContext->mRegistry.view<entt::entity>())
					{

						Entity entity{ item, mContext.get() };
						DrawEntityNode(entity);
					}
					ImGui::PushFont(bold);
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 0.4f));
				}
				else
				{
					ImGui::MenuItem(name.c_str());
					ImGui::PopStyleColor();
					ImGui::PopFont();
					if (ImGui::BeginPopupContextItem())
					{
						if (EditorLayer::mSceneState == EditorLayer::SceneState::Edit)
						{
							if (ImGui::MenuItem("Load Scene"))
							{
								SceneManager::SaveActiveScene();
								SceneManager::SetActiveScene(name);
								mContext = SceneManager::GetActiveScene();
								mSelectedEntity = {};
							}
						}
						ImGui::EndPopup();
					}
					ImGui::PushFont(bold);
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 0.4f));
				}
			}

			ImGui::PopFont();
			ImGui::PopStyleColor();
		}

		

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) // Deselect
		{
			mSelectedEntity = {};
		}

		// Right click on blank space	
		if (Project::GetProjectPath() != "")
		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				mContext->CreateEntity("Empty");
			}
			ImGui::EndPopup();
		}
		
		//Create Entities from prefab
		if (ImGui::BeginDrapDropTargetWindow("DragPrefab"))
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragPrefab"))
			{
				AssetHandle data = *(const uint64_t*)payload->Data;
				Ref<Prefab> prefab = PrefabManager::GetPrefab(data);
				prefab->CreateChild(mContext);

			}
			ImGui::EndDragDropTarget();
		}


		ImGui::End();

		ImGui::Begin("Inspector");

		if (Project::GetProjectPath() != "")

		{
			if (mSelectedEntity)
			{
				MaterialEditor::SetMaterial(0);
				DrawComponents(mSelectedEntity);
			}
			else if (ContentBrowserPanel::sSelectedAsset)
			{
				AssetMetaData const& metadata = AssetManager::GetMetaData(ContentBrowserPanel::sSelectedAsset);
#ifdef _DEB
				ImGui::Text(("UUID: " + std::to_string(metadata.Handle)).c_str());
#endif
				ImGui::Text(("Name: " + metadata.name).c_str());
				ImGui::Text(("Type: " + Asset::AssetTypeToString(metadata.Type)).c_str());
				ImGui::Text(("Path: " + metadata.SourcePath.string()).c_str());
				switch (metadata.Type)
				{
				case AssetType::Texture2D:
				{
					MaterialEditor::SetMaterial(0);
					break;
				}
				case AssetType::Audio:
				{
					MaterialEditor::SetMaterial(0);
					break;
				}
				case AssetType::Shader:
				{
					MaterialEditor::SetMaterial(0);
					break;
				}
				case AssetType::Mesh:
				{
					MaterialEditor::SetMaterial(0);
					break;
				}
				case AssetType::Material:
				{
					MaterialEditor::SetMaterial(metadata.Handle);
					break;
				}
				default:
				{
					MaterialEditor::SetMaterial(0);
					break;
				}

				}
			}
			MaterialEditor::RenderEditor();
		}

		ImGui::End();

	}
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = ((mSelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		uint64_t entityID = static_cast<uint64_t>((uint32_t)entity);
		bool opened = ImGui::TreeNodeEx((void*)entityID, flags, tag.c_str());

		//Dragging of items for creation of prefab
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("DragCreatePrefab", (const void*)&entity.GetUUID(), sizeof(UUID));
			ImGui::Text("%s", tag.c_str()); // Display the entity tag as the payload text
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemClicked())
		{
			mSelectedEntity = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Destroy Entity"))
			{
				mSelectedEntity = entity;
				entityDeleted = true;
			}

			if (ImGui::MenuItem("Create Empty Entity"))
			{
				mContext->CreateEntity("Empty");
			}

			if (ImGui::MenuItem("Duplicate Entity"))
			{
				mContext->DuplicateEntity(mSelectedEntity);
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		if(entityDeleted)
		{
			mContext->DestroyEntity(mSelectedEntity);
			mSelectedEntity = {};
		}
	}

	template <typename T>
	static void SearchBar(std::string& SearchList, Entity& mSelectedEntity, const char* menuText, char* search_buffer)
	{
		std::string MenuConverter(menuText);
		std::transform(MenuConverter.begin(), MenuConverter.end(), MenuConverter.begin(), ::tolower);
		if (SearchList.empty() || MenuConverter.find(SearchList) != std::string::npos)
			if (ImGui::MenuItem(menuText))
			{
				if (!mSelectedEntity.HasComponent<T>())
				{
					mSelectedEntity.AddComponent<T>();
					
					if (std::is_same<T, CameraComponent>::value)
					{
						mSelectedEntity.GetComponent<TransformComponent>().Translate.z = 350.f;
						mSelectedEntity.GetComponent<CameraComponent>().Camera.SetCameraType(SceneCamera::CameraType::Perspective);
					}

					if (std::is_same<T, MeshFilterComponent>::value)
					{
						if (!mSelectedEntity.HasComponent<MeshRendererComponent>())
							mSelectedEntity.AddComponent<MeshRendererComponent>();
					}
				}
				ImGui::CloseCurrentPopup();
				memset(search_buffer, 0, sizeof(search_buffer));
			}
	}

	

	static void DrawScriptField(Ref<ScriptInstance> component)
	{
		for (const auto& [name, field] : component->GetScriptClass()->mFields) // name of script field, script field
		{
			if (field.mType == ScriptFieldType::Bool)
			{
				bool Data = component->GetFieldValue<bool>(name);
				if (ImGui::Checkbox((name + "##" + component->GetKlassName()).c_str(), &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}
			if (field.mType == ScriptFieldType::Float)
			{
				float Data = component->GetFieldValue<float>(name);
				if (ImGui::DragFloat((name + "##" + component->GetKlassName()).c_str(), &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::Int)
			{
				int Data = component->GetFieldValue<int>(name);
				if (ImGui::DragInt((name + "##" + component->GetKlassName()).c_str(), &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::String)
			{
				std::string Data = component->GetFieldValue<std::string>(name);
				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), Data.c_str());
				if (ImGui::InputText((name + "##" + component->GetKlassName()).c_str(), buffer, sizeof(buffer)))
				{
					Data = std::string(buffer);
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::Vector2)
			{
				glm::vec2 Data = component->GetFieldValue<glm::vec2>(name);
				if (ImGui::DragFloat2((name + "##" + component->GetKlassName()).c_str(), glm::value_ptr(Data)))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::Vector3)
			{
				glm::vec3 Data = component->GetFieldValue<glm::vec3>(name);
				if (ImGui::DragFloat3((name + "##" + component->GetKlassName()).c_str(), glm::value_ptr(Data)))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::Vector4)
			{
				glm::vec4 Data = component->GetFieldValue<glm::vec4>(name);
				if (ImGui::DragFloat4((name + "##" + component->GetKlassName()).c_str(), glm::value_ptr(Data)))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::UChar)
			{
				unsigned char Data = component->GetFieldValue<unsigned char>(name);
				if (ImGui::DragScalar((name + "##" + component->GetKlassName()).c_str(), ImGuiDataType_U8, &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::Char)
			{
				char Data = component->GetFieldValue<char>(name);
				if (ImGui::DragScalar((name + "##" + component->GetKlassName()).c_str(), ImGuiDataType_S8, &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::UShort)
			{
				unsigned short Data = component->GetFieldValue<unsigned short>(name);
				if (ImGui::DragScalar((name + "##" + component->GetKlassName()).c_str(), ImGuiDataType_U16, &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::Short)
			{
				short Data = component->GetFieldValue<short>(name);
				if (ImGui::DragScalar((name + "##" + component->GetKlassName()).c_str(), ImGuiDataType_S16, &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::UInt)
			{
				unsigned int Data = component->GetFieldValue<unsigned int>(name);
				if (ImGui::DragScalar((name + "##" + component->GetKlassName()).c_str(), ImGuiDataType_U32, &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::Long)
			{
				long long Data = component->GetFieldValue<long long>(name);
				if (ImGui::DragScalar((name + "##" + component->GetKlassName()).c_str(), ImGuiDataType_S64, &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::ULong)
			{
				unsigned long long Data = component->GetFieldValue<unsigned long long>(name);
				if (ImGui::DragScalar((name + "##" + component->GetKlassName()).c_str(), ImGuiDataType_U64, &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}

			if (field.mType == ScriptFieldType::Double)
			{
				double Data = component->GetFieldValue<double>(name);
				if (ImGui::DragScalar((name + "##" + component->GetKlassName()).c_str(), ImGuiDataType_Double, &Data))
				{
					component->SetFieldValue(name, &Data);
				}
			}
		}
	}
	static void DrawScriptComponent(ScriptComponent& component, Entity& entity, bool allowDelete = true)
	{
		for (auto& [name, script] : component.mScripts)
		{
			ImGui::Spacing();
			bool deleteComponent = false;
			bool open;

			if (allowDelete)
			{
				auto ContentRegionAvailable = ImGui::GetContentRegionAvail();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
				float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
				ImGui::Separator();
				open = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
				ImGui::PopStyleVar();
				ImGui::SameLine(ContentRegionAvailable.x - lineHeight * 0.5f); // Align to right (Button)
				if (ImGui::Button("+", ImVec2{ lineHeight,lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettingsPopup");
				}


				if (ImGui::BeginPopup("ComponentSettingsPopup"))
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						deleteComponent = true;
					}

					ImGui::EndPopup();
				}
			}
			else
			{
				open = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			}

			if (open)
			{
				ImGui::Spacing();
				DrawScriptField(script);
			}

			if (deleteComponent)
			{
				component.RemoveScript(name);
				if (component.mScripts.empty())
				{
					entity.RemoveComponent<ScriptComponent>();
					return;
				}
			}
		}
	}
	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}

		ImGui::PopItemWidth();

		static char search_buffer[128] = "";
		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			ImGui::InputText("Search", search_buffer, IM_ARRAYSIZE(search_buffer));

			std::string search_text = search_buffer;
			std::transform(search_text.begin(), search_text.end(), search_text.begin(), ::tolower);

	
			SearchBar<SpriteRendererComponent>(search_text, mSelectedEntity, "Sprite Renderer", search_buffer);
			SearchBar<CircleRendererComponent>( search_text, mSelectedEntity, "Circle Renderer", search_buffer);
			SearchBar<CameraComponent>			(search_text, mSelectedEntity, "Camera", search_buffer);
			SearchBar<MeshFilterComponent	  >(search_text, mSelectedEntity,"Mesh Filter", search_buffer);
			SearchBar<MeshRendererComponent	  >(search_text, mSelectedEntity,"Mesh Renderer", search_buffer);
			SearchBar<BoxColliderComponent	  >(search_text, mSelectedEntity,"Box Collider", search_buffer);
			SearchBar<CapsuleColliderComponent>(search_text, mSelectedEntity,"Capsule Collider", search_buffer);
			SearchBar<RigidBodyComponent	  >(search_text, mSelectedEntity,"Rigidbody", search_buffer);
			SearchBar<LightComponent		  >(search_text, mSelectedEntity,"Light", search_buffer);
			SearchBar<TextComponent				>(search_text, mSelectedEntity,"Text", search_buffer);
			SearchBar<BehaviourTreeComponent	>(search_text, mSelectedEntity, "Behaviour Tree", search_buffer);
			SearchBar<AudioSourceComponent		>(search_text, mSelectedEntity, "Audio Source", search_buffer);
			SearchBar<AudioListenerComponent	>(search_text, mSelectedEntity, "Audio Listener", search_buffer);

			// scripts
			for (auto [name, klass] : ScriptingSystem::mScriptClasses)
			{
				if (name == "MonoBehaviour") { continue; }
				if (search_text.empty() || name.find(search_text) != std::string::npos)
					if (ImGui::MenuItem(name.c_str()))
					{
						if (mSelectedEntity.HasComponent<ScriptComponent>() == false)
						{
							mSelectedEntity.AddComponent<ScriptComponent>();
						}
						auto scriptInstance = MakeRef<ScriptInstance>(klass);
						mSelectedEntity.GetComponent<ScriptComponent>().AddScript(name, scriptInstance);
						scriptInstance->Init(mSelectedEntity.GetUUID());
						ImGui::CloseCurrentPopup();
						memset(search_buffer, 0, sizeof(search_buffer));
					}
			}
				
				


			ImGui::EndPopup();
					
		}

		DrawComponent<TransformComponent>("Transform", mSelectedEntity, [](auto& transformComponent)
			{
				ImGuiIO& io = ImGui::GetIO();
				float textScale = io.FontGlobalScale;
				DrawVec3Controller("Translation", transformComponent.Translate,0.f, textScale * 100.f);
				DrawVec3Controller("Rotation", transformComponent.Rotation,0.f, textScale * 100.f);
				DrawVec3Controller("Scale", transformComponent.Scale, 1.f, textScale * 100.f);
			}, false);
		

		DrawComponent<CameraComponent>("Camera", mSelectedEntity, [](auto& cameraComponent)
			{
				const char* CameraTypeStr[]{ "Perspective", "Orthographic" };
				const char* currentCameraTypeStr = CameraTypeStr[(int)cameraComponent.Camera.GetCameraType()];
				if (ImGui::BeginCombo("Projection", currentCameraTypeStr))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentCameraTypeStr == CameraTypeStr[i];
						if (ImGui::Selectable(CameraTypeStr[i], isSelected))
						{
							currentCameraTypeStr = CameraTypeStr[i];
							cameraComponent.Camera.SetCameraType((SceneCamera::CameraType)i);
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}


				if (cameraComponent.Camera.GetCameraType() == SceneCamera::CameraType::Perspective)
				{
					float perspectiveFOV = glm::degrees(cameraComponent.Camera.GetPerspFOV());
					if (ImGui::DragFloat("FOV", &perspectiveFOV))
					{
						cameraComponent.Camera.SetPerspFOV(glm::radians(perspectiveFOV));
					}
					float perspectiveNear = cameraComponent.Camera.GetPerspNear();
					if (ImGui::DragFloat("Near", &perspectiveNear))
					{
						cameraComponent.Camera.SetPerspNear(perspectiveNear);
					}
					float perspectiveFar = cameraComponent.Camera.GetPerspFar();
					if (ImGui::DragFloat("Far", &perspectiveFar))
					{
						cameraComponent.Camera.SetPerspFar(perspectiveFar);
					}
				}
				else if (cameraComponent.Camera.GetCameraType() == SceneCamera::CameraType::Orthographic)
				{
					float orthographicSize = cameraComponent.Camera.GetOrthoSize();
					if (ImGui::DragFloat("Size", &orthographicSize))
					{
						cameraComponent.Camera.SetOrthoSize(orthographicSize);
					}
					float orthographicNear = cameraComponent.Camera.GetOrthoNear();
					if (ImGui::DragFloat("Near", &orthographicNear))
					{
						cameraComponent.Camera.SetOrthoNear(orthographicNear);
					}
					float orthographicFar = cameraComponent.Camera.GetOrthoFar();
					if (ImGui::DragFloat("Far", &orthographicFar))
					{
						cameraComponent.Camera.SetOrthoFar(orthographicFar);
					}

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", mSelectedEntity, [&](auto& component)
			{
				if (ImGui::ColorEdit4("Color", glm::value_ptr(component.Colour)))
				{
					if (mSelectedEntity.HasComponent<PrefabComponent>()) // it's a prefab instance
					{
						auto& pComp = mSelectedEntity.GetComponent<PrefabComponent>();
						pComp.mEditedComponentList.insert("SpriteRendererComponent::Color");
					}
				}
				ImGui::Button("Texture");
				if(ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
					{
						AssetHandle data = *(const uint64_t*)payload->Data;
						component.Texture = AssetManager::GetAsset<Texture2D>(data);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.f);
			});

		DrawComponent<CircleRendererComponent>("Circle Renderer", mSelectedEntity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Colour));
				ImGui::DragFloat("Thickness", &component.thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("fade", &component.fade, 0.005f, 0.0f, 1.0f);
			});

		DrawComponent<MeshFilterComponent>("Mesh Filter", mSelectedEntity, [](auto& component)
			{
				ImGui::Button("Mesh");
				//{
				//	if (!component.Model)
				//		component.Model = MakeRef<Model>();
				//	component.Model->LoadModel();
				//}
				
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropMeshItem"))
					{
						//const char* data = (const char*)payload->Data;
						//std::string meshName = "assets/";
						//meshName += data;
						// Should reference off asset manager's mesh
						// imageName += ".meta";
						// Read UUID from .meta
						// Example Interface: component.mesh = AssetManager::GetMesh(UUID);
						//component.mesh->Load(filename);
						//Model model;
						//LoadModel(meshName, model);
						//component.Model = MakeRef<Model>(model); 
						//component.Model = MeshImporter::LoadFBXModel(meshName);

						//component.Model = MeshImporter::LoadFBXModel("assets/meshes/dragon.fbx");
						AssetHandle data = *(const uint64_t*)payload->Data;
						component.Model = AssetManager::GetAsset<Model>(data);
					}
					ImGui::EndDragDropTarget();
				}
			});
		DrawComponent<MeshRendererComponent>("Mesh Renderer", mSelectedEntity, [](auto& component)
			{
				ImGui::Button("Material");
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropMaterialItem"))
					{
						AssetHandle data = *(const uint64_t*)payload->Data;
						component.Material = AssetManager::GetAsset<Material>(data);
					}
					ImGui::EndDragDropTarget();
				}
				if (component.Material)
				{
					MaterialEditor::RenderProperties(component.Material);
				}

				
				ImGui::Checkbox("Cast Shadow", &component.castShadow);
			});

		DrawComponent<BoxColliderComponent>("Box Collider", mSelectedEntity, [](auto& component)
			{
				ImGui::Checkbox("Is Trigger", &component.isTrigger);
				ImGui::Checkbox("Provides Contact", &component.providesContact);

				float textScale = ImGui::GetIO().FontGlobalScale;
				DrawVec3Controller("Center", component.Center, 0.f, textScale * 100.f);
				DrawVec3Controller("Size", component.Size, 1.f, textScale * 100.f);
			});

		DrawComponent<CapsuleColliderComponent>("Capsule Collider", mSelectedEntity, [](auto& component)
			{
				ImGui::Checkbox("Is Trigger##capsule", &component.isTrigger);
				ImGui::Checkbox("Provides Contact##capsule", &component.providesContact);
				ImGui::DragFloat("Radius", &component.radius, 0.025f);
				ImGui::DragFloat("Height", &component.height, 0.025f);

				const char* DirectionTypeStr[]{ "X-Axis", "Y-Axis", "Z-Axis"};
				const char* currentDirectionTypeStr = DirectionTypeStr[(int)component.direction];
				if (ImGui::BeginCombo("Direction", currentDirectionTypeStr))
				{
					for (int i = 0; i < 3; i++)
					{
						bool isSelected = currentDirectionTypeStr == DirectionTypeStr[i];
						if (ImGui::Selectable(DirectionTypeStr[i], isSelected))
						{
							currentDirectionTypeStr = DirectionTypeStr[i];
							component.direction = (CapsuleColliderComponent::Direction)(i);
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			});

		DrawComponent<RigidBodyComponent>("Rigidbody", mSelectedEntity, [](auto& component)
			{

				if (ImGui::Checkbox("isBox", &component.isBox))
				{
					if (component.isBox)
					{
						PhysicsSystem::UpdateBoxValues(component);
					}
					else
					{
						PhysicsSystem::UpdateSphereValues(component);
					}
				}
				if (component.isBox)
				{
					if (ImGui::DragFloat("HalfExtent", &component.radius))
					{
						PhysicsSystem::UpdateBoxValues(component);
					}
				}
				else
				{
					if (ImGui::DragFloat("Radius", &component.radius))
					{
						PhysicsSystem::UpdateSphereValues(component);
					}
				}

				/*ImGui::DragFloat("Mass", &component.mass, 0.025f);
				ImGui::DragFloat("Drag", &component.drag, 0.025f);
				ImGui::DragFloat("Angular Drag", &component.angularDrag, 0.025f);
				ImGui::Checkbox("Automatic Center of Mass", &component.AutomaticCenterOfMass);
				if (!component.AutomaticCenterOfMass)
				{
					float textScale = ImGui::GetIO().FontGlobalScale;
					DrawVec3Controller("Center of Mass", component.centerOfMass, 0.f, textScale * 100.f);
				}
				ImGui::Checkbox("Automatic Tensor", &component.AutomaticTensor);
				if (!component.AutomaticTensor)
				{
					float textScale = ImGui::GetIO().FontGlobalScale;
					DrawVec3Controller("Inertia Tensor", component.inertiaTensor, 1.f, textScale * 100.f);
					DrawVec3Controller("Inertia Tensor Rotation", component.inertiaTensorRotation, 0.f, textScale * 100.f);
				}
				ImGui::Checkbox("Use Gravity", &component.useGravity);
				ImGui::Checkbox("Kinematic", &component.isKinematic);*/
			});

		if (mSelectedEntity.HasComponent<ScriptComponent>())
		{
			DrawScriptComponent(mSelectedEntity.GetComponent<ScriptComponent>(), mSelectedEntity);
		}

		DrawComponent<LightComponent>("Light", mSelectedEntity, [](auto& component)
			{
				if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
				{
					const char* LightTypeStr[]{ "Spot", "Directional", "Point" };
					const char* currentLightTypeStr = LightTypeStr[(int)component.type];
					if (ImGui::BeginCombo("Type", currentLightTypeStr))
					{
						for (int i = 0; i < 3; i++)
						{
							bool isSelected = currentLightTypeStr == LightTypeStr[i];
							if (ImGui::Selectable(LightTypeStr[i], isSelected))
							{
								currentLightTypeStr = LightTypeStr[i];
								component.type = (LightComponent::Type)(i);
							}
							if (isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGui::ColorEdit3("Ambient", glm::value_ptr(component.ambient));
					ImGui::ColorEdit3("Diffuse", glm::value_ptr(component.diffuse));
					ImGui::ColorEdit3("Specular", glm::value_ptr(component.specular));
					

					if (component.type == LightComponent::Type::Spot)
					{
						ImGui::DragFloat("Inner Spot", &component.InnerOuterSpot.x, 0.025f);
						ImGui::DragFloat("Outer Spot", &component.InnerOuterSpot.y, 0.025f);
					}

					if (component.type == LightComponent::Type::Directional)
					{
						ImGui::PushItemWidth(80.f);
						ImGui::Text("Direction");
						ImGui::SameLine(100.f);
						ImGui::DragFloat("X##direction", &component.direction.x, 0.025f, -1, 1);
						ImGui::SameLine();
						ImGui::DragFloat("Y##direction", &component.direction.y, 0.025f, -1, 1);
						ImGui::SameLine();
						ImGui::DragFloat("Z##direction", &component.direction.z, 0.025f, -1, 1);
						ImGui::PopItemWidth();
					}

					if (component.type == LightComponent::Type::Spot || component.type == LightComponent::Type::Point)
					{
						ImGui::DragFloat("Linear", &component.linear, 0.025f);
						ImGui::DragFloat("Quadratic", &component.quadratic, 0.025f);
					}
				}

				/*if (ImGui::CollapsingHeader("Emission", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
				{
					const char* LightAppearanceStr[]{ "Colour", "Filter and Temperature" };
					const char* currentLightAppearanceStr = LightAppearanceStr[(int)component.lightAppearance];
					if (ImGui::BeginCombo("Light Appearance", currentLightAppearanceStr))
					{
						for (int i = 0; i < 2; i++)
						{
							bool isSelected = currentLightAppearanceStr == LightAppearanceStr[i];
							if (ImGui::Selectable(LightAppearanceStr[i], isSelected))
							{
								currentLightAppearanceStr = LightAppearanceStr[i];
								component.lightAppearance = (LightComponent::LightAppearance)(i);
							}
							if (isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					if (component.lightAppearance == LightComponent::LightAppearance::Colour)
					{
						ImGui::ColorEdit4("Colour", glm::value_ptr(component.Colour));
					}
					else
					{
						ImGui::ColorEdit4("Filter", glm::value_ptr(component.Colour));
						ImGui::DragFloat("Temperature", &component.Temperature, 0.025f);
					}

					ImGui::DragFloat("Intensity", &component.Intensity, 0.025f);
					ImGui::DragFloat("Indirect Multiplier", &component.IndirectMultiplier, 0.025f);
					ImGui::DragFloat("Range", &component.Range, 0.025f);
				}*/

				
				/*if (ImGui::CollapsingHeader("Shadows", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
				{
					const char* ShadowStr[]{ "No Shadows", "Soft Shadows", "Hard Shadows" };
					const char* currentShadowStr = ShadowStr[(int)component.shadowType];
					if (ImGui::BeginCombo("Shadow Type", currentShadowStr))
					{
						for (int i = 0; i < 3; i++)
						{
							bool isSelected = currentShadowStr == ShadowStr[i];
							if (ImGui::Selectable(ShadowStr[i], isSelected))
							{
								currentShadowStr = ShadowStr[i];
								component.shadowType = (LightComponent::ShadowType)(i);
							}
							if (isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
				}*/
			});

		DrawComponent<TextComponent>("Text", mSelectedEntity, [](auto& component)
			{
				if (!component.font)
				{
					component.font = Font::GetDefaultFont();
				}

				char inputText[256] = "";
				strncpy_s(inputText, sizeof(inputText), component.text.c_str(), _TRUNCATE);
				int textSize = component.fontSize;

				ImGui::InputText("Text Input", inputText, IM_ARRAYSIZE(inputText));

				ImGui::InputInt("Text Size", &textSize);

				component.text = inputText;
				component.fontSize = textSize;
			});

			DrawComponent<AudioSourceComponent>("Audio Source", mSelectedEntity, [](auto& component)
			{
				ImGui::Button("Audio");

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropAudioItem"))
					{
						AssetHandle data = *(const uint64_t*)payload->Data;
						component.audio = AssetManager::GetAsset<Audio>(data);
					}
					ImGui::EndDragDropTarget();
				}

				if(component.audio)
				{
					bool loop = component.isLoop;
					ImGui::Checkbox("Mute", &component.isMute);
					ImGui::Checkbox("Loop", &component.isLoop);
					if (loop != component.isLoop)
					{
						component.isPlaying = true;
					}

					// DragFloat for volume control (range: -80 dB to 0 dB)
					ImGui::DragFloat("Volume", &component.Volume, 0.5f, -80.0f, 0.0f);
					if (ImGui::Button("Play"))
					{
						component.isPlaying = true;
					}
				}
			});

		DrawComponent<AudioListenerComponent>("Audio Listener", mSelectedEntity, [](auto& component)
			{
				ImGui::Checkbox("Audio Listener", &component.isAudioListener);

			});

		
		DrawComponent<BehaviourTreeComponent>("Behaviour Tree", mSelectedEntity, [](auto& component)
			{
				// Get the window size
				ImVec2 windowSize = ImGui::GetWindowSize();		
				if (ImGui::Button("Open Node Editor", ImVec2(500,0)))
				{
					// Open the node editor
				}
			});
	}
}
