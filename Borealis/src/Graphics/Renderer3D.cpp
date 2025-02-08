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
#include <Graphics/UniformBufferObject.hpp>
#include <Graphics/UBOBindings.hpp>

namespace Borealis
{
	struct Renderer3DData
	{
		Ref<Shader> mModelShader;
		Ref<Shader> mCommonShader;
		Ref<UniformBufferObject> mMaterialsUBO;
	};

	static std::unique_ptr<Renderer3DData> s3dData;
	LightEngine Renderer3D::mLightEngine;

	void Renderer3D::Init()
	{
		s3dData =  std::make_unique<Renderer3DData>();
		//s3dData->mModelShader = Shader::Create("engineResources/Shaders/Renderer3D_Material.glsl");
		s3dData->mModelShader = Shader::Create("engineResources/Shaders/Renderer3D_Material.glsl");

		s3dData->mCommonShader = Shader::Create("engineResources/Shaders/Renderer3D_Common.glsl");

		BOREALIS_CORE_INFO("Material UBO data cpu size, {}", sizeof(MaterialUBOData) * 128);

		s3dData->mMaterialsUBO = UniformBufferObject::Create(sizeof(MaterialUBOData) * 128, MATERIAL_ARRAY_BIND);

		UniformBufferObject::BindToShader(s3dData->mModelShader->GetID(), "Camera", CAMERA_BIND);
		UniformBufferObject::BindToShader(s3dData->mCommonShader->GetID(), "Camera", CAMERA_BIND);

		UniformBufferObject::BindToShader(s3dData->mModelShader->GetID(), "MaterialUBO", MATERIAL_ARRAY_BIND);
		UniformBufferObject::BindToShader(s3dData->mModelShader->GetID(), "LightsUBO", LIGHTING_BIND);
		UniformBufferObject::BindToShader(s3dData->mModelShader->GetID(), "AnimationUBO", ANIMATION_BIND);
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
		//s3dData->mModelShader->Bind();
		//s3dData->mModelShader->Set("u_ViewProjection", viewProj);

		//s3dData->mCommonShader->Bind();
		//s3dData->mCommonShader->Set("u_ViewProjection", viewProj);

		//if(shader)
		//{
		//	shader->Bind();
		//	shader->Set("u_ViewProjection", viewProj);
		//}
		mLightEngine.Begin();
	}

	bool DrawCallComparator(const Renderer3D::DrawCall& a, const Renderer3D::DrawCall& b) {
		if (a.shaderID != b.shaderID)
			return a.shaderID->GetID() < b.shaderID->GetID();
		if (a.materialHash != b.materialHash)
			return a.materialHash < b.materialHash;
		return a.entityID < b.entityID; // Fallback for stable sorting
	}

	void Renderer3D::End()
	{
		std::sort(drawQueue.begin(), drawQueue.end(), DrawCallComparator);

		UpdateMaterialUBO();

		for (DrawCall const& drawCall : drawQueue)
		{
			drawCall.shaderID->Bind();
			drawCall.shaderID->Set("materialIndex", materialMap[drawCall.materialHash]->GetIndex());
			drawCall.shaderID->Set("u_HasAnimation", false);
			drawCall.shaderID->Set("u_Transparent", false);

			auto const& textureMap = materialMap[drawCall.materialHash]->GetTextureMaps();

			int textureUnit = 2;

			if (textureMap.contains(Material::Albedo))
			{
				drawCall.shaderID->Set("albedoMap", textureUnit);
				textureMap.at(Material::Albedo)->Bind(textureUnit);
				textureUnit++;
			}
			if (textureMap.contains(Material::NormalMap))
			{
				drawCall.shaderID->Set("normalMap", textureUnit);
				textureMap.at(Material::NormalMap)->Bind(textureUnit);
				textureUnit++;
			}
			if (textureMap.contains(Material::Specular))
			{
				drawCall.shaderID->Set("specularMap", textureUnit);
				textureMap.at(Material::Specular)->Bind(textureUnit);
				textureUnit++;
			}
			if (textureMap.contains(Material::Metallic))
			{
				drawCall.shaderID->Set("metallicMap", textureUnit);
				textureMap.at(Material::Metallic)->Bind(textureUnit);
				textureUnit++;
			}

			if (std::holds_alternative<Ref<Model>>(drawCall.model))
			{
				std::get<Ref<Model>>(drawCall.model)->Draw(drawCall.transform, drawCall.shaderID, drawCall.entityID);
			}
			else
			{
				if (drawCall.drawData.hasAnimation)
				{
					drawCall.shaderID->Set("u_HasAnimation", true);
					drawCall.shaderID->Set("u_AnimationIndex", drawCall.drawData.animationIndex);
				}

				std::get<Ref<SkinnedModel>>(drawCall.model)->Draw(drawCall.transform, drawCall.shaderID, drawCall.entityID);
			}
		}

		drawQueue.clear();
	}

