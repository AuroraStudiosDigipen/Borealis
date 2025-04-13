/******************************************************************************/
/*!
\file		Project.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	September 09, 2024
\brief		Declares the class for the Project Management

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include <string>
#include <filesystem>
#include <memory>
#include <Core/ProjectInfo.hpp>
#include <Assets/IAssetManager.hpp>
#include <Assets/EditorAssetManager.hpp>

namespace Borealis
{
	class Project
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor of Layer System. Initializes the Layer System
			\param[in] name
				Name of the project
			\param[in] path
				Path of the project
		*************************************************************************/
		static void CreateProject(std::string name, std::string path);

		/*!***********************************************************************
			\brief
				Loads the project from the specified path
			\param[in] path
				Path of the project
			\param[out] activeSceneName
				Name of the active scene
			\return
				True if the project is loaded successfully, false otherwise
		*************************************************************************/
		static bool SetProjectPath(std::string path, std::string& activeSceneName, bool encrypt = false);

		/*!***********************************************************************
			\brief
				Gets the Project Path
			\return
				Path of the project
		*************************************************************************/
		static std::string GetProjectPath();

		/*!***********************************************************************
			\brief
				Gets the Project Name
			\return
				Name of the project
		*************************************************************************/
		static std::string GetProjectName();

		/*!***********************************************************************
			\brief
				Gets the Assets Path
			\return
				Path of the assets
		*************************************************************************/
		static std::string GetAssetsPath();

		/*!***********************************************************************
			\brief
				Saves the project
		*************************************************************************/
		static void SaveProject();

		/*!***********************************************************************
			\brief
				Gets the Project Info
		*************************************************************************/
		static ProjectInfo GetProjectInfo();

		/*!***********************************************************************
			\brief
				Creates the export settings when Building the project
			\param[in] buildPath
				Path to build the project
			\param[in] buildName
				Name of the build
		*************************************************************************/
		static void BuildExportSettings(std::string buildPath, std::string buildName, bool encrypt = true);

		/*!***********************************************************************
			\brief
				Utility to copy a folder
			\param[in] source
				Source path
			\param[in] destination
				Destination path
		*************************************************************************/
		static void CopyFolder(const std::filesystem::path& source, const std::filesystem::path& destination, std::string filter = ""
			,bool encrypt = false);

		/*!***********************************************************************
			\brief
				Utility to copy a file
			\param[in] source
				Source path
			\param[in] destination
				Destination path
		*************************************************************************/
		static void CopyIndividualFile(const std::filesystem::path& source, const std::filesystem::path& destination, bool encrypt = false);

		/*!***********************************************************************
			\brief
				Gets the Asset Manager
			\return
				Asset Manager
		*************************************************************************/
		static std::shared_ptr<EditorAssetManager> GetEditorAssetsManager();

		static std::vector<uint8_t> assembleKey();
		

		static std::vector<uint8_t> assembleIV();

	private:
		static ProjectInfo mProjectInfo; //!< Project Info

		static std::shared_ptr<IAssetManager> mAssetManager; //!< Asset Manager
	};

}
#endif