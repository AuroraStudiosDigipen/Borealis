#include "ResourceManager.hpp"
/******************************************************************************
/*!
\file       ResourceManager.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       October 02, 2024
\brief      Defines Resource manager class for the editor's resources

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

namespace Borealis
{
	std::unordered_map<FileIcon, Ref<Texture2D>> ResourceManager::mFileIcons;

	void Borealis::ResourceManager::LoadResources()
	{
		// Load Icons
		mFileIcons[FileIcon::Directory] = Texture2D::Create("resources/icons/directoryIcon.dds");
		mFileIcons[FileIcon::Scene] = Texture2D::Create("resources/icons/file-sc.dds");
		mFileIcons[FileIcon::Script] = Texture2D::Create("resources/icons/file-cs.dds");
		mFileIcons[FileIcon::Font] = Texture2D::Create("resources/icons/file-ttf.dds");
		mFileIcons[FileIcon::Text] = Texture2D::Create("resources/icons/file-txt.dds");
		mFileIcons[FileIcon::Shader] = Texture2D::Create("resources/icons/file-glsl.dds");
		mFileIcons[FileIcon::Material] = Texture2D::Create("resources/icons/file-mat.dds");
		mFileIcons[FileIcon::Mesh] = Texture2D::Create("resources/icons/file-mesh.dds");
		mFileIcons[FileIcon::Texture] = Texture2D::Create("resources/icons/file-png.dds");
		mFileIcons[FileIcon::Unknown] = Texture2D::Create("resources/icons/file-unknown-notext.dds");
		mFileIcons[FileIcon::Audio] = Texture2D::Create("resources/icons/MusicNote.dds");

	}
	void ResourceManager::Init()
	{
		LoadResources();
	}
	Ref<Texture2D> ResourceManager::GetFileIcon(FileIcon icon)
	{
		return mFileIcons[icon];
	}
	void ResourceManager::UnloadResource(FileIcon icon)
	{
		mFileIcons.erase(icon);
	}
	void ResourceManager::Free()
	{
		UnloadResources();
	}
	void ResourceManager::UnloadResources()
	{
		mFileIcons.clear();
	}
}

