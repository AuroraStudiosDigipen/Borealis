/******************************************************************************/
/*!
\file		Material.cpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date   	September 18, 2024
\brief		Declares the class for Material for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "BorealisPCH.hpp"
#include "Assets/AssetManager.hpp"
#include "Core/LoggerSystem.hpp"
#include "Graphics/Material.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML
{
    template<>
    struct convert<glm::vec2> {
        static Node encode(const glm::vec2& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs) {
            if (!node.IsSequence() || node.size() != 2) return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Borealis::Material::TextureMaps> {
        static Node encode(const Borealis::Material::TextureMaps& rhs) {
            return Node(static_cast<uint8_t>(rhs));
        }

        static bool decode(const Node& node, Borealis::Material::TextureMaps& rhs) {
            rhs = static_cast<Borealis::Material::TextureMaps>(node.as<uint8_t>());
            return true;
        }
    };

    template<>
    struct convert<Borealis::Material::Props> {
        static Node encode(const Borealis::Material::Props& rhs) {
            return Node(static_cast<uint8_t>(rhs));
        }

        static bool decode(const Node& node, Borealis::Material::Props& rhs) {
            rhs = static_cast<Borealis::Material::Props>(node.as<uint8_t>());
            return true;
        }
    };
}

namespace Borealis
{
    void Material::Init()
    {
        glm::vec4 defaultColor(0.0f, 0.0f, 0.0f, 1.0f);
        for (int i = TextureMaps::Albedo; i <= TextureMaps::Emission; ++i)
        {
            mTextureMapColor[static_cast<TextureMaps>(i)] = defaultColor;
        }

        mPropertiesVec2[Tiling] = { 1.f, 1.f };
        mPropertiesFloat[Smoothness] = 0.01f;
        mPropertiesFloat[Shininess] = 1.f;
    }

    Material::Material(std::filesystem::path path)
    {
        //temp until add to material meta config
        mShader = Shader::GetDefault3DMaterialShader();

        YAML::Node data = YAML::LoadFile(path.string());

        mName = data["Name"].as<std::string>();

        auto textureMaps = data["TextureMaps"];
        for (auto it = textureMaps.begin(); it != textureMaps.end(); ++it) {
            TextureMaps key = StringToTextureMaps(it->first.as<std::string>());
            AssetHandle handle = it->second.as<uint64_t>();
            mTextureMaps[key] = AssetManager::GetAsset<Texture2D>(handle);
        }

        auto textureMapColors = data["TextureMapColors"];
        for (auto it = textureMapColors.begin(); it != textureMapColors.end(); ++it) {
            TextureMaps key = StringToTextureMaps(it->first.as<std::string>());
            glm::vec4 value = it->second.as<glm::vec4>();
            mTextureMapColor[key] = value;
        }

        //Deserialize Texture Map Floats
        auto textureMapFloats = data["TextureMapFloats"];
        for (auto it = textureMapFloats.begin(); it != textureMapFloats.end(); ++it) {
            TextureMaps key = StringToTextureMaps(it->first.as<std::string>());
            float value = it->second.as<float>();
            mTextureMapFloat[key] = value;
        }

        // Deserialize Properties Floats
        auto propertiesFloats = data["PropertiesFloats"];
        for (auto it = propertiesFloats.begin(); it != propertiesFloats.end(); ++it) {
            Props key = StringToProps(it->first.as<std::string>());
            float value = it->second.as<float>();
            mPropertiesFloat[key] = value;
        }

        // Deserialize Properties Vec2
        auto propertiesVec2 = data["PropertiesVec2"];
        for (auto it = propertiesVec2.begin(); it != propertiesVec2.end(); ++it) {
            Props key = StringToProps(it->first.as<std::string>());
            glm::vec2 value = it->second.as<glm::vec2>();
            mPropertiesVec2[key] = value;
        }
    }

    Ref<Material> Material::CreateNewMaterial(std::filesystem::path const& path)
    {
        Material material(Shader::GetDefault3DMaterialShader());
        material.SerializeMaterial(path);
        return MakeRef<Material>(material);
    }

    void Material::SerializeMaterial(std::filesystem::path const& path)
    {
        YAML::Emitter out;

        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << mName;

        // Serialize Texture Maps
        out << YAML::Key << "TextureMaps" << YAML::Value << YAML::BeginMap;
        for (const auto& [key, value] : mTextureMaps) {
            if (value)
                out << YAML::Key << TextureMapToString(key) << YAML::Value << value->mAssetHandle;
        }
        out << YAML::EndMap;

        //Serialize Texture Map Colors
        out << YAML::Key << "TextureMapColors" << YAML::Value << YAML::BeginMap;
        for (const auto& [key, value] : mTextureMapColor) {
            out << YAML::Key << TextureMapToString(key) << YAML::Value << YAML::Flow << YAML::BeginSeq << value.r << value.g << value.b << value.a << YAML::EndSeq;
        }
        out << YAML::EndMap;

        // Serialize Texture Map Floats
        out << YAML::Key << "TextureMapFloats" << YAML::Value << YAML::BeginMap;
        for (const auto& [key, value] : mTextureMapFloat) {
            out << YAML::Key << TextureMapToString(key) << YAML::Value << value;
        }
        out << YAML::EndMap;

        // Serialize Properties Floats
        out << YAML::Key << "PropertiesFloats" << YAML::Value << YAML::BeginMap;
        for (const auto& [key, value] : mPropertiesFloat) {
            out << YAML::Key << PropsToString(key) << YAML::Value << value;
        }
        out << YAML::EndMap;

        out << YAML::Key << "PropertiesVec2" << YAML::Value << YAML::BeginMap;
        for (const auto& [key, value] : mPropertiesVec2) {
            out << YAML::Key << PropsToString(key) << YAML::Value << YAML::Flow << YAML::BeginSeq << value.x << value.y << YAML::EndSeq;
        }
        out << YAML::EndMap;

        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
    }

    void Material::SetUniforms(Ref<Shader> shader/*, int startingTextureUnit*/)
	{
		shader->Bind();
        int textureUnit = 2; //switched to start with 1, for shadow map, think of a more dynamic way in the future

        // Albedo Map
        if (mTextureMaps[Albedo])
        {
            shader->Set("u_Material.hasAlbedoMap", true);
            shader->Set("u_Material.albedoMap", textureUnit);
            shader->Set("u_Material.albedoColor", mTextureMapColor[Albedo]);
            mTextureMaps[Albedo]->Bind(textureUnit);
            textureUnit++;
		}
        else
		{
            shader->Set("u_Material.hasAlbedoMap", false);
            shader->Set("u_Material.albedoColor", mTextureMapColor[Albedo]);
        }

        // Specular Map
        if (mTextureMaps[Specular])
        {
            shader->Set("u_Material.specularMap", textureUnit);
            mTextureMaps[Specular]->Bind(textureUnit);
            shader->Set("u_Material.hasSpecularMap", true);
            textureUnit++;
        }
        else
        {
            shader->Set("u_Material.hasSpecularMap", false);
			shader->Set("u_Material.specularColor", mTextureMapColor[Specular]);
        }

        // Metallic
        if (mTextureMaps[Metallic])
        {
            shader->Set("u_Material.metallicMap", textureUnit);
            mTextureMaps[Metallic]->Bind(textureUnit);
            shader->Set("u_Material.hasMetallicMap", true);
            textureUnit++;
        }
        else
        {
            shader->Set("u_Material.metallic", mTextureMapFloat[Metallic]);
            shader->Set("u_Material.hasMetallicMap", false);
        }

        // Emission Map
        if (mTextureMaps[Emission])
        {
            shader->Set("u_Material.emissionMap", textureUnit);
            mTextureMaps[Emission]->Bind(textureUnit);
            shader->Set("u_Material.hasEmissionMap", true);
            textureUnit++;
        }
        else
        {
            shader->Set("u_Material.hasEmissionMap", false);
            shader->Set("u_Material.emissionColor", mTextureMapColor[Emission]);
        }

        // Normal Map
        if (mTextureMaps[NormalMap])
        {
            shader->Set("u_Material.normalMap", textureUnit);
            mTextureMaps[NormalMap]->Bind(textureUnit);
            shader->Set("u_Material.hasNormalMap", true);
            textureUnit++;
        }

        // Height Map
        if (mTextureMaps[HeightMap])
        {
            shader->Set("u_Material.heightMap", textureUnit);
            mTextureMaps[HeightMap]->Bind(textureUnit);
            textureUnit++;
        }

        // Occlusion
        if (mTextureMaps[Occlusion])
        {
            shader->Set("u_Material.occlusionMap", textureUnit);
            mTextureMaps[Occlusion]->Bind(textureUnit);
            textureUnit++;
        }

        // Detail Mask
        if (mTextureMaps[DetailMask])
        {
            shader->Set("u_Material.detailMaskMap", textureUnit);
            mTextureMaps[DetailMask]->Bind(textureUnit);
            textureUnit++;
        }

        // Set other properties
        shader->Set("u_Material.tiling", mPropertiesVec2[Tiling]);
        shader->Set("u_Material.offset", mPropertiesVec2[Offset]);
        shader->Set("u_Material.smoothness", mPropertiesFloat[Smoothness]);
        shader->Set("u_Material.shininess", mPropertiesFloat[Shininess]);

        shader->Unbind();
	}

    std::string Material::TextureMapToString(TextureMaps map)
    {
        switch (map)
        {
        case Albedo:      return "Albedo";
        case Specular:    return "Specular";
        case Metallic:    return "Metallic";
        case NormalMap:   return "Normal Map";
        case HeightMap:   return "Height Map";
        case Occlusion:   return "Occlusion";
        case DetailMask:  return "Detail Mask";
        case Emission:    return "Emission";
        default:          return "Unknown";
        }
    }

    Material::TextureMaps Material::StringToTextureMaps(const std::string& str)
    {
        static const std::unordered_map<std::string, TextureMaps> map =
        {
            {"Albedo",       Albedo},
            {"Specular",     Specular},
            {"Metallic",     Metallic},
            {"Normal Map",   NormalMap},
            {"Height Map",   HeightMap},
            {"Occlusion",    Occlusion},
            {"Detail Mask",  DetailMask},
            {"Emission",     Emission}
        };

        auto it = map.find(str);
        if (it != map.end()) {
            return it->second;
        }
        BOREALIS_CORE_ASSERT(false, "Invalid string for TextureMaps.");
        return UnknownTextureMap;
    }

    std::string Material::PropsToString(Props prop)
    {
        switch (prop)
        {
        case Tiling:          return "Tiling";
        case Offset:          return "Offset";
        case Smoothness:      return "Smoothness";
        case Shininess:       return "Shininess";
        case HasEmission:     return "Has Emission";
        case HasHeightMap:    return "Has Height Map";
        case HasNormalMap:    return "Has Normal Map";
        case HasOcclusion:    return "Has Occlusion";
        case HasDetailMask:   return "Has Detail Mask";
        default:              return "Unknown";
        }
    }

    Material::Props Material::StringToProps(const std::string& str)
    {
        static const std::unordered_map<std::string, Props> map =
        {
            {"Tiling",          Tiling},
            {"Offset",          Offset},
            {"Smoothness",      Smoothness},
            {"Shininess",       Shininess},
            {"Has Emission",     HasEmission},
            {"Has Height Map",    HasHeightMap},
            {"Has Normal Map",    HasNormalMap},
            {"Has Occlusion",    HasOcclusion},
            {"Has Detail Mask",   HasDetailMask},
        };

        auto it = map.find(str);
        if (it != map.end()) {
            return it->second;
        }
        BOREALIS_CORE_ASSERT(false, "Invalid string for Properties.");
        return UnknownProperty;
    }

    Ref<Asset> Material::Load(std::filesystem::path const& cachePath, AssetMetaData const& assetMetaData)
    {
        Material material(cachePath/std::to_string(assetMetaData.Handle));
        return MakeRef<Material>(material);
    }
}
