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
#include <Scene/Components.hpp>

namespace Borealis
{
	class LightEngine
	{
	public:
		void Begin();

		void AddLight(LightComponent const& light);

		void SetLights(Ref<Shader> shader);

	private:
		std::vector<LightComponent const*> mLights;
	};
}

#endif
