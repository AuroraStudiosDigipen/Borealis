/******************************************************************************
/*!
\file       AssetManager.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 27, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef AssetManager_HPP
#define AssetManager_HPP

#include <Core/Core.hpp>
#include <Core/Project.hpp>
#include <Core/ApplicationManager.hpp>

namespace Borealis
{
	class AssetManager
	{
	public:
		/*!***********************************************************************
			\brief
				Get Asset by handle
		*************************************************************************/
		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			if(mRunTime)
			{
				Ref<Asset> asset = mAssetManager.GetAsset(handle);
				return std::static_pointer_cast<T>(asset);
			}
			Ref<Asset> asset = Project::GetEditorAssetsManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		/*!***********************************************************************
			\brief
				Get meta data by handle
		*************************************************************************/
		static AssetMetaData const& GetMetaData(AssetHandle handle)
		{
			if (mRunTime)
			{
				return mAssetManager.GetMetaData(handle);
			}
			return Project::GetEditorAssetsManager()->GetMetaData(handle);
		}

		//TEMP
		//===============================================================
		static void InsertMetaData(AssetMetaData data)
		{
			if (!mRunTime)
			{
				Project::GetEditorAssetsManager()->GetAssetRegistry().insert({ data.Handle, data });
			}
		}

		static void SetRunTime()
		{
			mRunTime = true;
			mAssetManager.LoadAssetRegistryRunTime("AssetRegistry.brdb");
		}

	private:
		inline static bool mRunTime = false;
		inline static EditorAssetManager mAssetManager;
	};
}

#endif
