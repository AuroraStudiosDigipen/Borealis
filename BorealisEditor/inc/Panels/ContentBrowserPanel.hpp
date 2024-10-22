/******************************************************************************/
/*!
\file		ContentBrowserPanel.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 13, 2024
\brief		Declares the class Content Browser in Level Editor

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#ifndef CONTENT_BROWSER_PANEL_HPP
#define CONTENT_BROWSER_PANEL_HPP
#include <unordered_map>
#include <filesystem>
#include <Scene/scene.hpp>
#include <Scene/Entity.hpp>
#include <Core/core.hpp>
#include <Graphics/Texture.hpp>

namespace Borealis
{
	class ContentBrowserPanel
	{
	public:
		/*!***********************************************************************
			\brief
				Construct the Content Browser Panel
		*************************************************************************/
		ContentBrowserPanel();

		/*!***********************************************************************
			\brief
				Render the Content Browser Panel
		*************************************************************************/
		void ImGuiRender();

		/*!***********************************************************************
			\brief
				Set the directory of the Content Browser Panel
		*************************************************************************/
		void SetCurrDir(std::string path);
		static UUID sSelectedAsset;

	private:
		 char textBuffer[128] = { 0 };
		 std::filesystem::path mCurrDir;
		 std::string mAssetsDir;
		 int mThumbnailSize = 256;
		 int mPadding = 2;
		 const int mMinThumbnailSize = 62;
		 bool isCreatingScene = false;
		 bool isCreatingMaterial = false;

	};
}

#endif