	void Renderer3D::RenderTransparentObjects(Ref<Shader> const& transparencyShader)
	{
		std::sort(drawQueueTransparent.begin(), drawQueueTransparent.end(), DrawCallComparator);

		UpdateMaterialUBO();

		for (DrawCall const& drawCall : drawQueueTransparent)
		{
			drawCall.shaderID->Bind();
			drawCall.shaderID->Set("materialIndex", materialMap[drawCall.materialHash]->GetIndex());
			drawCall.shaderID->Set("u_HasAnimation", false);
			drawCall.shaderID->Set("u_Transparent", true);

			auto const& textureMap = materialMap[drawCall.materialHash]->GetTextureMaps();

			int textureUnit = 2;

			if (textureMap.contains(Material::Albedo))
			{
				drawCall.shaderID->Set("albedoMap", textureUnit);
				textureMap.at(Material::Albedo)->Bind(textureUnit);
				textureUnit++;
			}
			if (textureMap.contains(Material::NormalMap))
			{
				drawCall.shaderID->Set("normalMap", textureUnit);
				textureMap.at(Material::NormalMap)->Bind(textureUnit);
				textureUnit++;
			}
			if (textureMap.contains(Material::Specular))
			{
				drawCall.shaderID->Set("specularMap", textureUnit);
				textureMap.at(Material::Specular)->Bind(textureUnit);
				textureUnit++;
			}
			if (textureMap.contains(Material::Metallic))
			{
				drawCall.shaderID->Set("metallicMap", textureUnit);
				textureMap.at(Material::Metallic)->Bind(textureUnit);
				textureUnit++;
			}

			if (std::holds_alternative<Ref<Model>>(drawCall.model))
			{
				std::get<Ref<Model>>(drawCall.model)->Draw(drawCall.transform, drawCall.shaderID, drawCall.entityID);
			}
			else
			{
				if (drawCall.drawData.hasAnimation)
				{
					drawCall.shaderID->Set("u_HasAnimation", true);
					drawCall.shaderID->Set("u_AnimationIndex", drawCall.drawData.animationIndex);
				}

				std::get<Ref<SkinnedModel>>(drawCall.model)->Draw(drawCall.transform, drawCall.shaderID, drawCall.entityID);
			}
		}

		drawQueueTransparent.clear();
		//mLightEngine.Begin();//clear vector
	}

	void Renderer3D::AddLight(LightComponent & lightComponent)
	{
		mLightEngine.AddLight(lightComponent);
	}

	void Renderer3D::SetLights(Ref<UniformBufferObject> const& LightsUBO)
	{
		mLightEngine.SetLights(LightsUBO);
	}

	void Renderer3D::DrawMesh(const glm::mat4& transform, const MeshFilterComponent& meshFilter, const MeshRendererComponent& meshRenderer, int entityID)
	{
		//SetLights(s3dData->mModelShader);
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
		//add material to container
		if (meshFilter.Model) 
		{
			AddToDrawQueue(meshFilter.Model, shader, meshRenderer.Material, entityID, transform);
		}
	}

