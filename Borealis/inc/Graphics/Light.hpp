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
	struct LightUBO
	{
		glm::vec4 pos;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
		glm::vec4 direction;
		glm::vec2 innerOuterDirection;
		float linear;
		float quadratic;
		int type;
		int castShadow;

		glm::vec2 padding;
	};

	class Light
	{
	public:
		Light(const TransformComponent& transformComponent, const LightComponent& lightComponent);
		static void SetUniforms(LightComponent const& lightComponent, int index, Ref<Shader> shader);
		static void SetUBO(LightComponent const& lightComponent,LightUBO& lightUBO);

	private:
		LightComponent mLight;
		glm::vec3 mTranslate;

	};
}

#endif