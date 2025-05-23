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
#include <Scene/ComponentRegistry.hpp>
#include <Scene/Serialiser.hpp>
#include <Graphics/UI/Button.hpp>
#include <Graphics/Font.hpp>
#include <Windows.h>
#include <Core/TimeManager.hpp>

#define HIDE_CONSOLE 1

namespace BorealisRuntime
{
	void RuntimeLayer::Init()
	{

		if (HIDE_CONSOLE)
		{
			HWND hwnd = GetConsoleWindow();
			ShowWindow(hwnd, SW_HIDE);
		}
		// Retrieve the list of component names
		std::vector<std::string> properties = Borealis::ComponentRegistry::GetComponentNames();

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
		if (Borealis::Project::SetProjectPath(projectPath, activeSceneName, true))
		{
			// Load Asset Registry here
			Borealis::AssetManager::SetRunTime();
			Borealis::Serialiser serialiser(nullptr);
			Borealis::SceneManager::SetActiveScene(activeSceneName, serialiser, true);

			auto view = Borealis::SceneManager::GetActiveScene()->GetRegistry().view<Borealis::CameraComponent>();
			for (auto entity : view)
			{
				auto& cameraComponent = view.get<Borealis::CameraComponent>(entity);
				cameraComponent.Camera.SetViewportSize(1920, 1080); // Initialize all viewport aspect ratios: Should be serialised from a setting file in the future
			}
		}

		Borealis::ApplicationManager::Get().GetWindow()->ToggleFullScreen();

		Borealis::SceneManager::GetActiveScene()->RuntimeStart(); // Temporarily

		//TEMP
		{
			Borealis::Font font(std::filesystem::path("engineResources/fonts/aseprite.bfi"));
			font.SetTexture(std::filesystem::path("engineResources/fonts/aseprite.dds"));
			Borealis::Font::SetDefaultFont(MakeRef<Borealis::Font>(font));
		}
	}
	void RuntimeLayer::UpdateFn(float dt)
	{
		int windowWidth = Borealis::ApplicationManager::Get().GetWindow()->GetWidth();
		int windowHeight = Borealis::ApplicationManager::Get().GetWindow()->GetHeight();
		if (Borealis::FrameBufferProperties spec = Borealis::SceneManager::GetActiveScene()->GetRunTimeFB()->GetProperties();
			windowWidth > 0.0f && windowHeight > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != windowWidth || spec.Height != windowHeight))
		{
			Borealis::SceneManager::GetActiveScene()->GetRunTimeFB()->Resize((uint32_t)windowWidth, (uint32_t)windowHeight);
		}

		float normalizedMouseX = Borealis::InputSystem::GetMouseX() / (float)windowWidth - 0.5f;
		float normalizedMouseY = Borealis::InputSystem::GetMouseY() / (float)windowHeight - 0.5f;
		Borealis::ButtonSystem::SetMousePos({ normalizedMouseX, normalizedMouseY });
		Borealis::InputSystem::SetMouseViewport(std::make_pair(normalizedMouseX, normalizedMouseY));
		Borealis::SceneManager::GetActiveScene()->UpdateRuntime(dt);

		Borealis::Renderer2D::ResetStats();

		Borealis::SceneManager::GetActiveScene()->GetRunTimeFB()->Bind();
		Borealis::RenderCommand::Clear();
		Borealis::RenderCommand::SetClearColor({ 0.f, 0.0f, 0.0f, 1 });
		Borealis::SceneManager::GetActiveScene()->GetRunTimeFB()->Unbind();

		Borealis::RenderCommand::BindBackBuffer();
		Borealis::RenderCommand::Clear();

		Borealis::SceneManager::GetActiveScene()->SetRunTimeViewPort(true);
		Borealis::SceneManager::GetActiveScene()->SetRunTimeRenderPass();
		Borealis::RenderGraphConfig fconfig = Borealis::SceneManager::GetActiveScene()->GetRenderGraphConfig();
		////set render graph config manually for now
		//{
		//	Borealis::RenderGraphConfig fconfig;
		//	Borealis::RenderPassConfig SkyBoxPass(Borealis::RenderPassType::SkyboxPass, "SkyBox");
		//	SkyBoxPass.AddSinkLinkage("renderTarget", "RunTimeBuffer");
		//	SkyBoxPass.AddSinkLinkage("camera", "RunTimeCamera");
		//	fconfig.AddPass(SkyBoxPass);

