/******************************************************************************/
/*!
\file		Light.hpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date   	September 30, 2024
\brief		Declares the class for Light for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef LIGHT_HPP
#define LIGHT_HPP
#include <glm/glm.hpp>
#include <Graphics/Shader.hpp>
#include <Core/Core.hpp>
#include "Scene/Components.hpp"

namespace Borealis
{
	class Light
	{
	public:
		Light(const TransformComponent& transformComponent, const LightComponent& lightComponent);

		void SetUniforms(Ref<Shader> shader) const;

		static void SetUniforms(LightComponent const& lightComponent, int index, Ref<Shader> shader);

	private:
		LightComponent mLight;
		glm::vec3 mTranslate;

	};
}

#endif