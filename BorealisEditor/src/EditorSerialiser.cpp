/******************************************************************************
/*!
\file       EditorSerialiser.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <EditorSerialiser.hpp>
#include <Scene/Entity.hpp>
#include <PrefabComponent.hpp>
#include <Scene/SerialiserUtils.hpp>
#include <Scene/SceneManager.hpp>
namespace Borealis
{
	void EditorSerialiser::SerialiseAbstractItems(YAML::Emitter& out, Entity& entity)
	{
		if (entity.HasComponent<PrefabComponent>())
		{
			SerializeComponent(out, entity.GetComponent<PrefabComponent>());
		}
	}
	void EditorSerialiser::DeserialiseAbstractItems(YAML::detail::iterator_value& data, Entity& entity)
	{
		DeserialiseComponent<PrefabComponent>(data, entity);
	}
	void EditorSerialiser::SerializeEditorCameraProp(EditorCamera camera, std::string filepath)
	{
		std::ifstream inFile(filepath);
		YAML::Node data = YAML::Load(inFile);
		inFile.close();

		if (!data["Cameras"] || !data["Cameras"].IsSequence()) {
			return;  // Ensure the structure exists
		}

		std::string scenePath = SceneManager::GetActiveScene()->GetScenePath();
		EditorCameraProps prop = camera.GetProperties();

		bool updated = false;
		for (auto cameraNode : data["Cameras"])
		{
			if (cameraNode[scenePath])  // Find existing entry
			{
				auto node = cameraNode[scenePath];
				node["Position"] = prop.mPosition;
				node["Pitch"] = prop.mPitch;
				node["Yaw"] = prop.mYaw;
				node["FocalPoint"] = prop.mFocalPoint;
				node["Distance"] = prop.mDistance;
				updated = true;
				break;
			}
		}

		// If no existing entry, add a new one
		if (!updated) {
			YAML::Node newCamera;
			newCamera[scenePath]["Position"] = prop.mPosition;
			newCamera[scenePath]["Pitch"] = prop.mPitch;
			newCamera[scenePath]["Yaw"] = prop.mYaw;
			newCamera[scenePath]["FocalPoint"] = prop.mFocalPoint;
			newCamera[scenePath]["Distance"] = prop.mDistance;
			data["Cameras"].push_back(newCamera);
		}

		// Write back to file
		std::ofstream outFile(filepath);
		outFile << data;
		outFile.close();

	}
	void EditorSerialiser::DeserializeEditorCameraProp(EditorCamera& camera, std::string filepath)
	{

		std::filesystem::path path = filepath;
		if (path.empty() || !std::filesystem::exists(path)) return;
		EditorCameraProps prop = camera.GetProperties();
		std::ifstream inStream(filepath);
		std::stringstream ss;
		ss << inStream.rdbuf();
		inStream.close();
		auto name = SceneManager::GetActiveScene()->GetScenePath();
		YAML::Node data = YAML::Load(ss.str());
		auto cameras = data["Cameras"];
		for (auto camera : cameras)
		{
			if (camera[name])  // Check if the scene path exists
			{
				auto node = camera[name];  // Get the nested properties
				prop.mPosition = node["Position"].as<glm::vec3>();
				prop.mPitch = node["Pitch"].as<float>();
				prop.mYaw = node["Yaw"].as<float>();
				prop.mFocalPoint = node["FocalPoint"].as<glm::vec3>();
				prop.mDistance = node["Distance"].as<float>();
				break;
			}
		}
		camera.SetProperties(prop);
	}
}

