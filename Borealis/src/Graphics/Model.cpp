/******************************************************************************/
/*!
\file		Model.cpp
\author 	Chan Guo Geng Gordon
\par    	email: g.chan\@digipen.edu
\date   	September 12, 2024
\brief		Declares the class for Model for rendering

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/SkinnedModel.hpp>

#include <Core/LoggerSystem.hpp>
#include <Assets/AssetManager.hpp>
namespace Borealis
{
	void Model::Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID, bool posOnly)
	{
		for (auto mesh : mMeshes)
		{
			mesh.Draw(transform, shader, entityID, posOnly);
		}
	}

	void Model::LoadModel(std::filesystem::path const& path)
	{
		std::stringstream inFile;
		if (AssetManager::IsPakLoaded())
		{
			std::string subPath = path.filename().string();
			uint64_t id = std::stoull(subPath);
			char* buffer;
			uint64_t size;
			AssetManager::RetrieveFromPak(id, buffer, size);
			inFile << std::string(buffer, size);
			delete[] buffer;
		}
		else
		{
			std::ifstream actualFile(path, std::ios::binary);
			inFile << actualFile.rdbuf();
			actualFile.close();
		}

		

		uint32_t meshCount;
		inFile.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));

		mMeshes.resize(meshCount);

		//for (Mesh& mesh : mMeshes) 
		//{
		//	uint32_t verticesCount, indicesCount;
		//	inFile.read(reinterpret_cast<char*>(&verticesCount), sizeof(verticesCount));
		//	mesh.SetVerticesCount(verticesCount);
		//	inFile.read(reinterpret_cast<char*>(&indicesCount), sizeof(indicesCount));
		//	mesh.SetIndicesCount(indicesCount);

		//	std::vector<MeshVertex> oldVertex(verticesCount);
		//	std::vector<unsigned int> oldIndices(indicesCount);

		//	//mesh.GetVertices().resize(verticesCount);
		//	//mesh.GetIndices().resize(indicesCount);

		//	inFile.read(reinterpret_cast<char*>(oldVertex.data()), verticesCount * sizeof(MeshVertex));

		//	inFile.read(reinterpret_cast<char*>(oldIndices.data()), indicesCount * sizeof(uint32_t));

		//	mesh.GetVertices().resize(verticesCount);
		//	std::vector<Vertex>& meshData = mesh.GetVertices();
		//	for (size_t i{}; i < oldVertex.size(); ++i)
		//	{
		//		meshData[i].Position = oldVertex[i].Position;
		//		meshData[i].Normal = oldVertex[i].Normal;
		//		meshData[i].TexCoords = oldVertex[i].TexCoords;
		//	}
		//	
		//	mesh.GetIndices() = oldIndices;

		//	mesh.SetupMesh();

		//	mesh.GenerateRitterBoundingSphere();
		//	mesh.GenerateAABB();
		//}

		for (Mesh& mesh : mMeshes)
		{
			uint32_t vertexCount, indexCount;
			inFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
			inFile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

			mesh.GetPosition().resize(vertexCount);
			mesh.GetNormal().resize(vertexCount);
			mesh.GetTexCoord().resize(vertexCount);
			mesh.GetIndices().resize(indexCount);

			inFile.read(reinterpret_cast<char*>(mesh.GetPosition().data()), vertexCount * sizeof(glm::vec3));
			inFile.read(reinterpret_cast<char*>(mesh.GetNormal().data()), vertexCount * sizeof(glm::vec3));
			inFile.read(reinterpret_cast<char*>(mesh.GetTexCoord().data()), vertexCount * sizeof(glm::vec2));
			inFile.read(reinterpret_cast<char*>(mesh.GetIndices().data()), indexCount * sizeof(uint32_t));

			mesh.SetupMesh();

			mesh.GenerateRitterBoundingSphere();
			mesh.GenerateAABB();
		}

		GenerateRitterBoundingSphere();
		GenerateAABB();
	}

	void Model::SaveModel()
	{
		//std::ofstream outFile("model.mesh", std::ios::binary);

		//uint32_t meshCount = static_cast<uint32_t>(mMeshes.size());
		//outFile.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

		//for (const Mesh& mesh : mMeshes) {
		//	// Write mesh header (vertexCount, indexCount, normalCount, texCoordCount)
		//	uint32_t verticesCount = mesh.GetVerticesCount();
		//	uint32_t indicesCount = mesh.GetIndicesCount();
		//	outFile.write(reinterpret_cast<const char*>(&verticesCount), sizeof(verticesCount));
		//	outFile.write(reinterpret_cast<const char*>(&indicesCount), sizeof(indicesCount));

		//	// Write vertices
		//	outFile.write(reinterpret_cast<const char*>(mesh.GetVertices().data()), verticesCount * sizeof(Vertex));

		//	// Write indices
		//	outFile.write(reinterpret_cast<const char*>(mesh.GetIndices().data()), indicesCount * sizeof(uint32_t));
		//}

		//outFile.close();
	}

	void Model::GenerateRitterBoundingSphere()
	{
		if (mMeshes.empty()) return;

		// Start with the bounding sphere of the first mesh
		BoundingSphere modelSphere = mMeshes[0].GetBoundingSphere();

		for (size_t i = 1; i < mMeshes.size(); ++i)
		{
			const BoundingSphere& meshSphere = mMeshes[i].GetBoundingSphere();

			// Compute the distance between model sphere center and current mesh sphere center
			glm::vec3 direction = meshSphere.Center - modelSphere.Center;
			float distBetweenCenters = glm::length(direction);

			// Check if the mesh sphere is already within the current model sphere
			if (distBetweenCenters + meshSphere.Radius <= modelSphere.Radius)
				continue;

			// If the current model sphere fully encompasses the mesh sphere, skip
			if (distBetweenCenters + modelSphere.Radius <= meshSphere.Radius)
			{
				modelSphere.Center = meshSphere.Center;
				modelSphere.Radius = meshSphere.Radius;
				continue;
			}

			// Expand model sphere to include the mesh sphere
			float newRadius = (distBetweenCenters + modelSphere.Radius + meshSphere.Radius) / 2.0f;
			glm::vec3 offset = direction / distBetweenCenters * (newRadius - modelSphere.Radius);

			modelSphere.Center += offset;
			modelSphere.Radius = newRadius;
		}

		mBoundingSphere = modelSphere;
	}

	void Model::GenerateAABB()
	{
		if (mMeshes.empty()) return;

		AABB aabb = mMeshes[0].GetAABB();

		// Expand the AABB to include each mesh's AABB
		for (size_t i = 1; i < mMeshes.size(); ++i)
		{
			const AABB& meshAABB = mMeshes[i].GetAABB();

			// Update min extent
			aabb.minExtent.x = std::min(aabb.minExtent.x, meshAABB.minExtent.x);
			aabb.minExtent.y = std::min(aabb.minExtent.y, meshAABB.minExtent.y);
			aabb.minExtent.z = std::min(aabb.minExtent.z, meshAABB.minExtent.z);

			// Update max extent
			aabb.maxExtent.x = std::max(aabb.maxExtent.x, meshAABB.maxExtent.x);
			aabb.maxExtent.y = std::max(aabb.maxExtent.y, meshAABB.maxExtent.y);
			aabb.maxExtent.z = std::max(aabb.maxExtent.z, meshAABB.maxExtent.z);
		}

		// Now `aabb` represents the AABB that encloses all meshes in the model
		mAABB = aabb;
	}

	Ref<Asset> Model::Load(std::filesystem::path const& cachePath, AssetMetaData const& assetMetaData)
	{
		MeshConfig config = GetConfig<MeshConfig>(assetMetaData.Config);
		if (config.skinMesh)
		{
			SkinnedModel skinnedModel;
			skinnedModel.LoadModel(cachePath/std::to_string(assetMetaData.Handle));
			return MakeRef<SkinnedModel>(skinnedModel);
		}
		else
		{
			Model model;
			model.LoadModel(cachePath/std::to_string(assetMetaData.Handle));
			return MakeRef<Model>(model);
		}
	}

	void Model::Reload(AssetMetaData const& assetMetaData, Ref<Asset> asset)
	{
		MeshConfig config = GetConfig<MeshConfig>(assetMetaData.Config);
		if (config.skinMesh)
		{
			SkinnedModel skinnedModel;
			skinnedModel.LoadModel(assetMetaData.CachePath);
			auto newModel = MakeRef<SkinnedModel>(skinnedModel);
			newModel->mAssetHandle = assetMetaData.Handle;
			newModel->swap(*asset);
		}
		else
		{
			Model model;
			model.LoadModel(assetMetaData.CachePath);
			auto newModel = MakeRef<Model>(model);
			newModel->mAssetHandle = assetMetaData.Handle;
			newModel->swap(*asset);
		}
	}

	void Model::swap(Asset& other)
	{
		Model& otherModel = dynamic_cast<Model&>(other);
		std::swap(mMeshes, otherModel.mMeshes);
		std::swap(mBoundingSphere, otherModel.mBoundingSphere);
		std::swap(mAABB, otherModel.mAABB);
	}

	
}