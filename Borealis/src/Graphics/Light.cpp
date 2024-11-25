/******************************************************************************/
/*!
\file		Light.cpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date   	September 30, 2024
\brief		Declares the class for Light for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "BorealisPCH.hpp"
#include "Graphics/Light.hpp"

namespace Borealis
{
	Light::Light(const TransformComponent& transformComponent, const LightComponent& lightComponent)
	{
		mLight = lightComponent;
		mTranslate = transformComponent.Translate;
	}

	void Light::SetUniforms(LightComponent const& lightComponent, int index, Ref<Shader> shader)
	{
		shader->Bind();

		std::string lightPrefix = "u_Lights[" + std::to_string(index) + "]";

		float innerAngle = glm::radians(lightComponent.spotAngle * 0.8f);  // Example: 80% of outer angle
		float outerAngle = glm::radians(lightComponent.spotAngle);

		glm::vec2 innerOuterSpot = glm::vec2{ cos(innerAngle), cos(outerAngle) };

		std::string str = lightPrefix + ".position";
		shader->Set(str.c_str(), lightComponent.position);
		str = lightPrefix + ".ambient";
		shader->Set(str.c_str(), glm::vec3(lightComponent.color) * 0.8f * lightComponent.Intensity);
		str = lightPrefix + ".diffuse";
		shader->Set(str.c_str(), glm::vec3(lightComponent.color) * 0.5f * lightComponent.Intensity);
		str = lightPrefix + ".specular";
		shader->Set(str.c_str(), glm::vec3(lightComponent.color) * (lightComponent.Intensity * 0.5f));
		str = lightPrefix + ".direction";
		shader->Set(str.c_str(), lightComponent.direction);
		str = lightPrefix + ".innerOuterAngle";
		shader->Set(str.c_str(), innerOuterSpot);
		str = lightPrefix + ".linear";
		shader->Set(str.c_str(), 1.0f / lightComponent.range);
		str = lightPrefix + ".quadratic";
		shader->Set(str.c_str(), 1.0f / (lightComponent.range * lightComponent.range));
		str = lightPrefix + ".type";
		shader->Set(str.c_str(), static_cast<int>(lightComponent.type));
		str = lightPrefix + ".castShadow";
		shader->Set(str.c_str(), lightComponent.castShadow);

		shader->Unbind();
	}
}