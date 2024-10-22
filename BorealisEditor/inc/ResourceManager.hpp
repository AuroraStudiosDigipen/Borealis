/******************************************************************************
/*!
\file       ResourceManager.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 22, 2024
\brief      Declares Resource manager class for the editor's resources

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef ResourceManager_HPP
#define ResourceManager_HPP
#include <Core/Core.hpp>
#include <Graphics/Texture.hpp>
#include <unordered_map>
namespace Borealis
{
	enum class FileIcon
	{
		Directory,
		Scene,
		Script,
		Shader,
		Audio,
		Font,
		Text,
		Material,
		Mesh,
		Texture,
		Unknown
	};

	enum class ButtonIcon
	{

	};

	enum class PrimitiveEntities
	{
		Cube,
		Sphere,
		Cylinder,
		Cone,
		Plane,
		Quad,
		Unknown
	};

	enum class EditorFonts // Used for editor UI
	{
		Normal,
		Bold,
		Italic
	};

	enum class EngineFonts // Used for text components in the default engine
	{
		Normal,
		Bold,
		Italic
	};

	class ResourceManager
	{
		
	public:
		/*!***********************************************************************
			\brief
				Initializes the resource manager
		*************************************************************************/
		static void Init();

		/*!***********************************************************************
			\brief
				Gets a file icon texture
			\param[in] icon
				The file icon to get
			\return
				The texture of the file icon
		*************************************************************************/
		static Ref<Texture2D> GetFileIcon(FileIcon icon);

		/*!***********************************************************************
			\brief
				Unloads a file icon texture
			\param[in] icon
				The file icon to unload
		*************************************************************************/
		static void UnloadResource(FileIcon icon);

		/*!***********************************************************************
			\brief
				Frees all the resources in resource manager
		*************************************************************************/
		static void Free();
	private:

		/*!***********************************************************************
			\brief
				Loads all resources
		*************************************************************************/
		static void LoadResources();

		/*!***********************************************************************
			\brief
				Unloads all resources
		*************************************************************************/
		static void UnloadResources();
		static std::unordered_map<FileIcon, Ref<Texture2D>> mFileIcons;
		//static Ref<Texture2D> GetFileIcon(FileIcon icon);
		//static Ref<Texture2D> GetButtonIcon(ButtonIcon icon);
	};
}

#endif
