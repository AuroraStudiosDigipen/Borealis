/******************************************************************************/
/*!
\file		ImGuiLayer.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 19, 2024
\brief		Defines the ImGui Layer class that will be used to render the ImGui
			elements on the screen.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <Core/ApplicationManager.hpp>
#include <ImGui/ImGuiLayer.hpp>
#include <ImGui/ImGuiFontLib.hpp>
#include <Scene/Serialiser.hpp>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <ImGuizmo.h>

#include <Graphics/RenderCommand.hpp>
namespace Borealis
{ 
	/*!***********************************************************************
		\brief
			Constructor for ImGuiLayer.
	*************************************************************************/
#ifndef _DIST
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer"), mTime(0.0f)
	{
	}
#else
	ImGuiLayer::ImGuiLayer() : mTime(0.0f)
	{
	}
#endif

	/*!***********************************************************************
		\brief
			Destructor for ImGuiLayer.
	*************************************************************************/
	ImGuiLayer::~ImGuiLayer()
	{
	}

	/*!***********************************************************************
		\brief
			Initializes the ImGuiLayer.
	*************************************************************************/
	void ImGuiLayer::Init()
	{
		PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		for (auto& font : sImGuiFontLib)
		{
			io.Fonts->AddFontFromFileTTF(font.second, 18.f);
		}
		// Set default font
		io.FontDefault = io.Fonts->Fonts[ImGuiFonts::regular];

		// Setup Dear ImGui style
		//ImGui::StyleColorsDark();
		ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ApplicationManager& app = ApplicationManager::Get();
		GLFWwindow* window = (GLFWwindow*)app.GetWindow()->GetNativeWindow();

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		io.FontGlobalScale = static_cast<float>(width) / 1920.f;


		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");

		auto& imGuizmoStyle = ImGuizmo::GetStyle();

		imGuizmoStyle.CenterCircleSize = 8.0f;
		imGuizmoStyle.HatchedAxisLineThickness = 8.0f;
		imGuizmoStyle.RotationLineThickness = 3.0f;
		imGuizmoStyle.RotationOuterLineThickness = 4.0f;
		imGuizmoStyle.ScaleLineThickness = 4.0f; 
		imGuizmoStyle.ScaleLineCircleSize = 8.0f;
		imGuizmoStyle.TranslationLineThickness = 4.0f;
		imGuizmoStyle.TranslationLineArrowSize = 8.0f;

		imGuizmoStyle.Colors[ImGuizmo::DIRECTION_X] = ImVec4(0.666f, 0.200f, 0.200f, 1.000f);
		imGuizmoStyle.Colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.200f, 0.666f, 0.200f, 1.000f);
		imGuizmoStyle.Colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.200f, 0.200f, 0.666f, 1.000f);
		imGuizmoStyle.Colors[ImGuizmo::PLANE_X] = ImVec4(0.666f, 0.200f, 0.200f, 0.380f);
		imGuizmoStyle.Colors[ImGuizmo::PLANE_Y] = ImVec4(0.200f, 0.666f, 0.200f, 0.380f);
		imGuizmoStyle.Colors[ImGuizmo::PLANE_Z] = ImVec4(0.200f, 0.200f, 0.666f, 0.380f);
		imGuizmoStyle.Colors[ImGuizmo::SELECTION] = ImVec4(1.000f, 0.900f, 0.062f, 0.841f);
		imGuizmoStyle.Colors[ImGuizmo::INACTIVE] = ImVec4(0.600f, 0.600f, 0.600f, 0.600f);
		imGuizmoStyle.Colors[ImGuizmo::TRANSLATION_LINE] = ImVec4(0.666f, 0.666f, 0.666f, 0.666f);
		imGuizmoStyle.Colors[ImGuizmo::SCALE_LINE] = ImVec4(0.250f, 0.250f, 0.250f, 1.000f);
		imGuizmoStyle.Colors[ImGuizmo::ROTATION_USING_BORDER] = ImVec4(1.000f, 0.500f, 0.062f, 1.000f);
		imGuizmoStyle.Colors[ImGuizmo::ROTATION_USING_FILL] = ImVec4(1.000f, 0.500f, 0.062f, 0.500f);
		imGuizmoStyle.Colors[ImGuizmo::HATCHED_AXIS_LINES] = ImVec4(0.000f, 0.000f, 0.000f, 0.500f);
		imGuizmoStyle.Colors[ImGuizmo::TEXT] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
		imGuizmoStyle.Colors[ImGuizmo::TEXT_SHADOW] = ImVec4(0.000f, 0.000f, 0.000f, 1.000f);
	}

	/*!***********************************************************************
		\brief
			Frees the ImGuiLayer.
	*************************************************************************/
	void ImGuiLayer::Free()
	{
		PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::BeginFrame()
	{
		PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::EndFrame()
	{
		PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		ApplicationManager& app = ApplicationManager::Get();
		io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow()->GetWidth()), static_cast<float>(app.GetWindow()->GetHeight()));

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		RenderCommand::GetError({});

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::ImGuiRender(float dt)
	{

	}

	void ImGuiLayer::EventFn(Event& e)
	{
		if (mBlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.isDoneHandling |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.isDoneHandling |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}

	}

	void ImGuiLayer::OnWindowResize(float width, float height)
	{
		if (width > height)
		{
			ImGui::GetIO().FontGlobalScale = width / 1920.f;
		}
		else
		{
			ImGui::GetIO().FontGlobalScale = height / 1080.f;
		}
	}

	void ImGuiLayer::SetDarkThemeColours()
	{
		ImGui::StyleColorsDark();
		auto& colors = ImGui::GetStyle().Colors;
		for (auto& sColor : sImGuiDarkColours)
		{
			colors[sColor.first] = sColor.second;
		}
	}

	void ImGuiLayer::SetLightThemeColours()
	{
		auto& colors = ImGui::GetStyle().Colors;
		for (auto& sColor : sImGuiLightColours)
		{
			colors[sColor.first] = sColor.second;
		}
	}
}