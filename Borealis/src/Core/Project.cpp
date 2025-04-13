/******************************************************************************/
/*!
\file		Project.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	September 09, 2024
\brief		Defines the class for the Project Management

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <yaml-cpp/yaml.h>
#include <Core/Project.hpp>
#include <Core/LoggerSystem.hpp>
#include <Core/LoggerSystem.hpp>
#include <Scene/SceneManager.hpp>
#include <Core/LayerList.hpp>
#include <Audio/AudioEngine.hpp>
#include <openssl/evp.h>
#include <cstdint>
#include <cstring>
namespace Borealis
{
	ProjectInfo Project::mProjectInfo;

	std::shared_ptr<IAssetManager> Project::mAssetManager = nullptr;

	static const uint8_t key_part1[] = { 0xA3, 0x1F, 0x56, 0x90 };
	static const uint8_t key_part2[] = { 0xDE, 0x34, 0x12, 0x78 };
	static const uint8_t key_part3[] = { 0xC0, 0xFF, 0xEE, 0x11 };
	static const uint8_t key_part4[] = { 0x22, 0x33, 0x44, 0x55 };
	static const uint8_t key_part5[] = { 0x66, 0x77, 0x88, 0x99 };
	static const uint8_t key_part6[] = { 0xAB, 0xCD, 0xEF, 0x01 };
	static const uint8_t key_part7[] = { 0x23, 0x45, 0x67, 0x89 };
	static const uint8_t key_part8[] = { 0x10, 0x20, 0x30, 0x40 };

	static const uint8_t iv_part1[] = { 0x1A, 0x2B, 0x3C, 0x4D };
	static const uint8_t iv_part2[] = { 0x5E, 0x6F, 0x70, 0x81 };
	static const uint8_t iv_part3[] = { 0x92, 0xA3, 0xB4, 0xC5 };
	static const uint8_t iv_part4[] = { 0xD6, 0xE7, 0xF8, 0x09 };


	static bool decryptFileToStream(const std::string& inputPath,
		unsigned char* key, unsigned char* iv,
		std::stringstream& decryptedStream) {
		std::ifstream inFile(inputPath, std::ios::binary);
		if (!inFile) return false;

		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (!ctx) return false;

		EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);

		const size_t bufferSize = 4096;
		std::vector<unsigned char> inBuf(bufferSize);
		std::vector<unsigned char> outBuf(bufferSize);

		int outLen = 0;
		while (inFile.good()) {
			inFile.read(reinterpret_cast<char*>(inBuf.data()), bufferSize);
			std::streamsize readBytes = inFile.gcount();

			if (!EVP_DecryptUpdate(ctx, outBuf.data(), &outLen, inBuf.data(), static_cast<int>(readBytes))) {
				EVP_CIPHER_CTX_free(ctx);
				return false;
			}

			// Write decrypted data to stringstream
			decryptedStream.write(reinterpret_cast<char*>(outBuf.data()), outLen);
		}

		if (!EVP_DecryptFinal_ex(ctx, outBuf.data(), &outLen)) {
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}

		// Write the final decrypted data to stringstream
		decryptedStream.write(reinterpret_cast<char*>(outBuf.data()), outLen);

		EVP_CIPHER_CTX_free(ctx);
		return true;
	}

	static bool encryptString(const std::string& inputString, const std::string& outputPath,
		unsigned char* key, unsigned char* iv)
	{
		std::istringstream inFile(inputString);
		std::ofstream outFile(outputPath, std::ios::binary);
		if (!outFile) return false;

		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (!ctx) return false;

		EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);

		const size_t bufferSize = 4096;
		std::vector<unsigned char> inBuf(bufferSize);
		std::vector<unsigned char> outBuf(bufferSize + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

		int outLen = 0;
		while (inFile.good()) {
			inFile.read(reinterpret_cast<char*>(inBuf.data()), bufferSize);
			std::streamsize readBytes = inFile.gcount();

			if (!EVP_EncryptUpdate(ctx, outBuf.data(), &outLen, inBuf.data(), static_cast<int>(readBytes))) {
				EVP_CIPHER_CTX_free(ctx);
				return false;
			}
			outFile.write(reinterpret_cast<char*>(outBuf.data()), outLen);
		}

		if (!EVP_EncryptFinal_ex(ctx, outBuf.data(), &outLen)) {
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}
		outFile.write(reinterpret_cast<char*>(outBuf.data()), outLen);

		EVP_CIPHER_CTX_free(ctx);
		return true;
	}


	static bool encryptFile(const std::string& inputPath, const std::string& outputPath,
			unsigned char* key, unsigned char* iv) {
		std::ifstream inFile(inputPath, std::ios::binary);
		std::ofstream outFile(outputPath, std::ios::binary);
		if (!inFile || !outFile) return false;

		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (!ctx) return false;

		EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);

		const size_t bufferSize = 4096;
		std::vector<unsigned char> inBuf(bufferSize);
		std::vector<unsigned char> outBuf(bufferSize + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

		int outLen = 0;
		while (inFile.good()) {
			inFile.read(reinterpret_cast<char*>(inBuf.data()), bufferSize);
			std::streamsize readBytes = inFile.gcount();

			if (!EVP_EncryptUpdate(ctx, outBuf.data(), &outLen, inBuf.data(), static_cast<int>(readBytes))) {
				EVP_CIPHER_CTX_free(ctx);
				return false;
			}
			outFile.write(reinterpret_cast<char*>(outBuf.data()), outLen);
		}

		if (!EVP_EncryptFinal_ex(ctx, outBuf.data(), &outLen)) {
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}
		outFile.write(reinterpret_cast<char*>(outBuf.data()), outLen);

		EVP_CIPHER_CTX_free(ctx);
		return true;
	}

	void Project::CreateProject(std::string name, std::string path)
	{
		// Create Project file
		
		// Create directory if doesnt exist
		std::string projectFilePath = path;
		projectFilePath += "\\";
		projectFilePath += name;

		std::filesystem::path fileSystemPaths = projectFilePath;
		std::filesystem::create_directories(fileSystemPaths);

		std::string filepath = projectFilePath;
		filepath += mProjectInfo.AssetsDirectoryName;
		std::filesystem::create_directories(filepath);
		
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectName" << YAML::Value << name;

		mProjectInfo.ProjectPath = projectFilePath;
		mProjectInfo.AssetsPath = projectFilePath;
		mProjectInfo.CachePath = std::filesystem::path(projectFilePath).replace_filename("Cache");

		projectFilePath += "/Project.brproj";

		std::ofstream outStream(projectFilePath);
		outStream << out.c_str();
		outStream.close();

		mProjectInfo.ProjectName = name;
	}

	bool Project::SetProjectPath(std::string path, std::string& activeSceneName, bool encrypt)
	{
		// check if project path exists
		std::string retScene = "";
		std::string projectFile = path;
		std::string projectFilePath = projectFile.substr(0, projectFile.find_last_of("\\"));
		if (std::filesystem::exists(projectFilePath))
		{
			if (std::filesystem::exists(projectFile))
			{
				mProjectInfo.ProjectPath = projectFilePath;
				mProjectInfo.AssetsPath = projectFilePath + mProjectInfo.AssetsDirectoryName;
				mProjectInfo.AssetsRegistryPath = projectFilePath + mProjectInfo.AssetsRegistryName;
				mProjectInfo.CachePath = std::filesystem::path(mProjectInfo.AssetsPath).replace_filename("Cache");

				AudioEngine::Init(projectFilePath);

				LayerList::Reset();
				////pass in project info
				//GetEditorAssetsManager()->LoadRegistry(mProjectInfo);
				std::stringstream ss;
				if (encrypt)
				{
					decryptFileToStream(projectFile, assembleKey().data(), assembleIV().data(), ss);
				}
				else
				{
					std::ifstream inStream(projectFile);
					ss << inStream.rdbuf();
					inStream.close();
				}
				

				YAML::Node data = YAML::Load(ss.str());
				mProjectInfo.ProjectName = data["ProjectName"].as<std::string>();

				if (data["LayerNames"])
				{
					for (const auto& item : data["LayerNames"])
					{
						LayerList::SetLayer(item.first.as<int>(), item.second.as<std::string>());
					}
				}

				// Load Scenes
				if (data["Scenes"])
				{
					for (auto scene : data["Scenes"])
					{
						std::string sceneName = scene["SceneName"].as<std::string>();
						std::string scenePath = scene["ScenePath"].as<std::string>();
						scenePath = projectFilePath + "\\" + scenePath;

						// check if path exists
						if (!std::filesystem::exists(scenePath))
						{
							BOREALIS_CORE_WARN("Scene file {} does not exist in the specified path, scene will not be loaded", scenePath);
							continue;
						}

						SceneManager::AddScene(sceneName, scenePath);
					}

					if (SceneManager::GetSceneLibrary().empty())
					{
						BOREALIS_CORE_WARN("No scenes have been loaded");
					}
					else
					{
						activeSceneName = data["ActiveScene"].as<std::string>();
					}
				}
				
			}
			else
			{
				return false;
				BOREALIS_CORE_WARN("Project file does not exist in the specified path");
			}
		}
		else
		{
			return false;
			BOREALIS_CORE_WARN("Specified path does not exist");
		}

		return true;
	}
	std::string Project::GetProjectPath()
	{
		return mProjectInfo.ProjectPath.string();
	}
	std::string Project::GetProjectName()
	{
		return mProjectInfo.ProjectName;
	}

	std::string Project::GetAssetsPath()
	{
		return mProjectInfo.AssetsPath.string();
	}

	void Project::SaveProject()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectName" << YAML::Value << mProjectInfo.ProjectName;
		out << YAML::Key << "Scenes" << YAML::Value << YAML::BeginSeq;
		for (auto [sceneName,scenePath] : SceneManager::GetSceneLibrary())
		{
			scenePath = scenePath.substr(mProjectInfo.ProjectPath.string().length() + 1);

			out << YAML::BeginMap;
			out << YAML::Key << "SceneName" << YAML::Value << sceneName;
			out << YAML::Key << "ScenePath" << YAML::Value << scenePath;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::Key << "ActiveScene" << YAML::Value << SceneManager::GetActiveScene()->GetName();


		out << YAML::Key << "LayerNames";
		out << YAML::BeginMap;
		for (int i = 6; i < 32; i++)
		{
			if (LayerList::HasIndex(i))
			{
				out << YAML::Key << i << YAML::Value << LayerList::IndexToLayer(i);
			}
		}
		out << YAML::EndMap;

		std::string projectFilePath = mProjectInfo.ProjectPath.string();
		projectFilePath += "/Project.brproj";

		std::ofstream outStream(projectFilePath);
		outStream << out.c_str();
		outStream.close();
	}

	ProjectInfo Project::GetProjectInfo()
	{
		return mProjectInfo;
	}

	void Project::BuildExportSettings(std::string buildPath, std::string buildName, bool encrypt)
	{
		buildPath += "/" + buildName + ".brls";

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectName" << YAML::Value << buildName;
		out << YAML::Key << "Scenes" << YAML::Value << YAML::BeginSeq;
		for (auto& [sceneName, scenePath] : SceneManager::GetSceneLibrary())
		{
			scenePath = scenePath.substr(mProjectInfo.ProjectPath.string().length() + 1);
			out << YAML::BeginMap;
			out << YAML::Key << "SceneName" << YAML::Value << sceneName;
			out << YAML::Key << "ScenePath" << YAML::Value << scenePath;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::Key << "ActiveScene" << YAML::Value << SceneManager::GetActiveScene()->GetName();


		out << YAML::Key << "LayerNames";
		out << YAML::BeginMap;
		for (int i = 6; i < 32; i++)
		{
			if (LayerList::HasIndex(i))
			{
				out << YAML::Key << i << YAML::Value << LayerList::IndexToLayer(i);
			}
		}
		out << YAML::EndMap;

		if (encrypt)
			encryptString(out.c_str(), buildPath, assembleKey().data(), assembleIV().data());
		else
		{
			std::ofstream outStream(buildPath);
			outStream << out.c_str();
			outStream.close();
		}

	}

	std::vector<uint8_t> Project::assembleKey() {
		std::vector<uint8_t> key(32);
		memcpy(key.data(), key_part1, 4);
		memcpy(key.data() + 4, key_part2, 4);
		memcpy(key.data() + 8, key_part3, 4);
		memcpy(key.data() + 12, key_part4, 4);
		memcpy(key.data() + 16, key_part5, 4);
		memcpy(key.data() + 20, key_part6, 4);
		memcpy(key.data() + 24, key_part7, 4);
		memcpy(key.data() + 28, key_part8, 4);
		return key;
	}

	std::vector<uint8_t> Project::assembleIV() {
		std::vector<uint8_t> iv(16);
		memcpy(iv.data(), iv_part1, 4);
		memcpy(iv.data() + 4, iv_part2, 4);
		memcpy(iv.data() + 8, iv_part3, 4);
		memcpy(iv.data() + 12, iv_part4, 4);
		return iv;
	}

	void Project::CopyFolder(const std::filesystem::path& source, const std::filesystem::path& destination, std::string filter,
		bool encrypt)
	{

		// Check if the source directory exists
		if (!std::filesystem::exists(source) || !std::filesystem::is_directory(source)) {
			BOREALIS_CORE_ERROR("Source directory does not exist or is not a directory");
			return;
		}


		// Iterate through the source directory
		for (const auto& entry : std::filesystem::directory_iterator(source)) {
			const auto& path = entry.path();
			auto dest_path = destination / path.filename(); // Construct destination path

			if (std::filesystem::is_directory(path)) {
				// Recursively copy the directory
				CopyFolder(path, dest_path, filter);
			}
			else {
				// Copy the file
				if (filter == "" || path.extension().string() == filter)
				{
					if (!std::filesystem::exists(dest_path.parent_path()))
					{
						std::filesystem::create_directories(dest_path.parent_path());
					}
					if (!encrypt)
					{
						std::filesystem::copy(path, dest_path, std::filesystem::copy_options::overwrite_existing);
					}
					else
					{
						std::vector<uint8_t> key = assembleKey();
						std::vector<uint8_t> iv = assembleIV();
						encryptFile(path.string(), dest_path.string(), key.data(), iv.data());
					}
				}
			}


		}
	}


	void Project::CopyIndividualFile(const std::filesystem::path& source, const std::filesystem::path& destination, bool encrypt)
	{
		if (!std::filesystem::exists(destination.parent_path()))
		{
			std::filesystem::create_directories(destination.parent_path());
		}
		if (encrypt)
		{
			std::vector<uint8_t> key = assembleKey();
			std::vector<uint8_t> iv = assembleIV();
			encryptFile(source.string(), destination.string(), key.data(), iv.data());
		}
		else
		{
			std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
		}
	}

	std::shared_ptr<EditorAssetManager> Project::GetEditorAssetsManager()
	{
		if (!mAssetManager)
			mAssetManager = std::make_shared<EditorAssetManager>();

		return std::static_pointer_cast<EditorAssetManager>(mAssetManager);
	}
}


