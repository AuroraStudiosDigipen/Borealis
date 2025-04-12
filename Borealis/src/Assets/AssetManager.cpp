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
#include <map>
namespace Borealis
{
	struct AssetEntry
	{
		uint64_t id;
		uint64_t offset;
		uint32_t size;
	};

	struct AssetEntryValue
	{
		uint64_t offset;
		uint32_t size;
	};

	void AssetManager::BuildPak(std::filesystem::path folderPath, std::filesystem::path location)
	{
		
		std::vector<AssetEntry> entries;
		uint64_t offset = 0;
		std::ofstream out(location, std::ios::binary);

		for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
			if (!entry.is_regular_file()) continue;

			try
			{
				uint64_t hashId = std::stoull(entry.path().filename().string()); // assumes filename is hex
	
				std::ifstream in(entry.path(), std::ios::binary | std::ios::ate);
				if (!in) continue;

				uint32_t size = static_cast<uint32_t>(in.tellg());
				in.seekg(0);
				std::vector<char> buffer(size);
				in.read(buffer.data(), size);
				out.write(buffer.data(), size);

				entries.push_back({ hashId, offset, size });
				offset += size;
			}
			catch (const std::invalid_argument& e)
			{
				continue; // Skip this file if the filename is not a valid hex string
			}
			catch (const std::out_of_range& e)
			{
				continue; // Skip this file if the filename is out of range
			}
		}

		uint32_t count = entries.size();
		out.write((char*)&count, sizeof(count));
		out.write((char*)entries.data(), sizeof(AssetEntry)* entries.size());
		out.close();
	}
	std::map<uint64_t, AssetEntryValue> entryMap;

	void AssetManager::ReadPak(std::filesystem::path folderPath)
	{
		std::vector<AssetEntry> entries;
		std::ifstream in(folderPath, std::ios::binary);
		if (!in) return;
		uint64_t fileSize = in.tellg();
		in.seekg(fileSize -sizeof(uint32_t), std::ios::end);
		uint32_t count;
		in.read(reinterpret_cast<char*>(&count), sizeof(count));

		// Read asset entries into memor
		in.seekg(fileSize - sizeof(uint32_t) - count * sizeof(AssetEntry));
		entries.resize(count);
		in.read(reinterpret_cast<char*>(entries.data()), count * sizeof(AssetEntry));

		for (const AssetEntry& entry : entries)
		{
			entryMap[entry.id] = { entry.offset, entry.size };
		}
		PakPath = folderPath;
		PakLoaded = true;
	}

	void AssetManager::RetrieveFromPak(uint64_t id, char*& buffer, uint64_t& size)
	{
		auto it = entryMap.find(id);
		if (it != entryMap.end())
		{
			AssetEntryValue entry = it->second;
			size = entry.size;
			buffer = new char[size];
			std::ifstream in(PakPath, std::ios::binary);
			in.seekg(entry.offset);
			in.read(buffer, size);
			in.close();
		}
		else
		{
			std::cerr << "Asset ID not found in pak file." << std::endl;
		}
	}

	static void ReadEntry(uint64_t id, uint8_t* data, uint64_t size)
	{
		auto it = entryMap.find(id);
		if (it != entryMap.end())
		{
			AssetEntryValue entry = it->second;
			std::ifstream in("assets.pak", std::ios::binary);
			in.seekg(entry.offset);
			in.read(reinterpret_cast<char*>(data), entry.size);
			in.close();
		}
		else
		{
			std::cerr << "Asset ID not found in pak file." << std::endl;
		}
	}

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

