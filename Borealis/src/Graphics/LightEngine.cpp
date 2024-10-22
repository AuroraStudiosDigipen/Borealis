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
#include <Graphics/Light.hpp>

namespace Borealis
{
	void LightEngine::Begin()
	{
		mLights.clear();
	}

	void LightEngine::AddLight(LightComponent const& light)
	{
		mLights.push_back(&light);
	}

	void LightEngine::SetLights(Ref<Shader> shader)
	{
		shader->Bind();
		shader->Set("u_LightsCount", static_cast<int>(mLights.size()));
		shader->Unbind();


		for (int i{}; i < mLights.size(); ++i)
		{
			Light::SetUniforms(*mLights[i], i, shader);
		}
	}
}

