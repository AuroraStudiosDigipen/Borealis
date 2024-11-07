/******************************************************************************
/*!
\file       RuntimeLayer.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 19, 2024
\brief      Defines the Runtime Layer class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include <filesystem>
#include <RuntimeLayer.hpp>
#include <Scene/Serialiser.hpp>
namespace BorealisRuntime
{
	void RuntimeLayer::Init()
	{
		// Search for extension in current directory
		std::filesystem::path path = std::filesystem::current_path();
		std::string extension = ".brls";
		std::string projectPath = "";
		bool found = false;
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.path().extension() == extension)
			{
				if (found == true)
				{
					APP_LOG_WARN("Multiple Project Files found!", entry.path().string());
				}
				projectPath = entry.path().string();
				found = true;
			}
		}

		BOREALIS_CORE_ASSERT(found, "No Project File found!");

		std::string activeSceneName;
		if (Borealis::Project::SetProjectPath(projectPath, activeSceneName))
		{
			// Load Asset Registry here
			Borealis::AssetManager::SetRunTime();
			Borealis::Serialiser serialiser(nullptr);
			Borealis::SceneManager::SetActiveScene(activeSceneName, serialiser);
			Borealis::ScriptingSystem::InitCoreAssembly();

			auto view = Borealis::SceneManager::GetActiveScene()->GetRegistry().view<Borealis::CameraComponent>();
			for (auto entity : view)
			{
				auto& cameraComponent = view.get<Borealis::CameraComponent>(entity);
				cameraComponent.Camera.SetViewportSize(1920, 1080); // Initialize all viewport aspect ratios: Should be serialised from a setting file in the future
			}
		}


		Borealis::SceneManager::GetActiveScene()->RuntimeStart(); // Temporarily
	}
	void RuntimeLayer::UpdateFn(float dt)
	{
		Borealis::Renderer2D::ResetStats();
		Borealis::RenderCommand::Clear();
		Borealis::RenderCommand::SetClearColor({ 0.f, 0.0f, 0.0f, 1 });
		Borealis::SceneManager::GetActiveScene()->UpdateRuntime(dt);
	}	
	void RuntimeLayer::Free()
	{
	}
}

