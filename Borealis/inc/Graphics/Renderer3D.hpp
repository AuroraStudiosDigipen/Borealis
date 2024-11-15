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

		static void Begin(glm::mat4 viewProj, Ref<Shader> shader = nullptr);

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
		static void DrawMesh(const glm::mat4& transform, const MeshFilterComponent& meshFilter, const MeshRendererComponent& meshRenderer, Ref<Shader> shader, int entityID = -1);

		static void DrawHighlightedMesh(const glm::mat4& transform, const MeshFilterComponent& meshFilter, Ref<Shader> shader, bool filled = false, glm::vec4 color = { 0.043f, 0.8f, 0.961f , 1.f});

		static void DrawSkinnedMesh(const glm::mat4& transform, const SkinnedMeshRendererComponent & skinnedMeshRenderer, Ref<Shader> shader, int entityID = -1);

		static void DrawQuad();

		static void DrawCube(glm::vec3 translation, glm::vec3 minExtent, glm::vec3 maxExtent, glm::vec4 color, bool wireframe = false);
	
		static void SetGlobalWireFrameMode(bool wireFrameMode);
		static bool GetGlobalWireFrameMode();
	private:
		inline static bool mGlobalWireFrame = false;
		static LightEngine mLightEngine;
	};
}

#endif