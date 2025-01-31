/******************************************************************************
/*!
\file       LightEngine.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       October 6, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef LightEngine_HPP
#define LightEngine_HPP

#include <vector>

#include <Graphics/Shader.hpp>
#include <Graphics/UniformBufferObject.hpp>
#include <Graphics/Light.hpp>
#include <Scene/Components.hpp>

#define MAX_LIGHTS 32

namespace Borealis
{
	class LightEngine
	{
	public:
		/*!***********************************************************************
			\brief
				Begin the light engine for the loop
		*************************************************************************/
		void Begin();

		/*!***********************************************************************
			\brief
				Add light component to list
		*************************************************************************/
		void AddLight(LightComponent const& light);

		/*!***********************************************************************
			\brief
				Set the uniforms for the lights
		*************************************************************************/
		void SetLights(Ref<UniformBufferObject> const& LightsUBO);

	private:
		std::vector<LightComponent const*> mLights;
		std::array<LightUBO, 32> mLightsUBO;
		
	};
}

#endif
