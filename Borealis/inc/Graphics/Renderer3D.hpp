/******************************************************************************/
/*!
\file		Renderer3D.hpp
\author 	Chan Guo Geng Gordon
\par    	email: g.chan\@digipen.edu
\date   	September 11, 2024
\brief		Declares the class for Graphics Renderer3D

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef RENDERER3D_HPP
#define RENDERER3D_HPP
#include <Graphics/OrthographicCamera.hpp>
#include <Graphics/Camera.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/EditorCamera.hpp>
#include <Graphics/LightEngine.hpp>
#include <Scene/Components.hpp>

#include "Light.hpp"

namespace Borealis
{
	class Renderer3D
	{
	public:
		/*!***********************************************************************
			\brief
				Init for Renderer3d
		*************************************************************************/
		static void Init();

		/*!***********************************************************************
			\brief
				Setup the renderer for editor camera
			\param[in] camera
				camera
		*************************************************************************/
		static void Begin(const EditorCamera& camera);

		/*!***********************************************************************
			\brief
				Setup the renderer for camera
			\param[in] camera
				camera
			\param[in] transform
				transform
		*************************************************************************/
		static void Begin(const Camera& camera, const glm::mat4& transform);

		static void End();

		static void AddLight(LightComponent const& lightComponent);

		static void SetLights(Ref<Shader> shader);

		/*!***********************************************************************
			\brief
				Draw the mesh
			\param[in] transform
				camera
			\param[in] meshFilter
				transform
			\param[in] meshRenderer
				transform
			\param[in] light
				transform
		*************************************************************************/
		static void DrawMesh(const glm::mat4& transform, const MeshFilterComponent& meshFilter, const MeshRendererComponent& meshRenderer, int entityID = -1);

	private:
		static LightEngine mLightEngine;
	};
}

#endif