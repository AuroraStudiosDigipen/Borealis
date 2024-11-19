#include <imgui.h>
#include "MaterialEditor.hpp"
#include "Assets/AssetManager.hpp"

#include <string>

#include "glm/gtc/type_ptr.hpp"
#include "Graphics/Material.hpp"

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

    void DrawVec2Control(std::string const& label, glm::vec2& values, float min = 0.0f, float max = 1.0f)
    {
        std::string labelStrX = "##Vec2X" + label;
        std::string labelStrY = "##Vec2Y" + label;

        const float width{ 200.f };
        ImGui::PushItemWidth(width);

        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::DragFloat(labelStrX.c_str(), &values.x, 0.1f, min, max, "%.2f");

        ImGui::SameLine();
        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::DragFloat(labelStrY.c_str(), &values.y, 0.1f, min, max, "%.2f");

        ImGui::PopItemWidth();
    }

    void DrawFloatSlider(std::string const& label, float* value, float min = 0.0f, float max = 1.0f)
    {
        std::string labelStr = "##" + label;
        ImGui::SliderFloat(labelStr.c_str(), value, min, max, "%.2f");  // Draw the normal slider
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

    void MaterialEditor::RenderProperties(Ref<Material> const& material)
    {
        if (!material) return;

        /*static char materialName[128];
        strncpy(materialName, material->GetName().c_str(), sizeof(materialName));
        materialName[sizeof(materialName) - 1] = 0;

        if (ImGui::InputText("Material Name", materialName, IM_ARRAYSIZE(materialName))) {
            material->SetName(std::string(materialName));
        }*/

        for (int i = Material::Albedo; i <= Material::Emission; ++i)
        {
            std::string label = Material::TextureMapToString(static_cast<Material::TextureMaps>(i));
            ImGui::Text(label.c_str());

            ImGui::SameLine(100);

            switch (i)
            {
            case Material::Albedo:
            {
                ImGui::Button("Texture");
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                    {
                        //const char* data = (const char*)payload->Data;
                        //std::string imageName = "assets/";
                        //imageName += data;
                        //material->SetTextureMap(Material::Albedo, Texture2D::Create(imageName));

                        AssetHandle data = *(const uint64_t*)payload->Data;
                        material->SetTextureMap(Material::Albedo, AssetManager::GetAsset<Texture2D>(data));
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                glm::vec4 albedoColor = material->GetTextureMapColor()[Material::Albedo];
                if (ImGui::ColorEdit4("##Albedo", glm::value_ptr(albedoColor)))
                {
                    material->SetTextureMapColor(Material::Albedo, albedoColor);
                }

                break;
            }
            case Material::Specular:
            {
                ImGui::Button("Texture");
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                    {
                        //const char* data = (const char*)payload->Data;
                        //std::string imageName = "assets/";
                        //imageName += data;
                        //material->SetTextureMap(Material::Specular, Texture2D::Create(imageName));

                        AssetHandle data = *(const uint64_t*)payload->Data;
                        material->SetTextureMap(Material::Specular, AssetManager::GetAsset<Texture2D>(data));
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                glm::vec4 specularColor = material->GetTextureMapColor()[Material::Specular];
                if (ImGui::ColorEdit4("##Specular", glm::value_ptr(specularColor)))
                {
                    material->SetTextureMapColor(Material::Specular, specularColor);
                }

                break;
            }
            case Material::Metallic:
            {
                ImGui::Button("Texture");
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                    {
                        //const char* data = (const char*)payload->Data;
                        //std::string imageName = "assets/";
                        //imageName += data;
                        //material->SetTextureMap(Material::Metallic, Texture2D::Create(imageName));

                        AssetHandle data = *(const uint64_t*)payload->Data;
                        material->SetTextureMap(Material::Metallic, AssetManager::GetAsset<Texture2D>(data));
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                static float metallicValue = material->GetTextureMapFloats()[Material::Metallic];
                DrawFloatSlider("Metallic", &metallicValue);
                material->SetTextureMapFloat(Material::Metallic, metallicValue);

                break;
            }
            case Material::NormalMap:
            {
                ImGui::Button("Texture");
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                    {
                        //const char* data = (const char*)payload->Data;
                        //std::string imageName = "assets/";
                        //imageName += data;
                        //material->SetTextureMap(Material::NormalMap, Texture2D::Create(imageName));

                        AssetHandle data = *(const uint64_t*)payload->Data;
                        material->SetTextureMap(Material::NormalMap, AssetManager::GetAsset<Texture2D>(data));
                    }
                    ImGui::EndDragDropTarget();
                }
                break;
            }
            case Material::HeightMap:
            {
                ImGui::Button("Texture");
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                    {
                        //const char* data = (const char*)payload->Data;
                        //std::string imageName = "assets/";
                        //imageName += data;
                        //material->SetTextureMap(Material::HeightMap, Texture2D::Create(imageName));

                        AssetHandle data = *(const uint64_t*)payload->Data;
                        material->SetTextureMap(Material::HeightMap, AssetManager::GetAsset<Texture2D>(data));
                    }
                    ImGui::EndDragDropTarget();
                }
                break;
            }
            case Material::Occlusion:
            {
                ImGui::Button("Texture");
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                    {
                        //const char* data = (const char*)payload->Data;
                        //std::string imageName = "assets/";
                        //imageName += data;
                        //material->SetTextureMap(Material::Occlusion, Texture2D::Create(imageName));

                        AssetHandle data = *(const uint64_t*)payload->Data;
                        material->SetTextureMap(Material::Occlusion, AssetManager::GetAsset<Texture2D>(data));
                    }
                    ImGui::EndDragDropTarget();
                }
                break;
            }
            case Material::DetailMask:
            {
                ImGui::Button("Texture");
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                    {
                        //const char* data = (const char*)payload->Data;
                        //std::string imageName = "assets/";
                        //imageName += data;
                        //material->SetTextureMap(Material::DetailMask, Texture2D::Create(imageName));

                        AssetHandle data = *(const uint64_t*)payload->Data;
                        material->SetTextureMap(Material::DetailMask, AssetManager::GetAsset<Texture2D>(data));
                    }
                    ImGui::EndDragDropTarget();
                }
                break;
            }
            case Material::Emission:
            {
                ImGui::Button("Texture");
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropImageItem"))
                    {
                        //const char* data = (const char*)payload->Data;
                        //std::string imageName = "assets/";
                        //imageName += data;
                        //material->SetTextureMap(Material::Emission, Texture2D::Create(imageName));

                        AssetHandle data = *(const uint64_t*)payload->Data;
                        material->SetTextureMap(Material::Emission, AssetManager::GetAsset<Texture2D>(data));
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                glm::vec4 emissionColor = material->GetTextureMapColor()[Material::Emission];
                if (ImGui::ColorEdit4("##Emission", glm::value_ptr(emissionColor)))
                {
                    material->SetTextureMapColor(Material::Emission, emissionColor);
                }

                break;
            }
            default:
                break;
            }

            ImGui::Spacing();
        }

        for (int i = Material::Tiling; i <= Material::Shininess; ++i)
        {
            std::string label = Material::PropsToString(static_cast<Material::Props>(i));
            ImGui::Text(label.c_str());

            ImGui::SameLine(100);

            switch (i)
            {
            case Material::Tiling:
            {
                static glm::vec2 tilingValue = material->GetPropertiesVec2()[Material::Tiling];
                DrawVec2Control("Tiling", tilingValue);
                material->SetPropertyVec2(Material::Tiling, tilingValue);
                break;
            }
            case Material::Offset:
            {
                static glm::vec2 offsetValue = material->GetPropertiesVec2()[Material::Offset];
                DrawVec2Control("Offset", offsetValue);
                material->SetPropertyVec2(Material::Offset, offsetValue);
                break;
            }
            case Material::Smoothness:
            {
                static float smoothnessValue = material->GetPropertiesFloats()[Material::Smoothness];
                DrawFloatSlider("Smoothness", &smoothnessValue);
                material->SetPropertyFloat(Material::Smoothness, smoothnessValue);
                break;
            }
            case Material::Shininess:
            {
                static float shininessValue = material->GetPropertiesFloats()[Material::Shininess];
                DrawFloatSlider("Shininess", &shininessValue, 0.f, 128.f);
                material->SetPropertyFloat(Material::Shininess, shininessValue);
                break;
            }
            default:
                break;
            }

            ImGui::Spacing();
        }

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