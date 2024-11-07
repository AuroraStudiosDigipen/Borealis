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

#include <Graphics/RenderGraph/RenderGraph.hpp>

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
			Borealis::SceneManager::SetActiveScene(activeSceneName);
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
		//set render graph config manually for now
		{
			Borealis::RenderGraphConfig fconfig;
			Borealis::RenderPassConfig shadowPass(Borealis::RenderPassType::Shadow, "ShadowPass");
			shadowPass.AddSinkLinkage("shadowMap", "ShadowMapBuffer");
			shadowPass.AddSinkLinkage("camera", "RunTimeCamera");
			fconfig.AddPass(shadowPass);

			Borealis::RenderPassConfig Render3D(Borealis::RenderPassType::Render3D, "Render3D");
			Render3D.AddSinkLinkage("renderTarget", "RunTimeBuffer");
			Render3D.AddSinkLinkage("shadowMap", "ShadowPass.shadowMap");
			Render3D.AddSinkLinkage("camera", "RunTimeCamera");
			fconfig.AddPass(Render3D);

			Borealis::RenderPassConfig Render2D(Borealis::RenderPassType::Render2D, "Render2D");
			Render2D.AddSinkLinkage("renderTarget", "Render3D.renderTarget");
			Render2D.AddSinkLinkage("camera", "RunTimeCamera");
			fconfig.AddPass(Render2D);

			Borealis::SceneManager::GetActiveScene()->SetRenderGraphConfig(fconfig);
		}
		Borealis::SceneManager::GetActiveScene()->UpdateRenderer(dt);
		//Borealis::SceneManager::GetActiveScene()->GetRunTimeFB()->Blit(0, mRuntimeFrameBuffer->GetProperties());
	}	
	void RuntimeLayer::Free()
	{
	}
}

