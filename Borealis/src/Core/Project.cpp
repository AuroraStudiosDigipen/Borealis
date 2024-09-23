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

namespace Borealis
{
	ProjectInfo Project::mProjectInfo;

	std::shared_ptr<AssetManager> Project::mAssetManager = nullptr;

	void Project::CreateProject(std::string name, std::string path)
	{
		// Create Project file
		
		// Create directory if doesnt exist
		std::string projectFilePath = path;
		projectFilePath += "/";
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

		projectFilePath += "/Project.brproj";

		std::ofstream outStream(projectFilePath);
		outStream << out.c_str();
		outStream.close();

		mProjectInfo.ProjectName = name;
	}
	void Project::SetProjectPath(std::string path)
	{
		// check if project path exists
		std::string projectFile = path;
		std::string projectFilePath = projectFile.substr(0, projectFile.find_last_of("\\"));
		if (std::filesystem::exists(projectFilePath))
		{
			if (std::filesystem::exists(projectFile))
			{
				mProjectInfo.ProjectPath = projectFilePath;
				mProjectInfo.AssetsPath = projectFilePath + mProjectInfo.AssetsDirectoryName;
				mProjectInfo.AssetsRegistryPath = projectFilePath + mProjectInfo.AssetsRegistryName;

				GetEditorAssetsManager()->LoadRegistry(mProjectInfo.AssetsPath, mProjectInfo.AssetsRegistryPath);

				std::ifstream inStream(projectFile);
				std::stringstream ss;
				ss << inStream.rdbuf();
				inStream.close();

				YAML::Node data = YAML::Load(ss.str());
				mProjectInfo.ProjectName = data["ProjectName"].as<std::string>();

				// Load Scenes
				if (data["Scenes"])
				{
					for (auto scene : data["Scenes"])
					{
						std::string sceneName = scene["SceneName"].as<std::string>();
						std::string scenePath = scene["ScenePath"].as<std::string>();
						SceneManager::AddScene(sceneName, scenePath);
					}
					SceneManager::SetActiveScene(data["ActiveScene"].as<std::string>());
				}
				
			}
			else
			{
				BOREALIS_CORE_WARN("Project file does not exist in the specified path");
			}
		}
		else
		{
			BOREALIS_CORE_WARN("Specified path does not exist");
		}
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
		return std::string();
	}

	void Project::SaveProject()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectName" << YAML::Value << mProjectInfo.ProjectName;
		out << YAML::Key << "Scenes" << YAML::Value << YAML::BeginSeq;
		for (auto& [sceneName,scenePath] : SceneManager::GetSceneLibrary())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "SceneName" << YAML::Value << sceneName;
			out << YAML::Key << "ScenePath" << YAML::Value << scenePath;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::Key << "ActiveScene" << YAML::Value << SceneManager::GetActiveScene()->GetName();

		

		std::string projectFilePath = mProjectInfo.ProjectPath.string();
		projectFilePath += "/Project.brproj";

		std::ofstream outStream(projectFilePath);
		outStream << out.c_str();
		outStream.close();
	}
	void Project::BuildExportSettings(std::string buildPath, std::string buildName)
	{
		buildPath += "/" + buildName + ".brls";

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectName" << YAML::Value << buildName;
		out << YAML::Key << "Scenes" << YAML::Value << YAML::BeginSeq;
		for (auto& [sceneName, scenePath] : SceneManager::GetSceneLibrary())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "SceneName" << YAML::Value << sceneName;
			out << YAML::Key << "ScenePath" << YAML::Value << scenePath;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::Key << "ActiveScene" << YAML::Value << SceneManager::GetActiveScene()->GetName();

		std::ofstream outStream(buildPath);
		outStream << out.c_str();
		outStream.close();

	}
	void Project::CopyFolder(const std::filesystem::path& source, const std::filesystem::path& destination)
	{

		// Check if the source directory exists
		if (!std::filesystem::exists(source) || !std::filesystem::is_directory(source)) {
			BOREALIS_CORE_ERROR("Source directory does not exist or is not a directory");
			return;
		}

		// Create the destination directory if it doesn't exist
		if (!std::filesystem::exists(destination)) {
			std::filesystem::create_directory(destination);
		}

		// Iterate through the source directory
		for (const auto& entry : std::filesystem::directory_iterator(source)) {
			const auto& path = entry.path();
			auto dest_path = destination / path.filename(); // Construct destination path

			if (std::filesystem::is_directory(path)) {
				// Recursively copy the directory
				CopyFolder(path, dest_path);
			}
			else {
				// Copy the file
				std::filesystem::copy(path, dest_path, std::filesystem::copy_options::overwrite_existing);
			}


		}
	}


	void Project::CopyIndividualFile(const std::filesystem::path& source, const std::filesystem::path& destination)
	{
		std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
	}

	std::shared_ptr<EditorAssetManager> Project::GetEditorAssetsManager()
	{
		if (!mAssetManager)
			mAssetManager = std::make_shared<EditorAssetManager>();

		return std::static_pointer_cast<EditorAssetManager>(mAssetManager);
	}
}