		//	Borealis::RenderPassConfig shadowPass(Borealis::RenderPassType::Shadow, "ShadowPass");
		//	shadowPass.AddSinkLinkage("shadowMap", "ShadowMapBuffer");
		//	shadowPass.AddSinkLinkage("camera", "RunTimeCamera");
		//	fconfig.AddPass(shadowPass);

		//	Borealis::RenderPassConfig Render3D(Borealis::RenderPassType::Render3D, "Render3D");
		//	Render3D.AddSinkLinkage("renderTarget", "RunTimeBuffer");
		//	Render3D.AddSinkLinkage("accumulaionTarget", "accumulaionBuffer");
		//	Render3D.AddSinkLinkage("shadowMap", "ShadowPass.shadowMap");
		//	Render3D.AddSinkLinkage("camera", "RunTimeCamera");
		//	fconfig.AddPass(Render3D);

		//	Borealis::RenderPassConfig Render2D(Borealis::RenderPassType::Render2D, "Render2D");
		//	Render2D.AddSinkLinkage("renderTarget", "Render3D.renderTarget");
		//	Render2D.AddSinkLinkage("camera", "RunTimeCamera");
		//	fconfig.AddPass(Render2D);

		//	Borealis::RenderPassConfig UIWorldPass(Borealis::RenderPassType::UIWorldPass, "UIWorldPass");
		//	UIWorldPass.AddSinkLinkage("renderTarget", "Render2D.renderTarget");
		//	UIWorldPass.AddSinkLinkage("camera", "RunTimeCamera");
		//	fconfig.AddPass(UIWorldPass);

		//	Borealis::RenderPassConfig particleSystemPass(Borealis::RenderPassType::ParticleSystemPass, "ParticleSystem");
		//	particleSystemPass.AddSinkLinkage("camera", "RunTimeCamera");
		//	particleSystemPass.AddSinkLinkage("accumulaionTarget", "accumulaionBuffer");
		//	particleSystemPass.AddSinkLinkage("renderTarget", "UIWorldPass.renderTarget");
		//	fconfig.AddPass(particleSystemPass);

		//	Borealis::RenderPassConfig RunTimeHighlight(Borealis::RenderPassType::HighlightPass, "RunTimeHighlight");
		//	RunTimeHighlight.AddSinkLinkage("camera", "RunTimeCamera");
		//	RunTimeHighlight.AddSinkLinkage("renderTarget", "Render2D.renderTarget");
		//	fconfig.AddPass(RunTimeHighlight);

		//	Borealis::RenderPassConfig UIPass(Borealis::RenderPassType::UIPass, "UIPass");
		//	UIPass.AddSinkLinkage("renderTarget", "RunTimeHighlight.renderTarget");
		//	UIPass.AddSinkLinkage("camera", "RunTimeCamera");
		//	fconfig.AddPass(UIPass);

		Borealis::RenderPassConfig backBuffer(Borealis::RenderPassType::RenderToTarget, "BackBuffer");
		backBuffer.AddSinkLinkage("renderSource", "RunTimeBuffer");
		fconfig.AddPass(backBuffer);

		Borealis::SceneManager::GetActiveScene()->SetRenderGraphConfig(fconfig);
		Borealis::SceneManager::GetActiveScene()->UpdateRenderer(dt);

		if (Borealis::SceneManager::ToNextScene)
		{
			ULONGLONG currTime = GetTickCount64();
			Borealis::SceneManager::GetActiveScene()->RuntimeEnd();
			Borealis::Serialiser serialiser(nullptr);
			Borealis::SceneManager::SetActiveScene(Borealis::SceneManager::NextSceneName, serialiser, true);
			Borealis::SceneManager::ToNextScene = false;
			Borealis::SceneManager::NextSceneName = "";
			ULONGLONG deltaTime = GetTickCount64() - currTime;
			float loadDt = static_cast<float>(deltaTime) / 1000.0f;
			loadDt -= Borealis::TimeManager::GetDeltaTime();
			Borealis::TimeManager::SetDeltaTime(loadDt);
			Borealis::SceneManager::GetActiveScene()->RuntimeStart();
		}

		if (!Borealis::ApplicationManager::Get().GetIsRunning())
		{
			Borealis::SceneManager::GetActiveScene()->RuntimeEnd();
		}
	}	
	void RuntimeLayer::Free()
	{
	}
}

