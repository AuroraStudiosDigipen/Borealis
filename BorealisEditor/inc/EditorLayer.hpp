/******************************************************************************/
/*!
\file		EditorLayer.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 11, 2024
\brief		Declares the class for the layer for level editor

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/


#ifndef EDITOR_LAYER_HPP
#define EDITOR_LAYER_HPP
#include <vector>
#include <Borealis.hpp>
#include <Panels/SceneHierarchyPanel.hpp>
#include <Panels/ContentBrowserPanel.hpp>
#include <Panels/ConsolePanel.hpp>
#include <Panels/BTNodeEditorPanel.hpp>
#include <Panels/AudioMixerPanel.hpp>
#include <Graphics/EditorCamera.hpp>
#include <EditorAssets/AssetImporter.hpp>

namespace Borealis {
	class EditorLayer : public Layer
	{
	public:


		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Pause = 2
		};

		/*!***********************************************************************
			\brief
				Constructor for the EditorLayer
		*************************************************************************/
		EditorLayer();

		/*!***********************************************************************
			\brief
				Destructor for the EditorLayer
		*************************************************************************/
		~EditorLayer() {}

		/*!***********************************************************************
			\brief
				Initialise the EditorLayer
		*************************************************************************/
		void Init() override;

		/*!***********************************************************************
			\brief
				Free the EditorLayer
		*************************************************************************/
		void Free() override;

		/*!***********************************************************************
			\brief
				Update the EditorLayer
			\param[in] dt
				The time between frames
		*************************************************************************/
		void UpdateFn(float dt) override;

		/*!***********************************************************************
			\brief
				Event function for the EditorLayer
			\param[in] e
				The event to be processed
		*************************************************************************/
		void EventFn(Event& e) override;

		/*!***********************************************************************
			\brief
				Render the EditorLayer
		*************************************************************************/
		void ImGuiRender(float dt) override;

		/*!***********************************************************************
			\brief
				Key pressed event callback
			\param[in] e
				The event to be processed
			\return
				True if the event is processed
		*************************************************************************/
		bool onKeyPressed(EventKeyPressed& e);

		/*!***********************************************************************
			\brief
				Key released event callback
			\param[in] e
				The event to be processed
			\return
				True if the event is processed
		*************************************************************************/
		bool onMousePressed(EventMouseButtonTriggered& e);
		static SceneState mSceneState;
	private:

		/*!***********************************************************************
			\brief
				Create a new scene
		*************************************************************************/
		void NewScene();

		/*!***********************************************************************
			\brief
				Open a scene
		*************************************************************************/
		void OpenScene();

		/*!***********************************************************************
			\brief
				Open a scene from file
		*************************************************************************/
		void OpenScene(const char * filename);

		/*!***********************************************************************
			\brief
				Save the scene
		*************************************************************************/
		void SaveScene();

		/*!***********************************************************************
			\brief
				Play the scene
		*************************************************************************/
		void ScenePlay();

		/*!***********************************************************************
			\brief
				Stop the scene
		*************************************************************************/
		void SceneStop();

		/*!***********************************************************************
			\brief
				Pause the scene
		*************************************************************************/
		void ScenePause();

		/*!***********************************************************************
			\brief
				Resume the scene
		*************************************************************************/
		void SceneResume();

		/*!***********************************************************************
			\brief
				Load the scene
		*************************************************************************/
		void AddScene(std::string scenename, std::string scenepath);

		/*!***********************************************************************
			\brief
				Remove the scene
		*************************************************************************/
		void RemoveScene(std::string sceneName);

		/*!***********************************************************************
			\brief
				Deserialise for the editor scene
		*************************************************************************/
		void DeserialiseEditorScene();

		/*!***********************************************************************
			\brief
				Load a project
		*************************************************************************/
		void LoadProject();

		void LoadProjectBackground(const std::string& filepath);

		/*!***********************************************************************
			\brief
				New project
		*************************************************************************/
		void NewProject();

		/*!***********************************************************************
			\brief
				Save the project
		*************************************************************************/
		void SaveProject();


		/*!***********************************************************************
			\brief
				Build the project
		*************************************************************************/
		void BuildProject();


		/*!***********************************************************************
			\brief
				The ImGUI UI toolbar
		*************************************************************************/
		void UIToolbar();


		OrthographicCameraController mCamera;
		Ref<Texture2D> mTexture;
		Ref<SubTexture2D> mSubTexture;
		//Ref<FrameBuffer> mViewportFrameBuffer;
		//Ref<FrameBuffer> mRuntimeFrameBuffer;
		Ref<Scene> mEditorScene;

		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		glm::vec2 mRuntimeSize = { 0.0f, 0.0f };
		glm::vec2 mViewportBounds[2];
		bool mViewportFocused = false;
		bool mViewportHovered = false;
		bool mRuntimeFocused = false;
		bool mRuntimeHovered = false;
		bool hasRuntimeCamera = false;

		Entity mRuntimeCamera;
		Entity mHoveredEntity;
		
		int mGizmoType = -1;
		float imGuizmoClipSpace = 0.1f;

		EditorCamera mEditorCamera;
		SceneHierarchyPanel SCPanel;
		ContentBrowserPanel CBPanel;
		ConsolePanel CSPanel;
		BTNodeEditorPanel BTNEPanel;
		AudioMixerPanel AMPanel;
		bool mLightMode = true;

		float mLineThickness = 1.0f;


		AssetImporter mAssetImporter;

		Ref<FrameBuffer> mViewportFrameBuffer;
	};
}

#endif