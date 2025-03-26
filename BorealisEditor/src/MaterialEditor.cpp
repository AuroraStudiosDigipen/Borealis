#include <imgui.h>
#include "MaterialEditor.hpp"
#include "Assets/AssetManager.hpp"

#include <string>

#include "glm/gtc/type_ptr.hpp"
#include "Graphics/Material.hpp"
#include <Commands.hpp>
namespace Borealis
{
    AssetHandle MaterialEditor::mMaterialHandle{};
	Ref<Material> MaterialEditor::mMaterial = nullptr;

    void DrawVec4Control(std::string const& label, glm::vec4& values, float min = 0.0f, float max = 1.0f)
    {
        std::string labelStrX = "##Vec4X" + label;
        std::string labelStrY = "##Vec4Y" + label;
        std::string labelStrZ = "##Vec4Z" + label;
        std::string labelStrW = "##Vec4W" + label;

        const float width{ 60.f };
        ImGui::PushItemWidth(width);

        ImGui::SameLine();
        ImGui::Text("R");
        ImGui::SameLine();
        ImGui::DragFloat(labelStrX.c_str(), &values.x, 0.01f, min, max, "%.2f");

        ImGui::SameLine();
        ImGui::Text("G");
        ImGui::SameLine();
        ImGui::DragFloat(labelStrY.c_str(), &values.y, 0.01f, min, max, "%.2f");

        ImGui::SameLine();
        ImGui::Text("B");
        ImGui::SameLine();
        ImGui::DragFloat(labelStrZ.c_str(), &values.z, 0.01f, min, max, "%.2f");

        ImGui::SameLine();
        ImGui::Text("A");
        ImGui::SameLine();
        ImGui::DragFloat(labelStrW.c_str(), &values.w, 0.01f, min, max, "%.2f");

        ImGui::PopItemWidth();
    }

    bool DrawVec2Control(std::string const& label, glm::vec2& values, float min = 0.0f, float max = 100.0f)
    {
        bool isModified = false;
        std::string labelStrX = "##Vec2X" + label;
        std::string labelStrY = "##Vec2Y" + label;

        const float width{ 200.f };
        ImGui::PushItemWidth(width);

        ImGui::Text("X");
        ImGui::SameLine();
        if (ImGui::DragFloat(labelStrX.c_str(), &values.x, 0.1f, min, max, "%.2f"))
            isModified = true;

        ImGui::SameLine();
        ImGui::Text("Y");
        ImGui::SameLine();
        if (ImGui::DragFloat(labelStrY.c_str(), &values.y, 0.1f, min, max, "%.2f"))
            isModified = true;

        ImGui::PopItemWidth();

        return isModified;
    }

    bool DrawFloatSlider(std::string const& label, float* value, float min = 0.0f, float max = 1.0f)
    {
        std::string labelStr = "##" + label;
        return ImGui::SliderFloat(labelStr.c_str(), value, min, max, "%.2f");  // Draw the normal slider
    }

	void MaterialEditor::RenderEditor()
	{
        if (!mMaterial) return;

        static char materialName[128] = "New Material";
        ImGui::InputText("Material Name", materialName, IM_ARRAYSIZE(materialName));

        //static Ref<Material> material = MakeRef<Material>(Shader::Create("assets/shaders/Renderer3D_Material.glsl"));

    	ImGui::Separator();

    	RenderProperties(mMaterial);

        ImGui::Separator();

		if (ImGui::Button("Save Material"))
		{
            std::filesystem::path path = AssetManager::GetMetaData(mMaterial->mAssetHandle).SourcePath;
            mMaterial->SerializeMaterial(path);
		}
	}

    static const std::map<Material::TextureMaps, int> colorMaps =
    {
        {Material::Albedo, 0},
        {Material::Specular, 1},
        {Material::Emission, 2}
    };

    static const std::map<Material::TextureMaps, int> floatMaps =
    {
        {Material::Metallic, 0}
    };

