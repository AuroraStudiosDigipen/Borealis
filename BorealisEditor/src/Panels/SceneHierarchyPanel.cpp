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
#include <Scripting/ScriptingUtils.hpp>
#include <Panels/SceneHierarchyPanel.hpp>
#include <Panels/ContentBrowserPanel.hpp>
#include <Physics/PhysicsSystem.hpp>
#include <PrefabManager.hpp>
#include <Prefab.hpp>
#include <PrefabComponent.hpp>
#include <Scene/Serialiser.hpp>

#include <EditorAssets/MeshImporter.hpp>
#include <EditorAssets/FontImporter.hpp>
#include <EditorAssets/AssetImporter.hpp>
#include <EditorAssets/MetaSerializer.hpp>
#include <Assets/AssetManager.hpp>
//#include <Assets/MeshImporter.hpp>
//#include <Assets/FontImporter.hpp>
#include <EditorLayer.hpp>

#include <Core/Project.hpp>
#include <Core/LayerList.hpp>

#include "EditorAssets/MaterialEditor.hpp"
#include <EditorSerialiser.hpp>
#include <AI/BehaviourTree/BTreeFactory.hpp>

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
	static bool DrawVec3Controller(const std::string& label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 10.f)
	{
		bool output = false;
		ImGuiIO& io = ImGui::GetIO();
		ImFont* bold = io.Fonts->Fonts[ImGuiFonts::bold];

		columnWidth *= ImGui::GetFontSize();

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

	std::vector<std::string> SplitString(const std::string& str, char delimiter) {
		std::vector<std::string> tokens;
		std::stringstream ss(str);
		std::string token;
		while (std::getline(ss, token, delimiter)) {
			tokens.push_back(token);
		}
		return tokens;
	}

	static bool DrawProperty(rttr::property& Property, ReflectionInstance& rInstance, bool* propertyDrawn)
	{
		auto propType = Property.get_type();
		auto propName = Property.get_name().to_string();
		auto name = propName;

		if (Property.get_metadata("Dependency").is_valid())
		{
			auto dependencyVariable = Property.get_metadata("Dependency").get_value<std::string>(); //Is Box
			auto dependencyValue = Property.get_metadata("Visible for").get_value<std::string>(); // Box

			auto dependencyValues = SplitString(dependencyValue, ',');

			auto dependencyProperty = rInstance.get_type().get_property(dependencyVariable);
			auto currentValue = dependencyProperty.get_value(rInstance);

			bool isVisible = std::any_of(dependencyValues.begin(), dependencyValues.end(), [&dependencyProperty, &currentValue](const std::string& value) {
				return dependencyProperty.get_enumeration().name_to_value(value) == currentValue;
				});

			if (!isVisible) 
			{
				return false;
			}
		}

		if (Property.get_metadata("Hide").is_valid())
		{
			return false;
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
				ImGui::PushID(Property.get_name().to_string().c_str());
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 10 * ImGui::GetFontSize());
				ImGui::Text(name.c_str());
				ImGui::NextColumn();
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10, 0 }); // Spacing between Items
				*propertyDrawn = true;
				if (ImGui::ColorEdit3(("##" + name + propName).c_str(), glm::value_ptr(Data)))
				{
					Property.set_value(rInstance, Data);
					return true;
				}
			}

			else if (DrawVec3Controller(name, Data, min))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}



		ImGui::PushID(Property.get_name().to_string().c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 10 * ImGui::GetFontSize());
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10, 0 }); // Spacing between Items

		*propertyDrawn = true;

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
			bool isTrue = false;

			if (ImGui::BeginCombo(("##" + name + propName).c_str(), currentEnumString.to_string().c_str()))
			{
				for (int i = 0; i < enumMap.size(); i++)
				{
					auto& [enumName, enumValue] = *std::next(enumMap.begin(), i);
					bool isSelected = currentEnum == enumValue;
					if (ImGui::Selectable(enumName.to_string().c_str(), isSelected))
					{
						Property.set_value(rInstance, enumValue);
						isTrue = true;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			return isTrue;
		}

		if (propType == rttr::type::get<bool>())
		{
			bool Data = Property.get_value(rInstance).to_bool();
			if (ImGui::Checkbox(("##" + name + propName).c_str(), &Data))
			{
				Property.set_value(rInstance, Data);
				// if is tracking
				// PrefabComponent::TrackProperty(Property.get_name().to_string())
				return true;
			}
			return false;
		}
		if (propType == rttr::type::get<float>())
		{
			float Data = Property.get_value(rInstance).to_float();
			if (ImGui::DragFloat(("##" + name + propName).c_str(), &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<int>())
		{
			int Data = Property.get_value(rInstance).to_int();
			if (ImGui::DragInt(("##" + name + propName).c_str(), &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<std::string>())
		{
			std::string Data = Property.get_value(rInstance).to_string();
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), Data.c_str());
			if (ImGui::InputText(("##" + name + propName).c_str(), buffer, sizeof(buffer)))
			{
				Data = std::string(buffer);
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<glm::vec2>())
		{
			rttr::variant value = Property.get_value(rInstance);
			glm::vec2 Data = value.get_value<glm::vec2>();
			if (ImGui::DragFloat2(("##" + name + propName).c_str(), glm::value_ptr(Data)))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<glm::vec4>())
		{
			rttr::variant value = Property.get_value(rInstance);
			glm::vec4 Data = value.get_value<glm::vec4>();

			if (Property.get_metadata("Colour").is_valid())
			{
				if (ImGui::ColorEdit4(("##" + name + propName).c_str(), glm::value_ptr(Data)))
				{
					Property.set_value(rInstance, Data);
					return true;
				}
			}

			else if (ImGui::DragFloat4(("##" + name + propName).c_str(), glm::value_ptr(Data)))
			{
				Property.set_value(rInstance, Data);
				return true;
			}

			return false;
		}

		if (propType == rttr::type::get<unsigned char>())
		{
			unsigned char Data = Property.get_value(rInstance).to_uint8();
			if (ImGui::DragScalar(("##" + name + propName).c_str(), ImGuiDataType_U8, &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<char>())
		{
			char Data = Property.get_value(rInstance).to_int8();
			if (ImGui::DragScalar(("##" + name + propName).c_str(), ImGuiDataType_S8, &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<unsigned short>())
		{
			unsigned short Data = Property.get_value(rInstance).to_uint16();
			if (ImGui::DragScalar(("##" + name + propName).c_str(), ImGuiDataType_U16, &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<short>())
		{
			short Data = Property.get_value(rInstance).to_int16();
			if (ImGui::DragScalar(("##" + name + propName).c_str(), ImGuiDataType_S16, &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<unsigned int>())
		{
			unsigned int Data = Property.get_value(rInstance).to_uint32();
			if (ImGui::DragScalar(("##" + name + propName).c_str(), ImGuiDataType_U32, &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<long long>())
		{
			long long Data = Property.get_value(rInstance).to_int64();
			if (ImGui::DragScalar(("##" + name + propName).c_str(), ImGuiDataType_S64, &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<unsigned long long>())
		{
			unsigned long long Data = Property.get_value(rInstance).to_uint64();
			if (ImGui::DragScalar(("##" + name + propName).c_str(), ImGuiDataType_U64, &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<double>())
		{
			double Data = Property.get_value(rInstance).to_double();
			if (ImGui::DragScalar(("##" + name + propName).c_str(), ImGuiDataType_Double, &Data))
			{
				Property.set_value(rInstance, Data);
				return true;
			}
			return false;
		}

		if (propType == rttr::type::get<Ref<Model>>())
		{

			rttr::variant value = Property.get_value(rInstance);
			Ref<Model> Data = value.get_value<Ref<Model>>();
			if (Data)
			{
				AssetMetaData meta = AssetManager::GetMetaData(Data->mAssetHandle);
				std::string fileName = meta.name;
				ImGui::InputText(("##" + name + propName).c_str(), fileName.data(), fileName.size(), ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
				{
					if (ImGui::MenuItem("Remove Model"))
					{
						Property.set_value(rInstance, Ref<Model>());
					}
					ImGui::EndPopup();
				}
			}
			else
			{
				ImU32 color32 = IM_COL32(180, 120, 120, 255);
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				char buffer[1] = "";
				ImGui::InputText(("##" + name + propName).c_str(), buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopStyleColor();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropMeshItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					rttr::variant setValue(AssetManager::GetAsset<Model>(data));
					Property.set_value(rInstance, setValue);
					return true;
				}
				ImGui::EndDragDropTarget();
			}
			return false;

		}

		if (propType == rttr::type::get<Ref<SkinnedModel>>())
		{

			rttr::variant value = Property.get_value(rInstance);
			Ref<SkinnedModel> Data = value.get_value<Ref<SkinnedModel>>();
			if (Data)
			{
				AssetMetaData meta = AssetManager::GetMetaData(Data->mAssetHandle);
				std::string fileName = meta.name;
				ImGui::InputText(("##" + name + propName).c_str(), fileName.data(), fileName.size(), ImGuiInputTextFlags_ReadOnly);

				if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
				{
					if (ImGui::MenuItem("Remove Model"))
					{
						Property.set_value(rInstance, Ref<SkinnedModel>());
					}
					ImGui::EndPopup();
				}
			}
			else
			{
				ImU32 color32 = IM_COL32(180, 120, 120, 255);
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				char buffer[1] = "";
				ImGui::InputText(("##" + name + propName).c_str(), buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopStyleColor();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropMeshItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					MeshConfig config = GetConfig<MeshConfig>(AssetManager::GetMetaData(data).Config);
					rttr::variant setValue(AssetManager::GetAsset<SkinnedModel>(data));
					if (!config.skinMesh) 
						return false;
					Property.set_value(rInstance, setValue);
						return true;
				}
				ImGui::EndDragDropTarget();
			}
			return false;
		}

		if (propType == rttr::type::get<Ref<Animation>>())
		{

			rttr::variant value = Property.get_value(rInstance);
			Ref<Animation> Data = value.get_value<Ref<Animation>>();
			if (Data)
			{
				AssetMetaData meta = AssetManager::GetMetaData(Data->mAssetHandle);
				std::string fileName = meta.name;
				ImGui::InputText(("##" + name + propName).c_str(), fileName.data(), fileName.size(), ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
				{
					if (ImGui::MenuItem("Remove Animation"))
					{
						Property.set_value(rInstance, Ref<Animation>());
					}
					ImGui::EndPopup();
				}
			}
			else
			{
				ImU32 color32 = IM_COL32(180, 120, 120, 255);
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				char buffer[1] = "";
				ImGui::InputText(("##" + name + propName).c_str(), buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopStyleColor();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropAnimationItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					rttr::variant setValue(AssetManager::GetAsset<Animation>(data));
					Property.set_value(rInstance, setValue);
					return true;
				}
				ImGui::EndDragDropTarget();
			}
			return false;
		}

		if (propType == rttr::type::get<Ref<Material>>())
		{
			rttr::variant value = Property.get_value(rInstance);
			Ref<Material> Data = value.get_value<Ref<Material>>();
			if (Data)
			{
				AssetMetaData meta = AssetManager::GetMetaData(Data->mAssetHandle);
				std::string fileName = meta.name;
				ImGui::InputText(("##" + name + propName).c_str(), fileName.data(), fileName.size(), ImGuiInputTextFlags_ReadOnly);

				if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
				{
					if (ImGui::MenuItem("Remove Material"))
					{
						Property.set_value(rInstance, Ref<Material>());
					}
					ImGui::EndPopup();
				}
			}
			else
			{
				ImU32 color32 = IM_COL32(180, 120, 120, 255);
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				char buffer[1] = "";
				ImGui::InputText(("##" + name + propName).c_str(), buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopStyleColor();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropMaterialItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					rttr::variant setValue(AssetManager::GetAsset<Material>(data));
					Property.set_value(rInstance, setValue);
					return true;
				}
				ImGui::EndDragDropTarget();
			}
			return false;
		}

		if (propType == rttr::type::get<Ref<Texture2D>>())
		{
			rttr::variant value = Property.get_value(rInstance);
			Ref<Texture2D> Data = value.get_value<Ref<Texture2D>>();
			if (Data)
			{
				AssetMetaData meta = AssetManager::GetMetaData(Data->mAssetHandle);
				std::string fileName = meta.name;
				ImGui::InputText(("##" + name + propName).c_str(), fileName.data(), fileName.size(), ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
				{
					if (ImGui::MenuItem("Remove Texture"))
					{
						Property.set_value(rInstance, Ref<Texture2D>());
					}
					ImGui::EndPopup();
				}
			}
			else
			{
				ImU32 color32 = IM_COL32(180, 120, 120, 255);
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				char buffer[1] = "";
				ImGui::InputText(("##" + name + propName).c_str(), buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopStyleColor();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					rttr::variant setValue(AssetManager::GetAsset<Texture2D>(data));
					Property.set_value(rInstance, setValue);
					return true;
				}
				ImGui::EndDragDropTarget();
			}
			return false;
		}

		if (propType == rttr::type::get<Ref<BehaviourTree>>())
		{

			rttr::variant value = Property.get_value(rInstance);
			Ref<BehaviourTree> Data = value.get_value<Ref<BehaviourTree>>();
			if (Data)
			{
				AssetMetaData meta = AssetManager::GetMetaData(Data->mAssetHandle);
				std::string fileName = meta.name;
				ImGui::InputText(("##" + name + propName).c_str(), fileName.data(), fileName.size(), ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
				{
					if (ImGui::MenuItem("Remove Behaviour Tree"))
					{
						Property.set_value(rInstance, Ref<BehaviourTree>());
					}
					ImGui::EndPopup();
				}
			}
			else
			{
				ImU32 color32 = IM_COL32(180, 120, 120, 255);
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				char buffer[1] = "";
				ImGui::InputText(("##" + name + propName).c_str(), buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopStyleColor();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropBehaviourTreeItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					Ref<BehaviourTree> originalTree = AssetManager::GetAsset<BehaviourTree>(data);
					Ref<BehaviourTree> clonedTree = BTreeFactory::Instance().CloneBehaviourTree(originalTree);
					rttr::variant setValue(clonedTree);
					Property.set_value(rInstance, setValue);
					return true;
				}
				ImGui::EndDragDropTarget();
			}
			return false;
		}

		if (propType == rttr::type::get<Ref<Audio>>())
		{
			rttr::variant value = Property.get_value(rInstance);
			Ref<Audio> Data = value.get_value<Ref<Audio>>();
			if (Data)
			{
				AssetMetaData meta = AssetManager::GetMetaData(Data->mAssetHandle);
				std::string fileName = meta.name;
				ImGui::InputText(("##" + name + propName).c_str(), fileName.data(), fileName.size(), ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
				{
					if (ImGui::MenuItem("Remove Audio"))
					{
						Property.set_value(rInstance, Ref<Audio>());
					}
					ImGui::EndPopup();
				}
			}
			else
			{
				ImU32 color32 = IM_COL32(180, 120, 120, 255);
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				char buffer[1] = "";
				ImGui::InputText(("##" + name + propName).c_str(), buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopStyleColor();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropAudioItem"))
				{
					AssetHandle data = *(const uint64_t*)payload->Data;
					rttr::variant setValue(AssetManager::GetAsset<Audio>(data));
					Property.set_value(rInstance, setValue);
					return true;
				}
				ImGui::EndDragDropTarget();
			}
			return false;
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
					bool isPropertyDrawn = false;
					DrawProperty(prop, oldInstance, &isPropertyDrawn);
					if (isPropertyDrawn)
					{
						ImGui::PopID();
						ImGui::Columns(1);
						ImGui::PopStyleVar();
						ImGui::Dummy(ImVec2(0,5));
					}
				}

				Property.set_value(rInstance, oldVariant);
				return true;

			}
		}
		return false;
	}

	template <typename Type>
	static bool DrawComponent(Type& component, Entity& entity)
	{
		bool isEdited = false;
		ReflectionInstance rInstance(component);
		auto properties = rInstance.get_type().get_properties();
		for (auto Property : properties)
		{
			bool isPropertyDrawn = false;
			isEdited = DrawProperty(Property, rInstance, &isPropertyDrawn) ? true : isEdited;
			if (isPropertyDrawn)
			{
				ImGui::PopID();
				ImGui::Columns(1);
				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0,5));
			}
			if (entity.HasComponent<PrefabComponent>() && isEdited)
			{
				auto& prefabComp = entity.GetComponent<PrefabComponent>();
				prefabComp.mEditedComponentList.insert(rInstance.get_type().get_name().to_string() + ":" + Property.get_name().to_string());
			}
		}
		return isEdited;
	}

	template <typename T>
	static bool DrawComponentLayout(const std::string& name, Entity entity, bool allowDelete = true)
	{
		bool isEdited = false;
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
						isEdited = true;
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
				isEdited = DrawComponent(component,entity) ? true : isEdited;
			}

			if (deleteComponent)
			{
				entity.RemoveComponent<T>();
			}
		}
		return isEdited;
	}

	template<> //temp until idk
	static bool DrawComponentLayout<MeshRendererComponent>(const std::string& name, Entity entity, bool allowDelete)
	{
		bool isEdited = false;
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
						isEdited = true;
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
				isEdited = DrawComponent(component, entity) ? true : isEdited;
			}

			if (deleteComponent)
			{
				entity.RemoveComponent<MeshRendererComponent>();
			}

			if (component.Material)
			{
				MaterialEditor::RenderProperties(component.Material);
			}
		}
		return isEdited;
	}

	template<> //temp until idk
	static bool DrawComponentLayout<SkinnedMeshRendererComponent>(const std::string& name, Entity entity, bool allowDelete)
	{
		bool isEdited = false;
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
						isEdited = true;
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
				isEdited = DrawComponent(component, entity) ? true : isEdited;
			}

			if (deleteComponent)
			{
				entity.RemoveComponent<SkinnedMeshRendererComponent>();
			}

			if (component.Material)
			{
				MaterialEditor::RenderProperties(component.Material);
			}
		}
		return isEdited;
	}

	void ShowTextureConfig(AssetMetaData & metaData)
	{
		TextureConfig config = GetConfig<TextureConfig>(metaData.Config);

		const char* textureTypeNames[] = { "Default", "Normal Map" };
		int selectedTextureType = static_cast<int>(config.type);

		ImGui::Text("Texture Configuration");
		if (ImGui::Combo("Texture Type", &selectedTextureType, textureTypeNames, IM_ARRAYSIZE(textureTypeNames))) {
			config.type = static_cast<TextureType>(selectedTextureType);
			metaData.Config = config;
		}

		const char* textureShapeNames[] = { "2D", "Cube Map" };
		int selectedTextureShape = static_cast<int>(config.shape);

		if (ImGui::Combo("Texture Shape", &selectedTextureShape, textureShapeNames, IM_ARRAYSIZE(textureShapeNames))) {
			config.shape = static_cast<TextureShape>(selectedTextureShape);
			metaData.Config = config;
		}

		bool sRGB = config.sRGB;
		if (ImGui::Checkbox("sRGB", &sRGB)) {
			config.sRGB = sRGB;
			metaData.Config = config;
		}

		if (ImGui::Button("Apply"))
		{
			AssetMetaData newData = metaData;
			newData.Config = config;
			MetaFileSerializer::SaveMetaFile(newData);
			AssetImporter::AddToRecompileQueue(newData);
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
								EditorSerialiser serialiser(mContext);
								SceneManager::SaveActiveScene(serialiser);
								SceneManager::SetActiveScene(name, serialiser);
								mContext = SceneManager::GetActiveScene();
								mSelectedEntity = {};

								//Testing load all the prefab children
								//PrefabManager::ClearAllPrefabChildren(); //Might not be needed
								for (auto& item : mContext->mRegistry.view<entt::entity>())
								{

									Entity entity{ item, mContext.get() };
									if (entity.HasComponent<PrefabComponent>()) {
										// Retrieve the PrefabComponent
										auto& prefabComp = entity.GetComponent<PrefabComponent>();

										// Get the parent UUID from the PrefabComponent
										UUID parentUUID = prefabComp.mParentID;

										// Find the associated prefab by its UUID
										auto prefab = PrefabManager::GetPrefab(parentUUID);  // Use existing function GetPrefab
										if (prefab) {
											// Add the entity as a child to the found prefab
											prefab->AddChild(MakeRef<Entity>(entity));
										}
									}
								}
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
				AssetMetaData & metadata = AssetManager::GetMetaData(ContentBrowserPanel::sSelectedAsset);
#ifdef _DEB
				ImGui::Text(("UUID: " + std::to_string(metadata.Handle)).c_str());
#endif
				ImGui::Text(("Name: " + metadata.name).c_str());
				ImGui::Text(("Type: " + AssetManager::AssetTypeToString(metadata.Type)).c_str());
				ImGui::Text(("Path: " + metadata.SourcePath.string()).c_str());
				switch (metadata.Type)
				{
				case AssetType::Texture2D:
				{
					ShowTextureConfig(metadata);
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
				case AssetType::Prefab:
				{
					//When click on prefab, get the UUID, and use the UUID to get the prefab that is in the prefab registry
					//Update the prefab within the prefab registry.
					MaterialEditor::SetMaterial(0);
					Ref<Prefab> selectedPrefab = PrefabManager::GetPrefab(ContentBrowserPanel::sSelectedAsset);
					Entity prefabEntity(selectedPrefab->GetPrefabID(), PrefabManager::GetScenePtr());

					//Deselect the entity
					mSelectedEntity = {};

					//Draw prefabEntity
					if(DrawComponents(prefabEntity))
					{
						selectedPrefab->UpdateAllInstances();
					}
	
					//std::filesystem::path dir = metadata.SourcePath;
					//std::string dirString = dir.string(); // Convert to std::string
					//Serialiser::SerialisePrefab(dirString.c_str(), mSelectedPrefab);

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

		if (!entity.IsActive())
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.2f, 0.2f, 0.4f));

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

		if (!entity.IsActive())
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
				// check if child exists
				auto Entity = mContext->GetEntityByUUID(child);
				if (Entity.IsValid())
				{
					DrawEntityNode(Entity);
				}
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
	static bool SearchBar(std::string& SearchList, Entity& mSelectedEntity, const char* menuText, char* search_buffer)
	{
		bool isEdited = false;
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
						{
							mSelectedEntity.AddComponent<MeshRendererComponent>();
							isEdited = true;
						}
					}
				}
				ImGui::CloseCurrentPopup();
				memset(search_buffer, 0, sizeof(search_buffer));
			}
		return isEdited;
	}

	

	static bool DrawScriptField(Ref<ScriptInstance> component)
	{
		bool isEdited = false;
		for (const auto& name : component->GetScriptClass()->mOrder) // name of script field, script field
		{
			auto& field = component->GetScriptClass()->mFields[name];

			if (field.hasHeader(component->GetMonoClass()))
			{
				ImGui::NewLine();
				std::string text = field.GetHeader(component->GetMonoClass());
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[ImGuiFonts::bold]);
				ImGui::Text(text.c_str());
				ImGui::PopFont();
			}

			if (field.isPrivate() && !field.hasSerializeField(component->GetMonoClass()) || field.hasHideInInspector(component->GetMonoClass()))
			{
				continue;
			}

			if (field.mType == ScriptFieldType::Vector3)
			{
				glm::vec3 Data = component->GetFieldValue<glm::vec3>(name);
				if (DrawVec3Controller(name, Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;
				}

			}

			// Text organisation - split up camelcase
			std::string properName = StringUtils::SplitAndCapitalize(name);



			ImGui::PushID(name.c_str());
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 10 * ImGui::GetFontSize());
			ImGui::Text(properName.c_str());
			ImGui::NextColumn();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10, 0 }); // Spacing between Items

			if (field.isAssetField())
			{
				MonoObject* ObjData = component->GetFieldValue<MonoObject*>(name);
				std::vector<std::string> entityNames;
				std::vector<UUID> entityIDList;
				for (auto entity : SceneManager::GetActiveScene()->GetRegistry().group<TagComponent>())
				{
					entityIDList.push_back(SceneManager::GetActiveScene()->GetRegistry().get<IDComponent>(entity).ID);
					entityNames.push_back(std::string(SceneManager::GetActiveScene()->GetRegistry().get<TagComponent>(entity).Tag));
				}

				std::string currentEntityName = "";
				if (ObjData)
				{
					auto assetID = field.GetGameObjectID(ObjData);
					if (assetID != 0)
					{
						AssetMetaData meta = AssetManager::GetMetaData(assetID);
						std::string fileName = meta.name;
						currentEntityName = fileName;
					}
				}
				else
				{
					// call the constructor
					InitGameObject(ObjData, 0, field.mFieldClassName());
					component->SetFieldValue(name, ObjData);
				}

				int assetType = field.GetAssetType();

				std::string fileName = currentEntityName;
				if (currentEntityName == "")
				{
					ImU32 color32 = IM_COL32(180, 120, 120, 255);
					ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
					ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				}
				ImGui::InputText(("##" + name + field.mName).c_str(), fileName.data(), fileName.size(), ImGuiInputTextFlags_ReadOnly);
				if (currentEntityName == "")
				{
					ImGui::PopStyleColor();
				}

				if (assetType != -1)
				{
					switch (assetType)
					{
					case 0:
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
							{
								UUID data = *(const uint64_t*)payload->Data;
								InitGameObject(ObjData, data, field.mFieldClassName());
								component->SetFieldValue(name, ObjData);
							}
							ImGui::EndDragDropTarget();
						}
						break;
					case 1:
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropMaterialItem"))
							{
								UUID data = *(const uint64_t*)payload->Data;
								InitGameObject(ObjData, data, field.mFieldClassName());
								component->SetFieldValue(name, ObjData);
							}
							ImGui::EndDragDropTarget();
						}
						break;
					case 2:
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropAudioItem"))
							{
								UUID data = *(const uint64_t*)payload->Data;
								InitGameObject(ObjData, data, field.mFieldClassName());
								component->SetFieldValue(name, ObjData);
							}
							ImGui::EndDragDropTarget();
						}
						break;
					default:
						break;
					}
				}

			}


			if (field.isNativeComponent())
			{
				MonoObject* Data = component->GetFieldValue<MonoObject*>(name);
				std::vector<std::string> entityNames;
				std::vector<UUID> entityIDList;

				std::string currentEntityName = "";
				if (Data)
				{
					auto currentEntityID = field.GetGameObjectID(Data);
					if (currentEntityID != 0)
					{
						currentEntityName = SceneManager::GetActiveScene()->GetEntityByUUID(currentEntityID).GetName();
					}
				}
				else
				{
					// call the constructor
					InitGameObject(Data, 0, field.mFieldClassName());
					component->SetFieldValue(name, Data);
				}

				if (currentEntityName == "")
				{
					ImU32 color32 = IM_COL32(180, 120, 120, 255);
					ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
					ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				}
				if (ImGui::BeginCombo(("##" + component->GetKlassName() + name).c_str(), currentEntityName.c_str()))
				{
					int i = 0;
					for (auto ID : entityIDList)
					{
						bool isSelected = currentEntityName == entityNames[i];
						if (ImGui::Selectable(entityNames[i].c_str(), isSelected))
						{
							currentEntityName = entityNames[i];
							UUID entityID = ID;
							InitGameObject(Data, entityID, field.mFieldClassName());
							component->SetFieldValue(name, Data);
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
						i++;
					}
					ImGui::EndCombo();
				}
				if (currentEntityName == "")
				{
					ImGui::PopStyleColor();
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropEntityItem"))
					{
						UUID data = *(const uint64_t*)payload->Data;
						InitGameObject(Data, data, field.mFieldClassName());
						component->SetFieldValue(name, Data);
						// Init game object
					}
					ImGui::EndDragDropTarget();
				}

			}

			if (field.isGameObject())
			{
				MonoObject* Data = component->GetFieldValue<MonoObject*>(name);
				std::vector<std::string> entityNames;
				std::vector<UUID> entityIDList;
				for (auto entity : SceneManager::GetActiveScene()->GetRegistry().group<TagComponent>())
				{
					entityIDList.push_back(SceneManager::GetActiveScene()->GetRegistry().get<IDComponent>(entity).ID);
					entityNames.push_back(std::string(SceneManager::GetActiveScene()->GetRegistry().get<TagComponent>(entity).Tag));
				}

				std::string currentEntityName = "";
				if (Data)
				{
					auto currentEntityID = field.GetGameObjectID(Data);
					if (currentEntityID != 0)
					{
						Entity brEntity = SceneManager::GetActiveScene()->GetEntityByUUID(currentEntityID);
						if (brEntity.IsValid())
						{
							currentEntityName = brEntity.GetComponent<TagComponent>().Tag;
						}
					}
				}
				else
				{
					// call the constructor
					InitGameObject(Data, 0, field.mFieldClassName());
					component->SetFieldValue(name, Data);
				}
				if (currentEntityName == "")
				{
					ImU32 color32 = IM_COL32(180, 120, 120, 255);
					ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
					ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				}
				if (ImGui::BeginCombo(("##" + component->GetKlassName() + name).c_str(), currentEntityName.c_str()))
				{
					int i = 0;
					for (auto ID : entityIDList)
					{
						bool isSelected = currentEntityName == entityNames[i];
						if (ImGui::Selectable(entityNames[i].c_str(), isSelected))
						{
							currentEntityName = entityNames[i];
							UUID entityID = ID;
							InitGameObject(Data, entityID, field.mFieldClassName());
							component->SetFieldValue(name, Data);
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
						i++;
					}
					ImGui::EndCombo();
				}
				if (currentEntityName == "")
				{
					ImGui::PopStyleColor();
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropEntityItem"))
					{
						UUID data = *(const uint64_t*)payload->Data;
						InitGameObject(Data, data, field.mFieldClassName());
						component->SetFieldValue(name, Data);
						// Init game object
					}
					ImGui::EndDragDropTarget();
				}

			}
			if (field.isMonoBehaviour())
			{
				MonoObject* Data = component->GetFieldValue<MonoObject*>(name);

				//List of entities
				auto entityIDList = ScriptingSystem::mEntityScriptMap[field.mFieldClassName()];
				//List of entity names
				std::vector<std::string> entityNames;
				for (auto entity : entityIDList)
				{
					entityNames.push_back(SceneManager::GetActiveScene()->GetEntityByUUID(entity).GetName());
				}

				std::string currentEntityName = "";
				if (Data)
				{
					auto currentEntityID = field.GetAttachedID(Data);
					currentEntityName = SceneManager::GetActiveScene()->GetEntityByUUID(currentEntityID).GetName();
				}

				if (currentEntityName == "")
				{
					ImU32 color32 = IM_COL32(180, 120, 120, 255);
					ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
					ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
				}

				if (ImGui::BeginCombo(("##" + component->GetKlassName() + name).c_str(), currentEntityName.c_str()))
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

				if (currentEntityName == "")
				{
					ImGui::PopStyleColor();
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

			}
			if (field.mType == ScriptFieldType::Bool)
			{
				bool Data = component->GetFieldValue<bool>(name);
				if (ImGui::Checkbox(("##" + component->GetKlassName() + name).c_str(), &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;
				}
			}
			if (field.mType == ScriptFieldType::Float)
			{
				float Data = component->GetFieldValue<float>(name);
				if (ImGui::DragFloat(("##" + component->GetKlassName() + name).c_str(), &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;
				}
			}

			if (field.mType == ScriptFieldType::Int)
			{
				int Data = component->GetFieldValue<int>(name);
				if (ImGui::DragInt(("##" + component->GetKlassName() + name).c_str(), &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;
				}
			}

			if (field.mType == ScriptFieldType::String)
			{
				std::string Data = component->GetFieldValue<std::string>(name);
				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), Data.c_str());
				if (ImGui::InputText(("##" + component->GetKlassName() + name).c_str(), buffer, sizeof(buffer)))
				{
					Data = std::string(buffer);
					component->SetFieldValue(name, &Data);
					isEdited = true;
				}
			}

			if (field.mType == ScriptFieldType::Vector2)
			{
				glm::vec2 Data = component->GetFieldValue<glm::vec2>(name);
				if (ImGui::DragFloat2(("##" + component->GetKlassName() + name).c_str(), glm::value_ptr(Data)))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;
				}
			}

	

			if (field.mType == ScriptFieldType::Vector4)
			{
				glm::vec4 Data = component->GetFieldValue<glm::vec4>(name);
				if (ImGui::DragFloat4(("##" + component->GetKlassName() + name).c_str(), glm::value_ptr(Data)))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;

				}
			}

			if (field.mType == ScriptFieldType::UChar)
			{
				unsigned char Data = component->GetFieldValue<unsigned char>(name);
				if (ImGui::DragScalar(("##" + component->GetKlassName() + name).c_str(), ImGuiDataType_U8, &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;

				}
			}

			if (field.mType == ScriptFieldType::Char)
			{
				char Data = component->GetFieldValue<char>(name);
				if (ImGui::DragScalar(("##" + component->GetKlassName() + name).c_str(), ImGuiDataType_S8, &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;

				}
			}

			if (field.mType == ScriptFieldType::UShort)
			{
				unsigned short Data = component->GetFieldValue<unsigned short>(name);
				if (ImGui::DragScalar(("##" + component->GetKlassName() + name).c_str(), ImGuiDataType_U16, &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;

				}
			}

			if (field.mType == ScriptFieldType::Short)
			{
				short Data = component->GetFieldValue<short>(name);
				if (ImGui::DragScalar(("##" + component->GetKlassName() + name).c_str(), ImGuiDataType_S16, &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;

				}
			}

			if (field.mType == ScriptFieldType::UInt)
			{
				unsigned int Data = component->GetFieldValue<unsigned int>(name);
				if (ImGui::DragScalar(("##" + component->GetKlassName() + name).c_str(), ImGuiDataType_U32, &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;

				}
			}

			if (field.mType == ScriptFieldType::Long)
			{
				long long Data = component->GetFieldValue<long long>(name);
				if (ImGui::DragScalar(("##" + component->GetKlassName() + name).c_str(), ImGuiDataType_S64, &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;

				}
			}

			if (field.mType == ScriptFieldType::ULong)
			{
				unsigned long long Data = component->GetFieldValue<unsigned long long>(name);
				if (ImGui::DragScalar(("##" + component->GetKlassName() + name).c_str(), ImGuiDataType_U64, &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;

				}
			}

			if (field.mType == ScriptFieldType::Double)
			{
				double Data = component->GetFieldValue<double>(name);
				if (ImGui::DragScalar(("##" + component->GetKlassName() + name).c_str(), ImGuiDataType_Double, &Data))
				{
					component->SetFieldValue(name, &Data);
					isEdited = true;

				}
			}

			ImGui::PopID();
			ImGui::Columns(1);
			ImGui::PopStyleVar();
			ImGui::Dummy(ImVec2(0, 5));
		}
		return isEdited;
	}
	static bool DrawScriptComponent(ScriptComponent& component, Entity& entity, bool allowDelete = true)
	{
		bool isEdited = false;
		bool deleteComponent = false;
		std::queue<std::string> deleteQueue;
		for (auto& [name, script] : component.mScripts)
		{
			ImGui::Spacing();
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
				if (ImGui::Button(("+##" + name).c_str(), ImVec2{lineHeight,lineHeight}))
				{
					ImGui::OpenPopup(("ComponentSettingsPopup##" + name).c_str());
				}


				if (ImGui::BeginPopup(("ComponentSettingsPopup##" + name).c_str()))
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						deleteComponent = true;
						deleteQueue.push(name);
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
				ImGui::PushID(name.c_str());
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 10 * ImGui::GetFontSize());
				ImGui::Text("Script");
				ImGui::NextColumn();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10, 0 }); // Spacing between Items
				std::string dataName = name;
				ImGui::InputText(("##" + name).c_str(),dataName.data(), dataName.size(), ImGuiInputTextFlags_ReadOnly);

				ImGui::PopID();
				ImGui::Columns(1);
				ImGui::PopStyleVar();

				isEdited = DrawScriptField(script) ? true : isEdited;
			}

			
		}

		if (deleteComponent)
		{
			while (!deleteQueue.empty())
			{
				ScriptingSystem::mEntityScriptMap[deleteQueue.front()].erase(entity.GetUUID());
				component.RemoveScript(deleteQueue.front());
				deleteQueue.pop();
				if (component.mScripts.empty())
				{
					entity.RemoveComponent<ScriptComponent>();
					return true;
				}
			}
		}

		return isEdited;
	}
	bool SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		bool isEdited = false;
		if (entity.HasComponent<TagComponent>())
		{

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			ImGui::Checkbox("##Active", &entity.GetComponent<TagComponent>().active);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetFontSize() * 10);
			if (ImGui::BeginCombo("##LayerList", "Layers...", ImGuiComboFlags_HeightLarge))
			{
				static char layerTextBuffer[32][32];
				for (int i = 0; i < 32; i++)
				{
					std::string label = LayerList::HasIndex(i) ? LayerList::List().at(i) : "Unused Layer " + std::to_string(i);
					bool isChecked = entity.GetComponent<TagComponent>().mLayer.test(i);

					if (ImGui::Checkbox(("##" + std::to_string(i) + ": " + label).c_str(), &isChecked)) {
						// Update the state of the checkbox
						entity.GetComponent<TagComponent>().mLayer.flip(i);
					}

					std::memset(layerTextBuffer[i], 0, 64);
					std::memcpy(layerTextBuffer[i], label.data(), label.size());

					ImGui::SameLine();
					if (i < 10)
					{
						ImGui::Text((std::to_string(i) + ":   ").c_str());
					}
					else
					{
						ImGui::Text((std::to_string(i) + ": ").c_str());
					}
					ImGui::SameLine();
					ImGuiInputTextFlags_ flag;
					if (i <= 5)
					{
						flag = ImGuiInputTextFlags_ReadOnly;
					}
					else
					{
						flag = ImGuiInputTextFlags_EnterReturnsTrue;
					}
					bool hasPushed = false;
					if (!LayerList::HasIndex(i))
					{
						ImU32 color32 = IM_COL32(180, 120, 120, 255);
						ImVec4 color = ImGui::ColorConvertU32ToFloat4(color32);
						ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
						hasPushed = true;
					}
					ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
					if (ImGui::InputText(("##" + label).c_str(), layerTextBuffer[i], 64, flag))
					{
						LayerList::SetLayer(i, std::string(layerTextBuffer[i]));
					}
					if (hasPushed == true)
					{
						ImGui::PopStyleColor();
					}
					 
					// Right click to open menu
					if (ImGui::BeginPopupContextItem())
					{
						if (LayerList::HasIndex(i) && i > 5)
						{
							if (ImGui::MenuItem("Delete Layer"))
							{
								LayerList::RemoveLayer(i);
							}
						}
						ImGui::EndPopup();
					}
				}
				ImGui::EndCombo();
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

	
			isEdited = SearchBar<SpriteRendererComponent>(search_text, entity, "Sprite Renderer", search_buffer) ? true : isEdited;
			isEdited = SearchBar<CircleRendererComponent>( search_text, entity, "Circle Renderer", search_buffer) ? true : isEdited;
			isEdited = SearchBar<CameraComponent>			(search_text, entity, "Camera", search_buffer) ? true : isEdited;
			isEdited = SearchBar<MeshFilterComponent	  >(search_text, entity,"Mesh Filter", search_buffer) ? true : isEdited;
			isEdited = SearchBar<MeshRendererComponent	  >(search_text, entity,"Mesh Renderer", search_buffer) ? true : isEdited;
			isEdited = SearchBar<BoxColliderComponent	  >(search_text, entity,"Box Collider", search_buffer) ? true : isEdited;
			isEdited = SearchBar<CapsuleColliderComponent>(search_text, entity,"Capsule Collider", search_buffer) ? true : isEdited;
			isEdited = SearchBar<SphereColliderComponent>(search_text, entity, "Sphere Collider", search_buffer) ? true : isEdited;
			isEdited = SearchBar<RigidBodyComponent	  >(search_text, entity,"Rigidbody", search_buffer) ? true : isEdited;
			isEdited = SearchBar<CharacterControlComponent>(search_text, entity, "Character Controller", search_buffer) ? true : isEdited;
			isEdited = SearchBar<LightComponent		  >(search_text, entity,"Light", search_buffer) ? true : isEdited;
			isEdited = SearchBar<TextComponent				>(search_text, entity,"Text", search_buffer) ? true : isEdited;
			isEdited = SearchBar<BehaviourTreeComponent	>(search_text, entity, "Behaviour Tree", search_buffer) ? true : isEdited;
			isEdited = SearchBar<AudioSourceComponent		>(search_text, entity, "Audio Source", search_buffer) ? true : isEdited;
			isEdited = SearchBar<AudioListenerComponent	>(search_text, entity, "Audio Listener", search_buffer) ? true : isEdited;
			isEdited = SearchBar<SkinnedMeshRendererComponent	>(search_text, entity, "Skinned Mesh Renderer", search_buffer) ? true : isEdited;
			isEdited = SearchBar<AnimatorComponent	>(search_text, entity, "Animator", search_buffer) ? true : isEdited;
			isEdited = SearchBar<OutLineComponent	>(search_text, entity, "Outline", search_buffer) ? true : isEdited;
			isEdited = SearchBar<CanvasComponent	>(search_text, entity, "Canvas", search_buffer) ? true : isEdited;
			isEdited = SearchBar<CanvasRendererComponent	>(search_text, entity, "Canvas Renderer", search_buffer) ? true : isEdited;



			// scripts
			for (auto [name, klass] : ScriptingSystem::mScriptClasses)
			{
				std::string MenuConverter(name + " (Script)");
				std::transform(MenuConverter.begin(), MenuConverter.end(), MenuConverter.begin(), ::tolower);

				if (name == "MonoBehaviour") { continue; }
				if (search_text.empty() || MenuConverter.find(search_text) != std::string::npos)
					if (ImGui::MenuItem((name + " (Script)").c_str()))
					{
						if (mSelectedEntity.HasComponent<ScriptComponent>() == false)
						{
							mSelectedEntity.AddComponent<ScriptComponent>();
						}
						auto scriptInstance = MakeRef<ScriptInstance>(klass);
						mSelectedEntity.GetComponent<ScriptComponent>().AddScript(name, scriptInstance);
						scriptInstance->Init(entity.GetUUID());
						ImGui::CloseCurrentPopup();
						memset(search_buffer, 0, sizeof(search_buffer));
						isEdited = true;
					}
			}
			ImGui::EndPopup();
		}
		
		isEdited = DrawComponentLayout<TransformComponent>("Transform Component", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<SpriteRendererComponent>("Sprite Renderer", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<CircleRendererComponent>("Circle Renderer", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<CameraComponent>("Camera", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<MeshFilterComponent>("Mesh Filter", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<MeshRendererComponent>("Mesh Renderer", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<BoxColliderComponent>("Box Collider", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<CapsuleColliderComponent>("Capsule Collider", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<SphereColliderComponent>("Sphere Collider", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<RigidBodyComponent>("Rigidbody", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<CharacterControlComponent>("Character Controller", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<LightComponent>("Light", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<TextComponent>("Text", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<BehaviourTreeComponent>("Behaviour Tree", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<AudioSourceComponent>("Audio Source", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<AudioListenerComponent>("Audio Listener", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<SkinnedMeshRendererComponent>("Skinned Mesh Renderer", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<AnimatorComponent>("Animator", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<OutLineComponent>("Outline", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<CanvasComponent>("Canvas", entity) ? true : isEdited;
		isEdited = DrawComponentLayout<CanvasRendererComponent>("Canvas Renderer", entity) ? true : isEdited;


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



		if (entity.HasComponent<ScriptComponent>())
		{
			isEdited = DrawScriptComponent(entity.GetComponent<ScriptComponent>(), entity) ? true : isEdited;
		}

		return isEdited;
	}

	
}
