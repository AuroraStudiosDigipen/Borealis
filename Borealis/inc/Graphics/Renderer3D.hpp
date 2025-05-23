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

		static void End(bool posOnly = false);

		static void RenderTransparentObjects(Ref<TextureCubeMap> const& cubeMap);

		static void AddLight(LightComponent & lightComponent);

		static void SetLights(Ref<UniformBufferObject> const& LightsUBO);

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

		static void DrawHighlightedMesh(const glm::mat4& transform, const MeshFilterComponent& meshFilter, Ref<Shader> shader);

		static void DrawHighlightedMesh(const glm::mat4& transform, const SkinnedMeshRendererComponent& meshFilter, Ref<Shader> shader);

		static void DrawSkinnedMesh(const glm::mat4& transform, const SkinnedMeshRendererComponent & skinnedMeshRenderer, Ref<Shader> shader, int entityID = -1, int animationIndex = -1);

		static void DrawQuad();
		static void DrawQuad(const glm::mat4& transform, glm::vec4 color, bool wireframe = true);

		static void DrawCube(const glm::mat4& transform, glm::vec4 color, bool wireframe = true);

		static void DrawCubeMap();
		
		static void BeginCommonShapes(glm::mat4 const& viewProj);

		static void DrawCube(glm::vec3 translation, glm::vec3 minExtent, glm::vec3 maxExtent, glm::vec4 color, bool wireframe = false);

		static void DrawSphere(glm::vec3 center, float radius, glm::vec4 color, bool wireframe = false);

		static void DrawCylinder(glm::vec3 center, float radius, float height, glm::vec4 color, bool wireframe = false);

		static void DrawCapsule(glm::vec3 center, float radius, float height, glm::vec4 color, bool wireframe = false);

		static void DrawCone(float height, float baseThickness, float angle, const glm::mat4& transform, glm::vec4 color, bool wireframe = true);
	
		static void SetGlobalWireFrameMode(bool wireFrameMode);
		static bool GetGlobalWireFrameMode();

		static void UpdateMaterialUBO();

		struct DrawData
		{
			bool hasAnimation{};
			int animationIndex{};
		};

		struct DrawCall
		{
			std::variant<Ref<Model>,Ref<SkinnedModel>> model;
			Ref<Shader> shaderID;
			std::size_t materialHash;
			uint32_t entityID;
			glm::mat4 transform;
			DrawData drawData;
		};

	private:
		inline static bool mGlobalWireFrame = false;
		inline static bool mNewMaterialAdded = false;
		static LightEngine mLightEngine;

		inline static std::vector<DrawCall> drawQueue;
		inline static std::vector<DrawCall> drawQueueTransparent;
		inline static std::unordered_map<std::size_t, Ref<Material>> materialMap;
		inline static std::unordered_map<std::size_t, MaterialUBOData> materialUBODataMap;
		static void AddToDrawQueue(std::variant<Ref<Model>, Ref<SkinnedModel>> model, Ref<Shader> shaderID, Ref<Material> materialHash, uint32_t entityID, glm::mat4 const& transform, std::optional<DrawData> drawData = std::nullopt);
	};
}

#endif