    void MaterialEditor::RenderProperties(Ref<Material> const& material)
    {
        if (!material) return;

        /*static char materialName[128];
        strncpy(materialName, material->GetName().c_str(), sizeof(materialName));
        materialName[sizeof(materialName) - 1] = 0;

        if (ImGui::InputText("Material Name", materialName, IM_ARRAYSIZE(materialName))) {
            material->SetName(std::string(materialName));
        }*/

        bool isModified = false;

        bool transparency = material->isTransparent;
        auto old = transparency;
        if (ImGui::Checkbox("Is Transparent", &transparency))
        {
            ActionManager::execute(std::make_unique<ModifyMaterialTransparent>(material, old, transparency));
        }

        for (int i = Material::Albedo; i <= Material::Emission; ++i)
        {
            std::string label = Material::TextureMapToString(static_cast<Material::TextureMaps>(i));
            ImGui::Text(label.c_str());

            ImGui::SameLine(125);
            auto matMap = (Material::TextureMaps)i;


            auto TextureMaps = material->GetTextureMaps();
            if (TextureMaps.contains(matMap))
            {
                auto mapData = TextureMaps[matMap];
                ImGui::InputText("##Texture", AssetManager::GetMetaData(mapData->mAssetHandle).name.data(), AssetManager::GetMetaData(mapData->mAssetHandle).name.size(), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
                {
                    if (ImGui::MenuItem("Remove Texture"))
                    {
                        ActionManager::execute(std::make_unique<EraseMaterialTextureMap>(material, matMap));
						isModified = true;
                    }
                    ImGui::EndPopup();
                }
            }
            else
            {
                ImGui::InputText("##Texture", (char[])"", 1, ImGuiInputTextFlags_ReadOnly);
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                {
                    AssetHandle data = *(const uint64_t*)payload->Data;
                    ActionManager::execute(std::make_unique<ModifyMaterialTextureMap>(material, (Material::TextureMaps)i, material->GetTextureMaps()[matMap], AssetManager::GetAsset<Texture2D>(data)));
                    isModified = true;
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::NewLine();
            ImGui::SameLine(125);

            if (colorMaps.contains(matMap))
            {
                if (matMap == Material::Emission)
                {
                    glm::vec4 albedoColor = material->GetTextureMapColor()[matMap];
                    auto oldData = albedoColor;
                    if (ImGui::ColorEdit4(("##" + label).c_str(), glm::value_ptr(albedoColor), ImGuiColorEditFlags_NoAlpha))
                    {
                        isModified = true;
                    }
                    ImGui::Text("Intensity");
                    ImGui::SameLine(125);
                    if (DrawFloatSlider(("##" + std::string("Emission Intensity")).c_str(), &albedoColor.a,0.f,100.f))
                    {
                        isModified = true;
                    }

                    if (isModified)
                    {
                        ActionManager::execute(std::make_unique<ModifyMaterialTextureMapColor>(material, matMap, oldData, albedoColor));
                    }
                }
                else
                {

                    glm::vec4 albedoColor = material->GetTextureMapColor()[matMap];
                    auto oldData = albedoColor;
                    if (ImGui::ColorEdit4(("##" + label).c_str(), glm::value_ptr(albedoColor)))
                    {
                        ActionManager::execute(std::make_unique<ModifyMaterialTextureMapColor>(material, matMap, oldData, albedoColor));
                        isModified = true;
                    }
                }
            }
            else if(floatMaps.contains(matMap))
            {
                float floatValue = material->GetTextureMapFloats()[matMap];
                auto oldValue = floatValue;
                if (DrawFloatSlider(("##" + label).c_str(), &floatValue))
                {
                    ActionManager::execute(std::make_unique<ModifyMaterialTextureMapFloat>(material, matMap, oldValue, floatValue));
                    isModified = true;
                }
            }
           

            ImGui::Spacing();
            ImGui::Separator();
        }

        for (int i = Material::Tiling; i <= Material::Sharpness; ++i)
        {
            std::string label = Material::PropsToString(static_cast<Material::Props>(i));
            ImGui::Text(label.c_str());

            ImGui::SameLine(100);

            switch (i)
            {
            case Material::Tiling:
            {
                glm::vec2 tilingValue = material->GetPropertiesVec2()[Material::Tiling];
                auto oldValue = tilingValue;
                if(DrawVec2Control("Tiling", tilingValue))
                {
                    ActionManager::execute(std::make_unique<ModifyMaterialPropertyVec2Command>(material, Material::Tiling, oldValue, tilingValue));
                    isModified = true;
                }
                break;
            }
            case Material::Offset:
            {
                glm::vec2 offsetValue = material->GetPropertiesVec2()[Material::Offset];
                auto oldValue = offsetValue;
                if(DrawVec2Control("Offset", offsetValue))
                {
                    ActionManager::execute(std::make_unique<ModifyMaterialPropertyVec2Command>(material, Material::Offset, oldValue, offsetValue));
                    isModified = true;
                }
                break;
            }
            case Material::Smoothness:
            {
                float smoothnessValue = material->GetPropertiesFloats()[Material::Smoothness];
                auto oldValue = smoothnessValue;
                if(DrawFloatSlider("Smoothness", &smoothnessValue))
                {
                    ActionManager::execute(std::make_unique<ModifyMaterialPropertyFloatCommand>(material, Material::Smoothness, oldValue, smoothnessValue));
                    isModified = true;
                }
                break;
            }
            case Material::Shininess:
            {
                float shininessValue = material->GetPropertiesFloats()[Material::Shininess];
                auto oldValue = shininessValue;
                if(DrawFloatSlider("Shininess", &shininessValue, 0.f, 128.f))
                {
                    ActionManager::execute(std::make_unique<ModifyMaterialPropertyFloatCommand>(material, Material::Shininess, oldValue, shininessValue));
                    isModified = true;
                }
                break;
            }
            case Material::HexSize:
            {
                float hexSize = material->GetPropertiesFloats()[Material::HexSize];
                auto oldValue = hexSize;
                if (DrawFloatSlider("Hex Size", &hexSize, 0.f, 128.f))
                {
                    ActionManager::execute(std::make_unique<ModifyMaterialPropertyFloatCommand>(material, Material::HexSize, oldValue, hexSize));
                    isModified = true;
                }
                break;
            }
            case Material::Sharpness:
            {
                float sharpness = material->GetPropertiesFloats()[Material::Sharpness];
                auto oldValue = sharpness;
                if (DrawFloatSlider("Sharpness", &sharpness, 0.f, 128.f))
                {
                    ActionManager::execute(std::make_unique<ModifyMaterialPropertyFloatCommand>(material, Material::Sharpness, oldValue, sharpness));
                    isModified = true;
                }
                break;
            }
            default:
                break;
            }

            ImGui::Spacing();
        }

        bool checkMark = material->mNonRepeatingTiles;
        auto oldVal = checkMark;
        if (ImGui::Checkbox("Non Repeating Tiles", &checkMark))
        {
            ActionManager::execute(std::make_unique<ModifyMaterialRepeatingTiles>(material, oldVal, checkMark));
            isModified = true;
        }

        material->isModified = isModified;

        ImGui::Separator();
    }

    void MaterialEditor::SetMaterial(AssetHandle materialHandle)
    {
        if (materialHandle != mMaterialHandle)
        {
            if (materialHandle == 0)
            {
                mMaterial = nullptr;
                mMaterialHandle = 0;
                return;
            }
            mMaterial = AssetManager::GetAsset<Material>(materialHandle);
            mMaterialHandle = materialHandle;
        }
    }
}