	void Renderer3D::DrawHighlightedMesh(const glm::mat4& transform, const MeshFilterComponent& meshFilter, Ref<Shader> shader)
	{
		if (meshFilter.Model)
		{
			meshFilter.Model->Draw(transform, shader, -1, true);
		}
	}

	void Renderer3D::DrawSkinnedMesh(const glm::mat4& transform, const SkinnedMeshRendererComponent& skinnedMeshRenderer, Ref<Shader> shader, int entityID, int animationIndex)
	{
		if (skinnedMeshRenderer.SkinnnedModel)
		{
			//if (skinnedMeshRenderer.Material)
			//{
			//	skinnedMeshRenderer.Material->SetUniforms(shader);
			//}

			//skinnedMeshRenderer.SkinnnedModel->Draw(transform, shader, entityID);
			DrawData drawData;
			if (skinnedMeshRenderer.SkinnnedModel->mAnimation)
			{
				drawData.hasAnimation = true;
				drawData.animationIndex = animationIndex;
			}
			AddToDrawQueue(skinnedMeshRenderer.SkinnnedModel, shader, skinnedMeshRenderer.Material, entityID, transform, drawData);
		}
	}

	void Renderer3D::DrawQuad() //full screen quad
	{
		Mesh::DrawQuad();
	}

	void Renderer3D::DrawCubeMap()
	{
		Mesh::DrawCubeMap();
	}

	void Renderer3D::BeginCommonShapes(glm::mat4 const& viewProj)
	{
		//s3dData->mCommonShader->Bind();
		//s3dData->mCommonShader->Set("u_ViewProjection", viewProj);
		//s3dData->mCommonShader->Unbind();
	}

	void Renderer3D::DrawCube(glm::vec3 translation, glm::vec3 minExtent, glm::vec3 maxExtent, glm::vec4 color, bool wireframe)
	{
		Mesh::DrawCube(translation, minExtent, maxExtent, color, wireframe, s3dData->mCommonShader);
	}

	void Renderer3D::DrawSphere(glm::vec3 center, float radius, glm::vec4 color, bool wireframe)
	{
		Mesh::DrawSphere(center, radius, color, wireframe, s3dData->mCommonShader);
	}

	void Renderer3D::DrawCylinder(glm::vec3 center, float radius, float height, glm::vec4 color, bool wireframe)
	{
		Mesh::DrawCylinder(center, radius, height, color, wireframe, s3dData->mCommonShader);
	}

	void Renderer3D::DrawCapsule(glm::vec3 center, float radius, float height, glm::vec4 color, bool wireframe)
	{
		Mesh::DrawCapsule(center, radius, height, color, wireframe, s3dData->mCommonShader);
	}

	void Renderer3D::SetGlobalWireFrameMode(bool wireFrameMode)
	{
		mGlobalWireFrame = wireFrameMode;
	}

	bool Renderer3D::GetGlobalWireFrameMode()
	{
		return mGlobalWireFrame;
	}

