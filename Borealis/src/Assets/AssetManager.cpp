/******************************************************************************
/*!
\file       AssetManager.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 27, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Assets/AssetManager.hpp>

#include <AI/BehaviourTree/BTreeFactory.hpp>
#include <Audio/AudioEngine.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/Material.hpp>
#include <Graphics/Animation/Animation.hpp>
#include <Scripting/ScriptingSystem.hpp>

namespace Borealis
{
	void AssetManager::RegisterAllAssetType()
	{
		std::vector<AssetInfo> infos
		{
		//Asset Type					//Asset Name				//extensions			//load function			//Reload function
		{ AssetType::None,				"AssetType::None",			{},						nullptr},
		{ AssetType::Animation,			"AssetType::Animation",		{ ".anim" },			Animation::Load,		Animation::Reload },
		{ AssetType::BehaviourTreeData,	"AssetType::BehaviourTree", { ".btree" },			BTreeFactory::Load },
		{ AssetType::Folder,			"AssetType::Folder",		{},						nullptr},
		{ AssetType::Font,				"AssetType::Font",			{ ".ttf"},				nullptr},
		{ AssetType::Material,			"AssetType::Material",		{ ".mat" },				Material::Load,			Material::Reload },
		{ AssetType::Mesh,				"AssetType::Mesh",			{ ".fbx"},				Model::Load,			Model::Reload },
		{ AssetType::Prefab,			"AssetType::Prefab",		{ ".prefab"},			nullptr},
		{ AssetType::Scene,				"AssetType::Scene",			{ ".sc"},				nullptr},
		{ AssetType::Script,			"AssetType::Script",		{ ".cs"},				nullptr,				ScriptingSystem::Reload},
		{ AssetType::Shader,			"AssetType::Shader",		{ ".glsl"},				nullptr},
		{ AssetType::Texture2D,			"AssetType::Texture2D",		{ ".png"},				Texture2D::Load,		Texture2D::Reload }
		};


		for (AssetInfo const& info : infos)
		{
			RegisterAsset(info);
		}
	}

	void AssetManager::RegisterAsset(AssetInfo const& assetInfo)
	{
		for (std::string const& extension : assetInfo.extensions)
		{
			extensionToAssetType.insert({ extension, assetInfo.type });
		}

		assetTypeToString.insert({ assetInfo.type , assetInfo.name });
		stringToAssetType.insert({ assetInfo.name , assetInfo.type });

		Project::GetEditorAssetsManager()->RegisterAsset(assetInfo.type, assetInfo.loadFunc, assetInfo.reloadFunc);
	}

	AssetType AssetManager::GetAssetTypeFromExtension(std::filesystem::path path)
	{
		if (std::filesystem::is_directory(path))
			return AssetType::Folder;

		std::string extension = path.extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		auto it = extensionToAssetType.find(extension);
		if (it != extensionToAssetType.end()) {
			return it->second;
		}

		return AssetType::None;
	}

	std::string AssetManager::AssetTypeToString(AssetType type)
	{
		auto it = assetTypeToString.find(type);
		if (it != assetTypeToString.end()) {
			return it->second;
		}

		return "AssetType::<Invalid>";
	}

	void AssetManager::VerifyTexture(AssetHandle handle)
{
		AssetMetaData& metaData = Project::GetEditorAssetsManager()->GetMetaData(handle);
		if (metaData.Type == AssetType::Texture2D)
		{
			Texture2D::VerifyTexture(GetAsset<Texture2D>(handle));
		}
	}

	AssetType AssetManager::StringToAssetType(const std::string& type)
	{
		auto it = stringToAssetType.find(type);
		if (it != stringToAssetType.end()) {
			return it->second;
		}

		return AssetType::None;
	}
}

