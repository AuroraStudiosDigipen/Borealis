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
#include <Scene/ReflectionInstance.hpp>
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
	static bool DrawVec3Controller(const std::string& label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 100.f)
	{
		bool output = false;
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
			output = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			output = true;
		}
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
			output = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();

		if(ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			output = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(ImGui::CalcItemWidth()/3);

		ImGui::PushFont(bold);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.2f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.1f, 0.7f, 1.0f));
		if (ImGui::Button("Z", buttonSize))
		{
			output = true;
			values.z = resetValue;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();

		if(ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			output = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
		return output;
	}

	static void DrawProperty(rttr::property& Property, ReflectionInstance& rInstance)
	{

		auto propType = Property.get_type();
		auto propName = Property.get_name().to_string();
		auto name = propName;

		if (Property.get_metadata("Dependency").is_valid())
		{
			auto dependencyVariable = Property.get_metadata("Dependency").get_value<std::string>(); //Is Box
			auto dependencyValue = Property.get_metadata("Visible for").get_value<std::string>(); // Box
			auto dependencyProperty = rInstance.get_type().get_property(dependencyVariable);
			if (dependencyProperty.get_enumeration().name_to_value(dependencyValue) != dependencyProperty.get_value(rInstance))
			{
				return;
			}
		}

		if (Property.get_metadata("Hide").is_valid())
		{
			return;
		}

		if (Property.is_enumeration())
		{
			auto enumValues = Property.get_enumeration().get_names();
			std::map<rttr::string_view, rttr::variant> enumMap;
			for (auto value : enumValues)
			{
				enumMap[value] = Property.get_enumeration().name_to_value(value);
			}
			auto currentEnum = Property.get_value(rInstance);
			auto currentEnumString = Property.get_enumeration().value_to_name(currentEnum);

			if (ImGui::BeginCombo((name + "##" + propName).c_str(), currentEnumString.to_string().c_str()))
			{
				for (int i = 0; i < enumMap.size(); i++)
				{
					auto& [enumName, enumValue] = *std::next(enumMap.begin(), i);
					bool isSelected = currentEnum == enumValue;
					if (ImGui::Selectable(enumName.to_string().c_str(), isSelected))
					{
						Property.set_value(rInstance, enumValue);
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			return;
		}

		if (propType == rttr::type::get<bool>())
		{
			bool Data = Property.get_value(rInstance).to_bool();
			if (ImGui::Checkbox((name + "##" + propName).c_str(), &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}
		if (propType == rttr::type::get<float>())
		{
			float Data = Property.get_value(rInstance).to_float();
			if (ImGui::DragFloat((name + "##" + propName).c_str(), &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<int>())
		{
			int Data = Property.get_value(rInstance).to_int();
			if (ImGui::DragInt((name + "##" + propName).c_str(), &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<std::string>())
		{
			std::string Data = Property.get_value(rInstance).to_string();
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), Data.c_str());
			if (ImGui::InputText((name + "##" + propName).c_str(), buffer, sizeof(buffer)))
			{
				Data = std::string(buffer);
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<glm::vec2>())
		{
			rttr::variant value = Property.get_value(rInstance);
			glm::vec2 Data = value.get_value<glm::vec2>();
			if (ImGui::DragFloat2((name + "##" + propName).c_str(), glm::value_ptr(Data)))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<glm::vec3>())
		{
			rttr::variant value = Property.get_value(rInstance);
			glm::vec3 Data = value.get_value<glm::vec3>();

			float min = 0;

			if (Property.get_metadata("Min").is_valid())
			{
				min = Property.get_metadata("Min").get_value<float>();
			}

			if (Property.get_metadata("Colour").is_valid())
			{
				if (ImGui::ColorEdit3((name + "##" + propName).c_str(), glm::value_ptr(Data)))
				{
					Property.set_value(rInstance, Data);
				}
			}

			else if (DrawVec3Controller(name, Data, min))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<glm::vec4>())
		{
			rttr::variant value = Property.get_value(rInstance);
			glm::vec4 Data = value.get_value<glm::vec4>();

			if (Property.get_metadata("Colour").is_valid())
			{
				if (ImGui::ColorEdit4((name + "##" + propName).c_str(), glm::value_ptr(Data)))
				{
					Property.set_value(rInstance, Data);
				}
			}

			else if (ImGui::DragFloat4((name + "##" + propName).c_str(), glm::value_ptr(Data)))
			{
				Property.set_value(rInstance, Data);
			}

			return;
		}

		if (propType == rttr::type::get<unsigned char>())
		{
			unsigned char Data = Property.get_value(rInstance).to_uint8();
			if (ImGui::DragScalar((name + "##" + propName).c_str(), ImGuiDataType_U8, &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<char>())
		{
			char Data = Property.get_value(rInstance).to_int8();
			if (ImGui::DragScalar((name + "##" + propName).c_str(), ImGuiDataType_S8, &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<unsigned short>())
		{
			unsigned short Data = Property.get_value(rInstance).to_uint16();
			if (ImGui::DragScalar((name + "##" + propName).c_str(), ImGuiDataType_U16, &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<short>())
		{
			short Data = Property.get_value(rInstance).to_int16();
			if (ImGui::DragScalar((name + "##" + propName).c_str(), ImGuiDataType_S16, &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<unsigned int>())
		{
			unsigned int Data = Property.get_value(rInstance).to_uint32();
			if (ImGui::DragScalar((name + "##" + propName).c_str(), ImGuiDataType_U32, &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<long long>())
		{
			long long Data = Property.get_value(rInstance).to_int64();
			if (ImGui::DragScalar((name + "##" + propName).c_str(), ImGuiDataType_S64, &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<unsigned long long>())
		{
			unsigned long long Data = Property.get_value(rInstance).to_uint64();
			if (ImGui::DragScalar((name + "##" + propName).c_str(), ImGuiDataType_U64, &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<double>())
		{
			double Data = Property.get_value(rInstance).to_double();
			if (ImGui::DragScalar((name + "##" + propName).c_str(), ImGuiDataType_Double, &Data))
			{
				Property.set_value(rInstance, Data);
			}
			return;
		}

		if (propType == rttr::type::get<Ref<Model>>())
		{
			ImGui::Button(propName.c_str());
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropMeshItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					rttr::variant value(AssetManager::GetAsset<Model>(data));
					Property.set_value(rInstance, value);
				}
				ImGui::EndDragDropTarget();
			}
		}

		if (propType == rttr::type::get<Ref<SkinnedModel>>())
		{
			ImGui::Button(propName.c_str());
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropMeshItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					MeshConfig config = GetConfig<MeshConfig>(AssetManager::GetMetaData(data).Config);
					if (!config.skinMesh) return;
					rttr::variant value(AssetManager::GetAsset<SkinnedModel>(data));
					Property.set_value(rInstance, value);
				}
				ImGui::EndDragDropTarget();
			}
		}

		if (propType == rttr::type::get<Ref<Animation>>())
		{
			ImGui::Button(propName.c_str());
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropAnimationItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;

					rttr::variant value(AssetManager::GetAsset<Animation>(data));
					Property.set_value(rInstance, value);
				}
				ImGui::EndDragDropTarget();
			}
		}

		if (propType == rttr::type::get<Ref<Material>>())
		{
			ImGui::Button(propName.c_str());
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropMaterialItem"))
				{

					AssetHandle data = *(const uint64_t*)payload->Data;
					rttr::variant value(AssetManager::GetAsset<Material>(data));
					Property.set_value(rInstance, value);
				}
				ImGui::EndDragDropTarget();
			}
			return;
		}

		if (propType == rttr::type::get<Ref<Texture2D>>())
		{
			ImGui::Button(propName.c_str());
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					rttr::variant value(AssetManager::GetAsset<Texture2D>(data));
					Property.set_value(rInstance, value);
				}
				ImGui::EndDragDropTarget();
			}
			return;
		}

		if (propType == rttr::type::get<Ref<Audio>>())
		{
			ImGui::Button(propName.c_str());
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropAudioItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					rttr::variant value(AssetManager::GetAsset<Audio>(data));
					Property.set_value(rInstance, value);
				}
				ImGui::EndDragDropTarget();
			}
			return;
		}

		if (propType.is_valid() && propType.is_class()) // any other type that is registered but not in above "basic types"
		{
			
			auto properties = propType.get_properties();
			rttr::variant oldVariant = Property.get_value(rInstance);
			rttr::instance oldInstance(oldVariant);
			if (oldInstance.is_valid())
			{
				for (auto prop : properties)
				{
					DrawProperty(prop, oldInstance);
				}

				Property.set_value(rInstance, oldVariant);

			}
		}

	}

	template <typename Type>
	static void DrawComponent(Type& component)
	{
		ReflectionInstance rInstance(component);
		auto properties = rInstance.get_type().get_properties();
		for (auto Property : properties)
		{
			DrawProperty(Property, rInstance);
		}
	}

	template <typename T>
	static void DrawComponentLayout(const std::string& name, Entity entity, bool allowDelete = true)
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
				DrawComponent(component);
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

	template<> //temp until idk
	static void DrawComponentLayout<MeshRendererComponent>(const std::string& name, Entity entity, bool allowDelete)
	{
		if (entity.HasComponent<MeshRendererComponent>())
		{
			ImGui::Spacing();
			auto& component = entity.GetComponent<MeshRendererComponent>();

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
				if (ImGui::Button(("+##" + name).c_str(), ImVec2{ lineHeight,lineHeight }))
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
				DrawComponent(component);
			}

			if (deleteComponent)
			{
				if (typeid(MeshRendererComponent) == typeid(RigidBodyComponent))
				{
					PhysicsSystem::FreeRigidBody(entity.GetComponent<RigidBodyComponent>());
				}
				entity.RemoveComponent<MeshRendererComponent>();
			}

			if (component.Material)
			{
				MaterialEditor::RenderProperties(component.Material);
			}
		}
	}

	template<> //temp until idk
	static void DrawComponentLayout<SkinnedMeshRendererComponent>(const std::string& name, Entity entity, bool allowDelete)
	{
		if (entity.HasComponent<SkinnedMeshRendererComponent>())
		{
			ImGui::Spacing();
			auto& component = entity.GetComponent<SkinnedMeshRendererComponent>();

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
				if (ImGui::Button(("+##" + name).c_str(), ImVec2{ lineHeight,lineHeight }))
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
				DrawComponent(component);
			}

			if (deleteComponent)
			{
				if (typeid(SkinnedMeshRendererComponent) == typeid(RigidBodyComponent))
				{
					PhysicsSystem::FreeRigidBody(entity.GetComponent<RigidBodyComponent>());
				}
				entity.RemoveComponent<SkinnedMeshRendererComponent>();
			}

			if (component.Material)
			{
				MaterialEditor::RenderProperties(component.Material);
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
						auto transform = entity.GetComponent<TransformComponent>();
						if (transform.ParentID == 0)
						{
							DrawEntityNode(entity);
						}
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
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
		uint64_t entityID = static_cast<uint64_t>((uint32_t)entity);
		if (entity.HasComponent<PrefabComponent>())
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.16f, 0.34f, 0.63f, 1.f));

		bool opened = ImGui::TreeNodeEx((void*)entityID, flags, tag.c_str());
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropEntityItem");
			if (payload) {
				UUID data = *(const uint64_t*)payload->Data;
				Entity childEntity = SceneManager::GetActiveScene()->GetEntityByUUID(data);
				TransformComponent::ResetParent(childEntity);
				TransformComponent::SetParent(childEntity, entity);
			}
			ImGui::EndDragDropTarget();
		}

		if (entity.HasComponent<PrefabComponent>())
			ImGui::PopStyleColor();

		//Dragging of items for creation of prefab
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("DragDropEntityItem", (const void*)&entity.GetUUID(), sizeof(UUID));
			
			ImGui::Text("%s", tag.c_str()); // Display the entity tag as the payload text				
			ImGui::EndDragDropSource();
		}

		if (!ImGui::IsMouseDragging(0) && ImGui::IsMouseReleased(0))
		{
			if (ImGui::IsItemHovered())
			{
				mSelectedEntity = entity;
			}
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
			if (ImGui::MenuItem("Unparent Entity"))
			{
				TransformComponent::ResetParent(mSelectedEntity);
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			auto transform = entity.GetComponent<TransformComponent>();
			for (auto child : transform.ChildrenID)
			{
				DrawEntityNode(mContext->GetEntityByUUID(child));
			}
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

	

	static void DrawScriptField(Ref<ScriptInstance> component, Ref<Scene> context)
	{

		for (const auto& [name, field] : component->GetScriptClass()->mFields) // name of script field, script field
		{
			if (field.isMonoBehaviour())
			{
				MonoObject* Data = component->GetFieldValue<MonoObject*>(name);

				//List of entities
				auto entityIDList = ScriptingSystem::mEntityScriptMap[field.mFieldClassName()];
				//List of entity names
				std::vector<std::string> entityNames;
				for (auto entity : entityIDList)
				{
					entityNames.push_back(context->GetEntityByUUID(entity).GetName());
				}

				std::string currentEntityName = "";
				if (Data)
				{
					auto currentEntityID = field.GetAttachedID(Data);
					currentEntityName = SceneManager::GetActiveScene()->GetEntityByUUID(currentEntityID).GetName();
				}

				if (ImGui::BeginCombo(name.c_str(), currentEntityName.c_str()))
				{
					int i = 0;
					for (auto ID : entityIDList)
					{
						bool isSelected = currentEntityName == entityNames[i];
						if (ImGui::Selectable(entityNames[i].c_str(), isSelected))
						{
							currentEntityName = entityNames[i];
							UUID entityID = ID;
							Entity entity = SceneManager::GetActiveScene()->GetEntityByUUID(entityID);
							auto& scriptComponent = entity.GetComponent<ScriptComponent>();
							auto script = scriptComponent.mScripts.find(field.mFieldClassName());
							component->SetFieldValue(name, script->second->GetInstance());
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
						i++;
					}
					ImGui::EndCombo();
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropEntityItem"))
					{
						UUID data = *(const uint64_t*)payload->Data;
						Entity entity = SceneManager::GetActiveScene()->GetEntityByUUID(data);
						auto& scriptComponent = entity.GetComponent<ScriptComponent>();
						auto script = scriptComponent.mScripts.find(field.mFieldClassName());
						component->SetFieldValue(name, script->second->GetInstance());
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::Text(name.c_str());
			}
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
				if(DrawVec3Controller(name, Data))
					component->SetFieldValue(name, &Data);
				
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
	static void DrawScriptComponent(ScriptComponent& component, Entity& entity, Ref<Scene> context, bool allowDelete = true)
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

				bool Data = ScriptingSystem::GetEnabled(script);
				if (ImGui::Checkbox((std::string("##") + name + "enabled").c_str(), &Data))
				{
					ScriptingSystem::SetEnabled(script, Data);
				}

				ImGui::SameLine();
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
				DrawScriptField(script, context);
			}

			if (deleteComponent)
			{
				ScriptingSystem::mEntityScriptMap[name].erase(entity.GetUUID());
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
			ImGui::Checkbox("##Active", &entity.GetComponent<TagComponent>().active);
			ImGui::SameLine();
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}

			ImGui::SameLine();

			
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
			SearchBar<SkinnedMeshRendererComponent	  >(search_text, mSelectedEntity, "Skinned Mesh Renderer", search_buffer);
			SearchBar<AnimatorComponent	  >(search_text, mSelectedEntity, "Animator", search_buffer);
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
						ScriptingSystem::mEntityScriptMap[name].insert(mSelectedEntity.GetUUID());
						ImGui::CloseCurrentPopup();
						memset(search_buffer, 0, sizeof(search_buffer));
					}
			}
				
				


			ImGui::EndPopup();
		}
		
		DrawComponentLayout<TransformComponent>("Transform Component", mSelectedEntity);
		DrawComponentLayout<SpriteRendererComponent>("Sprite Renderer", mSelectedEntity);
		DrawComponentLayout<CircleRendererComponent>("Circle Renderer", mSelectedEntity);
		DrawComponentLayout<CameraComponent>("Camera", mSelectedEntity);
		DrawComponentLayout<MeshFilterComponent>("Mesh Filter", mSelectedEntity);
		DrawComponentLayout<MeshRendererComponent>("Mesh Renderer", mSelectedEntity);
		DrawComponentLayout<SkinnedMeshRendererComponent>("Skinned Mesh Renderer", mSelectedEntity);
		DrawComponentLayout<AnimatorComponent>("Animator", mSelectedEntity);
		DrawComponentLayout<BoxColliderComponent>("Box Collider", mSelectedEntity);
		DrawComponentLayout<CapsuleColliderComponent>("Capsule Collider", mSelectedEntity);
		DrawComponentLayout<RigidBodyComponent>("Rigidbody", mSelectedEntity);
		DrawComponentLayout<LightComponent>("Light", mSelectedEntity);
		DrawComponentLayout<TextComponent>("Text", mSelectedEntity);
		DrawComponentLayout<BehaviourTreeComponent>("Behaviour Tree", mSelectedEntity);
		DrawComponentLayout<AudioSourceComponent>("Audio Source", mSelectedEntity);
		DrawComponentLayout<AudioListenerComponent>("Audio Listener", mSelectedEntity);


		/*DrawComponent<CameraComponent>("Camera", mSelectedEntity, [](auto& cameraComponent)
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
			});*/



		if (mSelectedEntity.HasComponent<ScriptComponent>())
		{
			DrawScriptComponent(mSelectedEntity.GetComponent<ScriptComponent>(), mSelectedEntity, mContext);
		}


	}

	
}
