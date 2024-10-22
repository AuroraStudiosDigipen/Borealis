/******************************************************************************/
/*!
\file		Renderer3D.cpp
\author 	Chan Guo Geng Gordon
\par    	email: g.chan\@digipen.edu
\date   	September 11, 2024
\brief		Declares the class for Graphics Renderer3D

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "BorealisPCH.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "Graphics/Renderer3D.hpp"
#include <Graphics/VertexArray.hpp>
#include <Graphics/RenderCommand.hpp>

namespace Borealis
{
	struct Renderer3DData
	{
		Ref<Shader> mModelShader;
	};

	static std::unique_ptr<Renderer3DData> sData;
	LightEngine Renderer3D::mLightEngine;

	void Renderer3D::Init()
	{
		sData =  std::make_unique<Renderer3DData>();
		sData->mModelShader = Shader::Create("engineResources/Shaders/Renderer3D_Material.glsl");
	}



	void Renderer3D::Begin(const EditorCamera& camera)
	{
		sData->mModelShader->Bind();
		sData->mModelShader->Set("u_ViewProjection", camera.GetViewProjectionMatrix());

		mLightEngine.Begin();
	}

	void Renderer3D::Begin(const Camera& camera, const glm::mat4& transform)
	{
		sData->mModelShader->Bind();
		glm::mat4 viewProj = camera.GetProjectionMatrix() * glm::inverse(transform);
		sData->mModelShader->Set("u_ViewProjection", viewProj);

		mLightEngine.Begin();
	}

	void Renderer3D::End()
	{
		mLightEngine.Begin();//clear vector
	}

	void Renderer3D::AddLight(LightComponent const& lightComponent)
	{
		mLightEngine.AddLight(lightComponent);
	}

	void Renderer3D::SetLights(Ref<Shader> shader)
	{
		mLightEngine.SetLights(shader);
	}

	void Renderer3D::DrawMesh(const glm::mat4& transform, const MeshFilterComponent& meshFilter, const MeshRendererComponent& meshRenderer, int entityID)
	{
		SetLights(sData->mModelShader);
		if (meshFilter.Model) {
			if (meshRenderer.Material)
			{
				meshRenderer.Material->SetUniforms(sData->mModelShader);
			}
			
			meshFilter.Model->Draw(transform, sData->mModelShader, entityID);
		}
	}

}