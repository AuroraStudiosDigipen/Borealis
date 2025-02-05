/******************************************************************************
/*!
\file       LightEngine.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       October 6, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>

#include <Graphics/LightEngine.hpp>

namespace Borealis
{
	void LightEngine::Begin()
	{
		mLights.clear();
	}

	void LightEngine::AddLight(LightComponent & light)
	{
		mLights.push_back(&light);
		//mNeedToUpdate = true;
	}

	void LightEngine::SetLights(Ref<UniformBufferObject> const& LightsUBO)
	{
		bool modified = false;

		for (LightComponent * light : mLights)
		{
			if (light->isEdited)
			{
				modified = true;
				light->isEdited = false;
			}
		}

		if(mNeedToUpdate || modified || mLights.empty())
		{
			for (int i{}; i < mLights.size(); ++i)
			{
				Light::SetUBO(*mLights[i], mLightsUBO[i]);
			}
			LightsUBO->SetData(mLightsUBO.data(), mLightsUBO.size() * sizeof(LightUBO));
			int lightsCount = static_cast<int>(mLights.size());
			LightsUBO->SetData(&lightsCount, sizeof(int), mLightsUBO.size() * sizeof(LightUBO));
		}
	}
}