	void Renderer3D::UpdateMaterialUBO()
	{
		if (mNewMaterialAdded)
		{
			mNewMaterialAdded = false;

			for (auto [hash, materialRef] : materialMap)
			{
				if (!materialUBODataMap.contains(hash))
				{
					MaterialUBOData uboData;

					uboData.albedoColor = materialRef->GetTextureMapColor()[Material::Albedo];
					uboData.specularColor = materialRef->GetTextureMapColor()[Material::Specular];
					uboData.emissionColor = materialRef->GetTextureMapColor()[Material::Emission];

					uboData.tiling = materialRef->GetPropertiesVec2()[Material::Tiling];
					uboData.offset = materialRef->GetPropertiesVec2()[Material::Offset];

					uboData.smoothness = materialRef->GetPropertiesFloats()[Material::Smoothness];
					uboData.shininess = materialRef->GetPropertiesFloats()[Material::Shininess];
					uboData.metallic = materialRef->GetTextureMapFloats()[Material::Metallic];

					auto const& textureMap = materialRef->GetTextureMaps();

					if (textureMap.contains(Material::Albedo))
					{
						uboData.hasAlbedoMap = true;
					}
					if (textureMap.contains(Material::NormalMap))
					{
						uboData.hasNormalMap = true;
					}
					if (textureMap.contains(Material::Specular))
					{
						uboData.hasSpecularMap = true;
					}
					if (textureMap.contains(Material::Metallic))
					{
						uboData.hasMetallicMap = true;
					}

					materialUBODataMap.insert({ hash, uboData });
				}
			}

			std::array<MaterialUBOData, 128> bufferUBOData;
			int i = 0;
			for (auto [hash, UBOData] : materialUBODataMap)
			{
				bufferUBOData[i] = UBOData;
				materialMap[hash]->SetIndex(i);
				i++;
			}

			s3dData->mMaterialsUBO->SetData(bufferUBOData.data(), sizeof(MaterialUBOData) * 128);
		}

		for (auto [hash, materialRef] : materialMap)
		{
			if (materialRef->isModified)
			{
				MaterialUBOData& uboData = materialUBODataMap[hash];

				uboData.albedoColor = materialRef->GetTextureMapColor()[Material::Albedo];
				uboData.specularColor = materialRef->GetTextureMapColor()[Material::Specular];
				uboData.emissionColor = materialRef->GetTextureMapColor()[Material::Emission];

				uboData.tiling = materialRef->GetPropertiesVec2()[Material::Tiling];
				uboData.offset = materialRef->GetPropertiesVec2()[Material::Offset];

				uboData.smoothness = materialRef->GetPropertiesFloats()[Material::Smoothness];
				uboData.shininess = materialRef->GetPropertiesFloats()[Material::Shininess];
				uboData.metallic = materialRef->GetTextureMapFloats()[Material::Metallic];

				auto const& textureMap = materialRef->GetTextureMaps();

				if (textureMap.contains(Material::Albedo))
				{
					uboData.hasAlbedoMap = true;
				}
				if (textureMap.contains(Material::NormalMap))
				{
					uboData.hasNormalMap = true;
				}
				if (textureMap.contains(Material::Specular))
				{
					uboData.hasSpecularMap = true;
				}
				if (textureMap.contains(Material::Metallic))
				{
					uboData.hasMetallicMap = true;
				}

				s3dData->mMaterialsUBO->SetData(&materialUBODataMap[hash], sizeof(MaterialUBOData), sizeof(MaterialUBOData) * materialRef->GetIndex());
				materialRef->isModified = false;
			}
		}
	}

	void Renderer3D::AddToDrawQueue(std::variant<Ref<Model>, Ref<SkinnedModel>> model, Ref<Shader> shaderID, Ref<Material> materialHash, uint32_t entityID, glm::mat4 const& transform, std::optional<DrawData> drawData)
	{
		if(!materialMap.contains(materialHash->hash))
		{
			materialMap.insert({ materialHash->hash, materialHash });
			mNewMaterialAdded = true;
		}
		if(drawData.has_value())
		{
			if(materialHash->isTransparent)
				drawQueueTransparent.push_back({ model, shaderID, materialHash->hash, entityID, transform, drawData.value() });
			else
				drawQueue.push_back({ model, shaderID, materialHash->hash, entityID, transform, drawData.value() });
		}
		else
		{
			if (materialHash->isTransparent)
				drawQueueTransparent.push_back({ model, shaderID, materialHash->hash, entityID, transform });
			else
				drawQueue.push_back({ model, shaderID, materialHash->hash, entityID, transform });
		}
	}
}