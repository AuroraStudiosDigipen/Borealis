/******************************************************************************/
/*!
\file		EditorLayer.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 11, 2024
\brief		Defines the class for the layer for level editor

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/


#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/ImGuiLayer.hpp>
#include <Core/Project.hpp>
#include <Scene/SceneManager.hpp>
#include <Scene/Serialiser.hpp>	
#include <Scene/ComponentRegistry.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <EditorLayer.hpp>
#include <Prefab.hpp>
//	#include <Project/Project.hpp>
#include "Audio/AudioEngine.hpp"
#include <ResourceManager.hpp>

#include <Assets/AssetManager.hpp>
#include <Graphics/Font.hpp>

#include <EditorAssets/AssetImporter.hpp>
#include <EditorAssets/FontImporter.hpp>
#include <EditorAssets/MeshImporter.hpp>
//#include <Assets/FontImporter.hpp>
#include <AI/BehaviourTree/RegisterNodes.hpp>
#include <AI/BehaviourTree/BehaviourTree.hpp>

namespace Borealis {
	EditorLayer::SceneState EditorLayer::mSceneState = EditorLayer::SceneState::Edit;
#ifndef _DIST
	EditorLayer::EditorLayer() : Layer("EditorLayer"), mCamera(1280.0f / 720.0f)
	{
	}
#else
	EditorLayer::EditorLayer() : mCamera(1280.0f / 720.0f)
	{
	}
#endif

	void EditorLayer::Init()
	{

		if (Serialiser::DeserialiseEditorStyle())
		{
			ImGuiLayer::SetLightThemeColours();
		}

		PROFILE_FUNCTION();

		FrameBufferProperties props{ 1280, 720, false };
		props.Attachments = { FramebufferTextureFormat::RGBA8,  FramebufferTextureFormat::RedInteger, FramebufferTextureFormat::Depth };
		mViewportFrameBuffer = FrameBuffer::Create(props);
		
		FrameBufferProperties propsRuntime{ 1280, 720, false };
		propsRuntime.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RedInteger,FramebufferTextureFormat::Depth };
		mRuntimeFrameBuffer = FrameBuffer::Create(propsRuntime);

		mEditorScene = MakeRef<Scene>();
		SceneManager::AddScene(mEditorScene->GetName(), mEditorScene->GetScenePath());
		SceneManager::SetActiveScene(mEditorScene->GetName());
		mEditorScene = SceneManager::GetActiveScene();

		SCPanel.SetContext(SceneManager::GetActiveScene());

		mEditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		ScriptingSystem::InitCoreAssembly();
		ResourceManager::Init();

		
		//TEMP
		{
			Font font(std::filesystem::path("engineResources/fonts/OpenSans_Condensed-Bold.bfi"));
			font.SetTexture(std::filesystem::path("engineResources/fonts/OpenSans_Condensed-Bold.dds"));
			Font::SetDefaultFont(MakeRef<Font>(font));
		}
	}

	void EditorLayer::Free()
	{
		PROFILE_FUNCTION();
		ResourceManager::Free();

	}

	void EditorLayer::UpdateFn(float dt)
	{
		PROFILE_FUNCTION();
		if (Borealis::FrameBufferProperties spec = mViewportFrameBuffer->GetProperties();
			mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != mViewportSize.x || spec.Height != mViewportSize.y))
		{
			mViewportFrameBuffer->Resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mEditorCamera.SetViewportSize(mViewportSize.x, mViewportSize.y);
			SceneManager::GetActiveScene()->ResizeViewport((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		}

		if (Borealis::FrameBufferProperties spec = mRuntimeFrameBuffer->GetProperties();
			mRuntimeSize.x > 0.0f && mRuntimeSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != mRuntimeSize.x || spec.Height != mRuntimeSize.y))
		{
			mRuntimeFrameBuffer->Resize((uint32_t)mRuntimeSize.x, (uint32_t)mRuntimeSize.y);
			if (hasRuntimeCamera)
			{
				mRuntimeCamera.GetComponent<CameraComponent>().Camera.SetViewportSize(static_cast<uint32_t>(mRuntimeSize.x), static_cast<uint32_t>(mRuntimeSize.y));
			}
		}


		if (mViewportHovered)
		{
			mCamera.UpdateFn(dt);
			mEditorCamera.UpdateFn(dt);
		}


		Renderer2D::ResetStats();
		{
			PROFILE_SCOPE("Renderer::Prep");
			mViewportFrameBuffer->Bind();
			RenderCommand::Clear();
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			mViewportFrameBuffer->Unbind();

			mRuntimeFrameBuffer->Bind();
			RenderCommand::Clear();
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			mRuntimeFrameBuffer->Unbind();
		}
		mViewportFrameBuffer->ClearAttachment(1, -1);
		{
			PROFILE_SCOPE("Renderer::Draw");
			mViewportFrameBuffer->Bind();
			SceneManager::GetActiveScene()->UpdateEditor(dt,mEditorCamera);

			auto[mx,my] = ImGui::GetMousePos();
			mx -= mViewportBounds[0].x;
			my -= mViewportBounds[0].y;
			glm::vec2 viewportSize { mViewportBounds[1].x - mViewportBounds[0].x, mViewportBounds[1].y - mViewportBounds[0].y };
			my = viewportSize.y - my;

			int mouseX = (int)mx;
			int mouseY = (int)my;

			if (mViewportHovered)
			{
				if (mViewportFrameBuffer->ReadPixel(1, mouseX, mouseY) != -1)
				{
					//int id_ent = mViewportFrameBuffer->ReadPixel(1, mouseX, mouseY);
					mHoveredEntity = { (entt::entity)mViewportFrameBuffer->ReadPixel(1, mouseX, mouseY), SceneManager::GetActiveScene().get()};
					//BOREALIS_CORE_INFO("picking id {}", id_ent);
					//BOREALIS_CORE_INFO("Name : {}", mHoveredEntity.GetName());
				}
				else
				{
					mHoveredEntity = {};
				}
			}
			mViewportFrameBuffer->Unbind();

			mRuntimeFrameBuffer->Bind();
			SceneManager::GetActiveScene()->UpdateRuntime(dt);
			mRuntimeFrameBuffer->Unbind();
		}
	}

	void EditorLayer::EventFn(Event& e)
	{
		PROFILE_FUNCTION();
		mCamera.EventFn(e);
		mEditorCamera.EventFn(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<EventKeyPressed>(BIND_EVENT(EditorLayer::onKeyPressed));
		dispatcher.Dispatch<EventMouseButtonTriggered>(BIND_EVENT(EditorLayer::onMousePressed));
	}

	void EditorLayer::ImGuiRender(float dt)
	{
		PROFILE_FUNCTION();

		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		bool p_open = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &p_open, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// Submit the DockSpace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}


			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{

					if (ImGui::MenuItem("New Project...","Ctrl+N")) {
						NewProject();
						SaveProject();
					}

					if (ImGui::MenuItem("Open Project...","Ctrl+O")) {
						LoadProject();
					}

					if (ImGui::MenuItem("Save Project...","Ctrl+S")) {
						SaveProject();
					}

					if (ImGui::MenuItem("Build Project"))
					{
						BuildProject();
					}
					

					if (ImGui::MenuItem("Quit", "Ctrl+Q")) { ApplicationManager::Get().Close(); }
					ImGui::EndMenu();

				}
				ImGui::EndMenuBar();
			}

			ImGui::Begin("Settings");
				float fps = 1.0f / dt;
				std::string FPSNote = "FPS: " + std::to_string(fps);
				ImGui::Text(FPSNote.c_str());
				auto stats = Renderer2D::GetStats();
				ImGui::Text("Renderer2D Stats:");
				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::Text("Quads: %d", stats.QuadCount);
				ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
				ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
				if (ImGui::Button("Toggle Light/Dark Mode"))
				{
					if (mLightMode)
					{
						ImGuiLayer::SetDarkThemeColours();
						mLightMode = false;
					}
					else
					{
						ImGuiLayer::SetLightThemeColours();
						mLightMode = true;
					}
								
				}


				// Eventually move this to a separate window
				if (ImGui::CollapsingHeader("Style Settings"))
				{
					if (ImGui::Button("Save Style"))
					{
						if (mLightMode)
						{
							auto& colors = ImGui::GetStyle().Colors;
							for (auto& sColor : sImGuiLightColours)
							{
								sColor.second = colors[sColor.first];
							}
						}
						else
						{
							auto& colors = ImGui::GetStyle().Colors;
							for (auto& sColor : sImGuiDarkColours)
							{
								sColor.second = colors[sColor.first];
							}
						}
						Serialiser::SerialiseEditorStyle();
					}
					auto& style = ImGui::GetStyle();

					auto ColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions;

					ImGui::Columns(2, "ColorColumns");
					// Adjust the width of the first column (labels)
					float labelColumnWidth = 200.f; // Adjust this value based on your layout needs
					ImGui::SetColumnWidth(0, labelColumnWidth);

					ImGui::Text("Text"); ImGui::NextColumn(); ImGui::ColorEdit4("##Text", &style.Colors[ImGuiCol_Text].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Text Disabled"); ImGui::NextColumn(); ImGui::ColorEdit4("##Text Disabled", &style.Colors[ImGuiCol_TextDisabled].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Window Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Window Background", &style.Colors[ImGuiCol_WindowBg].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Child Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Child Background", &style.Colors[ImGuiCol_ChildBg].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Popup Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Popup Background", &style.Colors[ImGuiCol_PopupBg].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Border"); ImGui::NextColumn(); ImGui::ColorEdit4("##Border", &style.Colors[ImGuiCol_Border].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Border Shadow"); ImGui::NextColumn(); ImGui::ColorEdit4("##Border Shadow", &style.Colors[ImGuiCol_BorderShadow].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Frame Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Frame Background", &style.Colors[ImGuiCol_FrameBg].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Frame Background Hovered"); ImGui::NextColumn(); ImGui::ColorEdit4("##Frame Background Hovered", &style.Colors[ImGuiCol_FrameBgHovered].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Frame Background Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Frame Background Active", &style.Colors[ImGuiCol_FrameBgActive].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Title Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Title Background", &style.Colors[ImGuiCol_TitleBg].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Title Background Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Title Background Active", &style.Colors[ImGuiCol_TitleBgActive].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Title Background Collapsed"); ImGui::NextColumn(); ImGui::ColorEdit4("##Title Background Collapsed", &style.Colors[ImGuiCol_TitleBgCollapsed].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Menubar Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##MenuBar Background", &style.Colors[ImGuiCol_MenuBarBg].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Scrollbar Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Scrollbar Background", &style.Colors[ImGuiCol_ScrollbarBg].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Scrollbar Grab"); ImGui::NextColumn(); ImGui::ColorEdit4("##Scrollbar Grab", &style.Colors[ImGuiCol_ScrollbarGrab].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Scrollbar Grab Hovered"); ImGui::NextColumn(); ImGui::ColorEdit4("##Scrollbar Grab Hovered", &style.Colors[ImGuiCol_ScrollbarGrabHovered].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Scrollbar Grab Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Scrollbar Grab Active", &style.Colors[ImGuiCol_ScrollbarGrabActive].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("CheckMark"); ImGui::NextColumn(); ImGui::ColorEdit4("##CheckMark", &style.Colors[ImGuiCol_CheckMark].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Slider Grab"); ImGui::NextColumn(); ImGui::ColorEdit4("##Slider Grab", &style.Colors[ImGuiCol_SliderGrab].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Slider Grab Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Slider Grab Active", &style.Colors[ImGuiCol_SliderGrabActive].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Button"); ImGui::NextColumn(); ImGui::ColorEdit4("##Button", &style.Colors[ImGuiCol_Button].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Button Hovered"); ImGui::NextColumn(); ImGui::ColorEdit4("##Button Hovered", &style.Colors[ImGuiCol_ButtonHovered].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Button Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Button Active", &style.Colors[ImGuiCol_ButtonActive].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Header"); ImGui::NextColumn(); ImGui::ColorEdit4("##Header", &style.Colors[ImGuiCol_Header].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Header Hovered"); ImGui::NextColumn(); ImGui::ColorEdit4("##Header Hovered", &style.Colors[ImGuiCol_HeaderHovered].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Header Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Header Active", &style.Colors[ImGuiCol_HeaderActive].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab", &style.Colors[ImGuiCol_Tab].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab Hovered"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab Hovered", &style.Colors[ImGuiCol_TabHovered].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab Active", &style.Colors[ImGuiCol_TabActive].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab Unfocused"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab Unfocused", &style.Colors[ImGuiCol_TabUnfocused].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab Unfocused Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab Unfocused Active", &style.Colors[ImGuiCol_TabUnfocusedActive].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab Selected"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab Selected", &style.Colors[ImGuiCol_TabSelected].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab Selected Overline"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab Selected Overline", &style.Colors[ImGuiCol_TabSelectedOverline].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab Dimmed"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab Dimmed", &style.Colors[ImGuiCol_TabDimmed].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab Dimmed Selected"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab Dimmed Selected", &style.Colors[ImGuiCol_TabDimmedSelected].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Tab Dimmed Selected Overline"); ImGui::NextColumn(); ImGui::ColorEdit4("##Tab Dimmed Selected Overline", &style.Colors[ImGuiCol_TabDimmedSelectedOverline].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Docking Preview"); ImGui::NextColumn(); ImGui::ColorEdit4("##Docking Preview", &style.Colors[ImGuiCol_DockingPreview].x, ColorEditFlags); ImGui::NextColumn();
					ImGui::Text("Docking Empty Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Docking Empty Background", &style.Colors[ImGuiCol_DockingEmptyBg].x, ColorEditFlags); ImGui::NextColumn();



					if (ImGui::CollapsingHeader("Not Frequently Used"))
					{
						ImGui::NextColumn(); ImGui::NextColumn();
						ImGui::Text("Separator"); ImGui::NextColumn(); ImGui::ColorEdit4("##Separator", &style.Colors[ImGuiCol_Separator].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Separator Hovered"); ImGui::NextColumn(); ImGui::ColorEdit4("##Separator Hovered", &style.Colors[ImGuiCol_SeparatorHovered].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Separator Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Separator Active", &style.Colors[ImGuiCol_SeparatorActive].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Resize Grip"); ImGui::NextColumn(); ImGui::ColorEdit4("##Resize Grip", &style.Colors[ImGuiCol_ResizeGrip].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Resize Grip Hovered"); ImGui::NextColumn(); ImGui::ColorEdit4("##Resize Grip Hovered", &style.Colors[ImGuiCol_ResizeGripHovered].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Resize Grip Active"); ImGui::NextColumn(); ImGui::ColorEdit4("##Resize Grip Active", &style.Colors[ImGuiCol_ResizeGripActive].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Plot Lines"); ImGui::NextColumn(); ImGui::ColorEdit4("##Plot Lines", &style.Colors[ImGuiCol_PlotLines].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Plot Lines Hovered"); ImGui::NextColumn(); ImGui::ColorEdit4("##Plot Lines Hovered", &style.Colors[ImGuiCol_PlotLinesHovered].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Plot Histogram"); ImGui::NextColumn(); ImGui::ColorEdit4("##Plot Histogram", &style.Colors[ImGuiCol_PlotHistogram].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Plot Histogram Hovered"); ImGui::NextColumn(); ImGui::ColorEdit4("##Plot Histogram Hovered", &style.Colors[ImGuiCol_PlotHistogramHovered].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Table Header Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Table Header Background", &style.Colors[ImGuiCol_TableHeaderBg].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Table Border Strong"); ImGui::NextColumn(); ImGui::ColorEdit4("##Table Border Strong", &style.Colors[ImGuiCol_TableBorderStrong].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Table Border Light"); ImGui::NextColumn(); ImGui::ColorEdit4("##Table Border Light", &style.Colors[ImGuiCol_TableBorderLight].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Table Row Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Table Row Background", &style.Colors[ImGuiCol_TableRowBg].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Table Row Background Alt"); ImGui::NextColumn(); ImGui::ColorEdit4("##Table Row Background Alt", &style.Colors[ImGuiCol_TableRowBgAlt].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Text Link"); ImGui::NextColumn(); ImGui::ColorEdit4("##Text Link", &style.Colors[ImGuiCol_TextLink].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Text Selected Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Text Selected Background", &style.Colors[ImGuiCol_TextSelectedBg].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Drag Drop Target"); ImGui::NextColumn(); ImGui::ColorEdit4("##Drag Drop Target", &style.Colors[ImGuiCol_DragDropTarget].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Nav Highlight"); ImGui::NextColumn(); ImGui::ColorEdit4("##Nav Highlight", &style.Colors[ImGuiCol_NavHighlight].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Nav Windowing Highlight"); ImGui::NextColumn(); ImGui::ColorEdit4("##Nav Windowing Highlight", &style.Colors[ImGuiCol_NavWindowingHighlight].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Nav Windowing Dim Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Nav Windowing Dim Background", &style.Colors[ImGuiCol_NavWindowingDimBg].x, ColorEditFlags); ImGui::NextColumn();
						ImGui::Text("Modal Window Background"); ImGui::NextColumn(); ImGui::ColorEdit4("##Modal Window Background", &style.Colors[ImGuiCol_ModalWindowDimBg].x, ColorEditFlags); ImGui::NextColumn();
					}

					ImGui::Columns(1);

				}
			ImGui::End(); // Of Settings

			SCPanel.ImGuiRender();
			CBPanel.ImGuiRender();
			
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
			ImGui::Begin("Viewport");

				mViewportFocused = ImGui::IsWindowFocused();
				mViewportHovered = ImGui::IsWindowHovered();
				// True when viewport not focused or not hovered
				if (mViewportFocused && mViewportHovered)
					ApplicationManager::Get().GetImGuiLayer()->SetBlockEvents(!mViewportFocused && !mViewportHovered);
				else
					ApplicationManager::Get().GetImGuiLayer()->SetBlockEvents(true);
				ImVec2 viewportSize = ImGui::GetContentRegionAvail();
				mViewportSize = { viewportSize.x, viewportSize.y };
				uint64_t screenID = static_cast<uint64_t>(mViewportFrameBuffer->GetColorAttachmentRendererID());
				ImGui::Image((ImTextureID)screenID, ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
				// Test code for drag and drop
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropSceneItem"))
					{
						AssetHandle data = *(const uint64_t*)payload->Data;
						//const char* data= (const char*)payload->Data;
						if (Project::GetProjectPath() != "")
						{
							std::string sceneName = Project::GetProjectPath() + "/assets/";
							sceneName += std::to_string(data);
							AssetMetaData assetMeta = AssetManager::GetMetaData(data);
							//OpenScene(sceneName.c_str());
							OpenScene(assetMeta.SourcePath.string().c_str());
						}
						else
						{
							std::string sceneName = "assets/";
							sceneName += std::to_string(data);
							OpenScene(sceneName.c_str());
						}
					}

					ImGui::EndDragDropTarget();
				}


				auto windowSize = ImGui::GetWindowSize();
				ImVec2 minBound = ImGui::GetWindowPos();


				ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
				mViewportBounds[0] = { minBound.x, minBound.y };
				mViewportBounds[1] = { maxBound.x, maxBound.y };

				// Gizmo here:
				Entity selectedEntity = SCPanel.GetSelectedEntity();
				if (selectedEntity && mGizmoType != -1)
				{
					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist();
					float windowWidth =	(float)ImGui::GetWindowWidth();
					float windowHeight = (float)ImGui::GetWindowHeight();
					ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

					//auto CameraEntity = mActiveScene->GetPrimaryCameraEntity();
					//auto& cameraComponent = CameraEntity.GetComponent<CameraComponent>();
					//const glm::mat4 cameraView = glm::inverse(CameraEntity.GetComponent<TransformComponent>().GetTransform());
					//const glm::mat4& cameraProjection = cameraComponent.Camera.GetProjectionMatrix();

					const glm::mat4& cameraView = mEditorCamera.GetViewMatrix();
					const glm::mat4& cameraProjection = mEditorCamera.GetProjectionMatrix();
					 
					auto& tc = selectedEntity.GetComponent<TransformComponent>();
					glm::mat4 transform = tc.GetTransform();

					bool snap = InputSystem::IsKeyPressed(Key::LeftShift);
					float snapValue = 0.5f;
					if (mGizmoType == ImGuizmo::OPERATION::ROTATE)
						snapValue = 45.0f;

					float snapValues[3] = { snapValue, snapValue, snapValue };


					ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)mGizmoType, 
						ImGuizmo::MODE::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues:nullptr);
						if (!(InputSystem::IsKeyPressed(Key::LeftAlt) || InputSystem::IsKeyPressed(Key::RightAlt)))
						{
							if (ImGuizmo::IsUsing())
							{
								glm::vec3 translation, rotation, scale;
								ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));
								glm::vec3 deltaRotation = rotation - tc.Rotation;
								tc.Rotation = rotation;
								tc.Translate = translation;
								tc.Scale = scale;
							}
						}
					
				}



			ImGui::End(); // Of Viewport
			

			ImGui::Begin("Runtime");
			{
				if (hasRuntimeCamera)
				{
					mRuntimeHovered = ImGui::IsWindowHovered();
					mRuntimeFocused = ImGui::IsWindowFocused();
					ImVec2 runtimeSize = ImGui::GetContentRegionAvail();
					mRuntimeSize = { runtimeSize.x, runtimeSize.y };
					uint64_t screenID = static_cast<uint64_t>(mRuntimeFrameBuffer->GetColorAttachmentRendererID());
					ImGui::Image((ImTextureID)screenID, ImVec2{ mRuntimeSize.x, mRuntimeSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
				}
			}
			ImGui::End(); // Of Runtime
			ImGui::PopStyleVar();
			UIToolbar();
		ImGui::End(); // Of Dockspace
	}

	bool EditorLayer::onMousePressed(EventMouseButtonTriggered& e)
	{
		switch (e.GetMouseButton())
		{
			case Mouse::ButtonLeft:
			{
				if (!(InputSystem::IsKeyPressed(Key::LeftAlt) || InputSystem::IsKeyPressed(Key::RightAlt)))
				{
					if (mViewportHovered && mHoveredEntity && !ImGuizmo::IsOver())
					{
						if(mHoveredEntity.IsValid())
						{
							SCPanel.SetSelectedEntity(mHoveredEntity);
						}
					}
					else if (mViewportHovered && !ImGuizmo::IsOver())
					{
						SCPanel.SetSelectedEntity({});
					}
				}
				break;
			}
		}
		return true;
	}

	bool EditorLayer::onKeyPressed(EventKeyPressed& e)
	{

		bool control = InputSystem::IsKeyPressed(Key::LeftControl) || InputSystem::IsKeyPressed(Key::RightControl);
		bool shift = InputSystem::IsKeyPressed(Key::LeftShift) || InputSystem::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
			{
				NewProject();
			}
			break;
		}
		case Key::O:
		{
			if (control)
			{
				LoadProject();
			}
			break;
		}
		case Key::S:
		{
			if (control)
			{
				SaveProject();
			}
			break;
		}
		case Key::Q:
		{
			if (control)
			{
				ApplicationManager::Get().Close(); 
			}
			else if (SCPanel.GetSelectedEntity())
			{
				mGizmoType = -1;
			}
			break;
		}

		case Key::W:
		{
			if (SCPanel.GetSelectedEntity())
			{
				mGizmoType = ImGuizmo::TRANSLATE;
			}
			break;
		}

		case Key::E:
		{
			if (SCPanel.GetSelectedEntity())
			{
				mGizmoType = ImGuizmo::ROTATE;
			}
			break;
		}

		case Key::R:
		{
			if (SCPanel.GetSelectedEntity())
			{
				mGizmoType = ImGuizmo::SCALE;
			}
			break;
		}

		case Key::Equal:
		{
			if (SCPanel.GetSelectedEntity() && mGizmoType!= -1)
			{
				if (shift) // plus key
				{
					imGuizmoClipSpace += 0.1f;
					ImGuizmo::SetGizmoSizeClipSpace(imGuizmoClipSpace);
				}
			}
			break;
		}

		case Key::Minus:
		{
			if (SCPanel.GetSelectedEntity() && mGizmoType != -1)
			{
				if (shift) // minus key
				{
					imGuizmoClipSpace = std::max(0.1f, imGuizmoClipSpace - 0.1f);
					ImGuizmo::SetGizmoSizeClipSpace(imGuizmoClipSpace);
				}
			}
			break;
		}

		case Key::D:
		{
			if (control)
			{
				if (mSceneState == SceneState::Edit && SCPanel.GetSelectedEntity())
				{
					SceneManager::GetActiveScene()->DuplicateEntity(SCPanel.GetSelectedEntity());
				}
			}
			break;
		}

		case Key::Delete:
		{
			if (SCPanel.GetSelectedEntity())
			{
				SceneManager::GetActiveScene()->DestroyEntity(SCPanel.GetSelectedEntity());
				SCPanel.SetSelectedEntity({});
			}
			break;
		}

		case Key::F:
		{
			if (SCPanel.GetSelectedEntity())
			{
				mEditorCamera.SetFocalPoint(SCPanel.GetSelectedEntity().GetComponent<TransformComponent>().Translate);
			}
		}

		}

		return true;
	}

	void EditorLayer::NewScene()
	{
		mEditorScene = MakeRef<Scene>();
		mEditorScene->ResizeViewport((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		SCPanel.SetContext(mEditorScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Scene File (*.sc)\0*.sc\0");
		if (!filepath.empty())
		{

			if (mSceneState != SceneState::Edit)
			{
				SceneStop();
			}

			mEditorScene = MakeRef<Scene>();
			mEditorScene->ResizeViewport((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			SCPanel.SetContext(mEditorScene);

			Serialiser serialiser(mEditorScene);
			serialiser.DeserialiseScene(filepath);

			SceneManager::GetActiveScene() = mEditorScene;
			if (hasRuntimeCamera)
			{
				SceneManager::GetActiveScene()->GetRegistry().get<CameraComponent>(mRuntimeCamera).Camera.SetViewportSize((uint32_t)mRuntimeSize.x, (uint32_t)mRuntimeSize.y);
			}
		}
	}

	void EditorLayer::OpenScene(const char* Cfilepath)
	{
		if (mSceneState != SceneState::Edit)
		{
			BOREALIS_CORE_ERROR("Scene is running, cannot open scene");
			return;
		}

		std::string filepath = Cfilepath;
		if (!filepath.empty())
		{
			std::string fileName = filepath.substr(filepath.find_last_of("/\\") + 1);
			fileName = fileName.substr(0, fileName.find_last_of("."));
			AddScene(fileName, filepath);
			SceneManager::SetActiveScene(fileName);

			DeserialiseEditorScene();
		}
		//auto entity = SceneManager::GetActiveScene()->CreateEntity("testBehaviourTree");
		//auto& btC = entity.AddComponent<BehaviourTreeComponent>();
		//auto idleNode = NodeFactory::createNodeByName("L_Idle");
		//auto sequenceNode = NodeFactory::createNodeByName("C_Sequencer");
		//auto clickNode = NodeFactory::createNodeByName("L_CheckMouseClick");
		//Ref<BehaviourTree> betree = MakeRef<BehaviourTree>();
		//betree->SetBehaviourTreeName("Test-Tree");
		//betree->SetRootNode(sequenceNode);
		//betree->AddNode(betree->GetRootNode(), idleNode,1);
		//betree->AddNode(betree->GetRootNode(), clickNode,1);
		//betree->add selector(root)


		//btC.AddTree(betree);
	}

	void EditorLayer::SaveScene()
	{

	}

	void EditorLayer::BuildProject()
	{
		std::string filepath = FileDialogs::SaveFile("Folder Name");
		if (!filepath.empty())
		{
			std::string projectName = filepath.substr(filepath.find_last_of("/\\") + 1);
			std::filesystem::create_directory(filepath);
			// make a new folder
			Project::BuildExportSettings(filepath, projectName);
			// Copy and paste assets
			std::filesystem::create_directory(filepath + "\\Assets");
			Project::CopyFolder(Project::GetProjectPath() + "\\Assets", filepath + "\\Assets");
			std::filesystem::create_directory(filepath + "\\Cache");
			Project::CopyFolder(Project::GetProjectPath() + "\\Cache", filepath + "\\Cache");
			Project::CopyIndividualFile(Project::GetProjectPath() + "\\AssetRegistry.brdb", filepath + "\\AssetRegistry.brdb");

			// copy fmod dll and mono dll from editor
			// Editor directory
			std::string editorPath = std::filesystem::current_path().string();
#ifndef _DEB
			Project::CopyIndividualFile(editorPath + "\\fmod.dll", filepath + "\\fmod.dll");
#else
			Project::CopyIndividualFile(editorPath + "\\fmodL.dll", filepath + "\\fmodL.dll");
#endif
			Project::CopyIndividualFile(editorPath + "\\mono-2.0-sgen.dll", filepath + "\\mono-2.0-sgen.dll");
			Project::CopyFolder(editorPath + "\\mono", filepath + "\\mono");
			Project::CopyFolder(editorPath + "\\resources", filepath + "\\resources");
			Project::CopyFolder(editorPath + "\\engineResources", filepath + "\\engineResources");
			Project::CopyIndividualFile(editorPath + "\\BorealisRuntime.exe", filepath + "\\BorealisRuntime.exe");
			Project::CopyIndividualFile(editorPath + "\\BorealisAssetCompiler.exe", filepath + "\\BorealisAssetCompiler.exe");
			Project::CopyIndividualFile(editorPath + "\\ispc_texcomp.dll", filepath + "\\" + "\\ispc_texcomp.dll");
			// Copy and paste .exe file
		}
	}

	void EditorLayer::ScenePlay()
	{

		if (!hasRuntimeCamera)
		{
			BOREALIS_CORE_TRACE("Cannot start scene without camera");
			return;
		}
		mSceneState = SceneState::Play;

		Ref<Scene> copiedScene = Scene::Copy(SceneManager::GetActiveScene());
		copiedScene->SetName(copiedScene->GetName() + "-runtime");
		SceneManager::AddScene(copiedScene->GetName(), "");
		mEditorScene = SceneManager::GetActiveScene();
		SceneManager::SetActiveScene(copiedScene);
		SCPanel.SetContext(SceneManager::GetActiveScene());
		SceneManager::GetActiveScene()->RuntimeStart();

		auto view = SceneManager::GetActiveScene()->GetRegistry().view<ScriptComponent>();
		for (auto entity : view)
		{
			auto& scriptComponent = view.get<ScriptComponent>(entity);
			for (auto& [name,script] : scriptComponent.mScripts)
			{
				script->Start();
			}
		}
	}

	void EditorLayer::SceneStop()
	{
		mSceneState = SceneState::Edit;
		SceneManager::GetActiveScene()->RuntimeEnd();
		SCPanel.SetSelectedEntity({});
		std::string tmpName = SceneManager::GetActiveScene()->GetName();
		SceneManager::SetActiveScene(mEditorScene);
		SceneManager::RemoveScene(tmpName);
		SCPanel.SetContext(SceneManager::GetActiveScene());

		auto view = SceneManager::GetActiveScene()->GetRegistry().view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (cameraComponent.Primary)
			{
				mRuntimeCamera = Entity(entity, SceneManager::GetActiveScene().get());
			}
		}
	}

	void EditorLayer::ScenePause()
	{
		mSceneState = SceneState::Pause;
	}

	void EditorLayer::SceneResume()
	{
		mSceneState = SceneState::Play;
	}

	void EditorLayer::AddScene(std::string scenename, std::string scenepath)
	{
		SceneManager::AddScene(scenename, scenepath);
	}

	void EditorLayer::RemoveScene(std::string sceneName)
	{
		SceneManager::RemoveScene(sceneName);
	}

	void EditorLayer::DeserialiseEditorScene()
	{
		mEditorScene = SceneManager::GetActiveScene();
		mEditorScene->ResizeViewport((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		SCPanel.SetContext(mEditorScene);
	}

	void EditorLayer::LoadProject()
	{
		if (mSceneState != SceneState::Edit)
		{
			BOREALIS_CORE_ERROR("Scene is running, cannot open project");
			return;
		}

		std::string filepath = FileDialogs::OpenFile("Borealis Project File (*.brproj)\0*.brproj\0");
		if (!filepath.empty())
		{
			SceneManager::ClearSceneLibrary();
			std::string activeSceneName;
			if (Project::SetProjectPath(filepath.c_str(), activeSceneName))
			{
				mAssetImporter.LoadRegistry(Project::GetProjectInfo());
				SceneManager::SetActiveScene(activeSceneName);

				for (auto [handle,meta] : Project::GetEditorAssetsManager()->GetAssetRegistry())
				{
					if (meta.Type == AssetType::Prefab)
					{
						PrefabManager::DeserialisePrefab(meta.SourcePath.string());
					}
				}
			}
		


			std::string assetsPath = Project::GetProjectPath() + "\\Assets";
			CBPanel.SetCurrDir(assetsPath);
			DeserialiseEditorScene();
			// Clear Scenes in Scene Manager
			// Clear Assets in Assets Manager
			// Load Scenes in Assets Manager
			// Load Assets in Assets Manager
		}
	}

	void EditorLayer::NewProject()
	{
		if (mSceneState != SceneState::Edit)
		{
			BOREALIS_CORE_ERROR("Scene is running, cannot create new project");
			return;
		}

		std::string filepath = FileDialogs::SaveFile("Folder");
		if (!filepath.empty())
		{
			std::string originalFilePath = filepath;
			// extract last part of the path
			std::string projectName = filepath.substr(filepath.find_last_of("/\\") + 1);
			// exclude project name from file path
			filepath = filepath.substr(0, filepath.find_last_of("/\\"));
			Project::CreateProject(projectName.c_str(), filepath.c_str());
			std::string assetsPath = Project::GetProjectPath() + "\\Assets";
			std::string buffer;
			Project::SetProjectPath(Project::GetProjectPath() + "\\Project.brproj", buffer); //TEMP
			// Create default empty scene
			SceneManager::ClearSceneLibrary();
			SceneManager::CreateScene("untitled", assetsPath);
			SceneManager::SetActiveScene("untitled");

			CBPanel.SetCurrDir(assetsPath);
			EditorLayer::DeserialiseEditorScene();

			mAssetImporter.LoadRegistry(Project::GetProjectInfo());

			// Clear Scenes in Scene Manager
			// Clear Assets in Assets Manager
		}
	
	}

	void EditorLayer::SaveProject()
	{
		if (mSceneState != SceneState::Edit)
		{
			BOREALIS_CORE_ERROR("Scene is running, cannot save project");
			return;
		}
		Project::SaveProject();
		SceneManager::SaveActiveScene();
	}

	void EditorLayer::UIToolbar()
	{
		ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			Entity mainCamera{};
			const char* currentCameraTag = nullptr;
			std::vector<const char*> cameraTags;
			std::vector<Entity> cameraEntities;
			auto group = SceneManager::GetActiveScene()->GetRegistry().group<CameraComponent>(entt::get<TagComponent>);
			group.each([&](auto entity, CameraComponent& camera, TagComponent& tag)
				{
					if (camera.Primary == true)
					{
						mainCamera = Entity{ entity, SceneManager::GetActiveScene().get() };
						hasRuntimeCamera = true;
						mRuntimeCamera = mainCamera;
						currentCameraTag = tag.Tag.c_str();
					}
					cameraEntities.push_back(Entity{ entity, SceneManager::GetActiveScene().get() });
					cameraTags.push_back(tag.Tag.c_str());
				});

			if (!currentCameraTag && !group.empty())
			{
				mainCamera = cameraEntities[0];
				currentCameraTag = cameraTags[0];
				mainCamera.GetComponent<CameraComponent>().Primary = true;
				mRuntimeCamera = mainCamera;
				hasRuntimeCamera = true;
			}

			
			if (currentCameraTag)
			{
				ImGui::Text("Main Camera: ");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(250.f);
				if (ImGui::BeginCombo("##MainCamera", currentCameraTag))
				{
					for (int i = 0; i < cameraEntities.size(); i++)
					{
						bool isSelected = mainCamera == cameraEntities[i];
						if (ImGui::Selectable(cameraTags[i], isSelected))
						{
							group.each([&](auto entity, CameraComponent& camera, TagComponent& tag)
								{
									camera.Primary = false;
								});
							cameraEntities[i].GetComponent<CameraComponent>().Primary = true;
							mRuntimeCamera = cameraEntities[i];
						}
						else
						{
							if (isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
					}
					ImGui::EndCombo();
				}
			}
			else
			{
				hasRuntimeCamera = false;
				ImGui::Text("Main Camera: ");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(250.f);
				if (ImGui::BeginCombo("##MainCamera:", "")) {
					ImGui::EndCombo();
				};
			}
			ImGui::SameLine();

			if (mSceneState == SceneState::Edit) // Not playing
			{
				if (ImGui::Button("O"))
				{
					ScenePlay();
				}
			}
			else if (mSceneState == SceneState::Play)
			{
				if (ImGui::Button("X"))
				{
					SceneStop();
				}
				ImGui::SameLine();
				if (ImGui::Button("::"))
				{
					ScenePause();
				}
			}
			else if (mSceneState == SceneState::Pause)
			{
				if (ImGui::Button("X"))
				{
					SceneStop();
				}
				ImGui::SameLine();
				if (ImGui::Button("O"))
				{
					SceneResume();
				}
			}

		}

		ImGui::SameLine();

		ImGui::SetNextItemWidth(100.f);
		if (ImGui::SliderFloat("##LineThickness", &mLineThickness, 0.1f, 10.0f, "", ImGuiSliderFlags_NoInput))
		{
			RenderCommand::SetLineThickness(mLineThickness);
		}
		ImGui::End();
	}

}