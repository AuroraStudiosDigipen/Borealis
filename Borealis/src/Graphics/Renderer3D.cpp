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
		Ref<Shader> mCommonShader;
	};

	static std::unique_ptr<Renderer3DData> s3dData;
	LightEngine Renderer3D::mLightEngine;

	void Renderer3D::Init()
	{
		s3dData =  std::make_unique<Renderer3DData>();
		//s3dData->mModelShader = Shader::Create("engineResources/Shaders/Renderer3D_Material.glsl");
		s3dData->mModelShader = Shader::Create("engineResources/Shaders/Renderer3D_Material.glsl");

		s3dData->mCommonShader = Shader::Create("engineResources/Shaders/Renderer3D_Common.glsl");
	}



	void Renderer3D::Begin(const EditorCamera& camera)
	{
		Renderer3D::Begin(camera.GetViewProjectionMatrix());
	}

	void Renderer3D::Begin(const Camera& camera, const glm::mat4& transform)
	{
		glm::mat4 viewProj = camera.GetProjectionMatrix() * glm::inverse(transform);

		Renderer3D::Begin(viewProj);
	}

	void Renderer3D::Begin(glm::mat4 viewProj, Ref<Shader> shader)
	{
		s3dData->mModelShader->Bind();
		s3dData->mModelShader->Set("u_ViewProjection", viewProj);

		s3dData->mCommonShader->Bind();
		s3dData->mCommonShader->Set("u_ViewProjection", viewProj);

		if(shader)
		{
			shader->Bind();
			shader->Set("u_ViewProjection", viewProj);
		}

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
		SetLights(s3dData->mModelShader);
		if (meshFilter.Model) 
		{
			if (meshRenderer.Material)
			{
				meshRenderer.Material->SetUniforms(s3dData->mModelShader);
			}
			
			meshFilter.Model->Draw(transform, s3dData->mModelShader, entityID);
		}
	}

	void Renderer3D::DrawMesh(const glm::mat4& transform, const MeshFilterComponent& meshFilter, const MeshRendererComponent& meshRenderer, Ref<Shader> shader, int entityID)
	{
		//SetLights(shader);
		if (meshFilter.Model) 
		{
			if (meshRenderer.Material)
			{
				meshRenderer.Material->SetUniforms(shader);
			}

			meshFilter.Model->Draw(transform, shader, entityID);
		}
	}

	void Renderer3D::DrawHighlightedMesh(const glm::mat4& transform, const MeshFilterComponent& meshFilter, Ref<Shader> shader)
	{
		if (meshFilter.Model)
		{
			meshFilter.Model->Draw(transform, shader, -1);
		}
	}

	void Renderer3D::DrawSkinnedMesh(const glm::mat4& transform, const SkinnedMeshRendererComponent& skinnedMeshRenderer, Ref<Shader> shader, int entityID)
	{
		if (skinnedMeshRenderer.SkinnnedModel)
		{
			if (skinnedMeshRenderer.Material)
			{
				skinnedMeshRenderer.Material->SetUniforms(shader);
			}

			skinnedMeshRenderer.SkinnnedModel->Draw(transform, shader, entityID);
		}
	}

	void Renderer3D::DrawQuad() //full screen quad
	{
		Mesh::DrawQuad();
	}

	void Renderer3D::DrawCube(glm::vec3 translation, glm::vec3 minExtent, glm::vec3 maxExtent, glm::vec4 color, bool wireframe)
	{
		Mesh::DrawCube(translation, minExtent, maxExtent, color, wireframe, s3dData->mCommonShader);
	}

	void Renderer3D::SetGlobalWireFrameMode(bool wireFrameMode)
	{
		mGlobalWireFrame = wireFrameMode;
	}

	bool Renderer3D::GetGlobalWireFrameMode()
	{
		return mGlobalWireFrame;
	}

}