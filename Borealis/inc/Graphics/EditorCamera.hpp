/******************************************************************************/
/*!
\file		EditorCamera.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 15, 2024
\brief		Declares the class for Editor Camera

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef EDITOR_CAMERA_HPP
#define EDITOR_CAMERA_HPP
#include <glm/glm.hpp>
#include <Events/EventInput.hpp>
#include <Core/EventSystem.hpp>
#include <Graphics/Camera.hpp>

namespace Borealis
{

	struct EditorCameraProps
	{
		float mDistance = 10.0f; /*!< Distance of Editor Camera */
		float mPitch = 0.0f; /*!< Pitch of Editor Camera */
		float mYaw = 0.0f; /*!< Yaw of Editor Camera */
		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f }; /*!< Position of Editor Camera */
		glm::vec3 mFocalPoint = { 0.0f, 0.0f, 0.0f }; /*!< Focal Point of Editor Camera */
	};


	class EditorCamera : public Camera
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor for Editor Camera
		*************************************************************************/
		EditorCamera() {};

		/*!***********************************************************************
			\brief
				Constructor for Editor Camera
			\param fov
				Field of view
			\param aspectRatio
				Aspect ratio
			\param nearClip
				Near clipping plane
			\param farClip
				Far clipping plane
		*************************************************************************/
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		/*!***********************************************************************
			\brief
				Update function for Editor Camera
			\param dt
				Delta time
		*************************************************************************/
		void UpdateFn(float dt);

		/*!***********************************************************************
			\brief
				Event function for Editor Camera
			\param e
				Event
		*************************************************************************/
		void EventFn(Event& e);

		/*!***********************************************************************
			\brief
				Get Distance function for Editor Camera
		*************************************************************************/
		inline float GetDistance() const { return mDistance; }

		/*!***********************************************************************
			\brief
				Set Distance function for Editor Camera
			\param distance
				Distance
		*************************************************************************/
		inline void SetDistance(float distance) { mDistance = distance; }

		/*!***********************************************************************
			\brief
				Set Viewport Size function for Editor Camera
			\param width
				Width
			\param height
				Height
		*************************************************************************/
		inline void SetViewportSize(float width, float height) { mViewportWidth = width; mViewportHeight = height; UpdateProjection(); }

		glm::vec2 GetViewPortSize() const override { return glm::vec2(mViewportWidth, mViewportHeight); }

		/*!***********************************************************************
			\brief
				Get View Matrix of Editor Camera
			\return
				View Matrix
		*************************************************************************/
		const glm::mat4& GetViewMatrix() const { return mViewMatrix; }

		/*!***********************************************************************
			\brief
				Get View Projection Matrix of Editor Camera
			\return
				View Projection Matrix
		*************************************************************************/
		glm::mat4 GetViewProjectionMatrix() const { return mProjectionMatrix * mViewMatrix; }

		/*!***********************************************************************
			\brief
				Get Up direction vector relative to the editor camera
			\return
				Up direction vector
		*************************************************************************/
		glm::vec3 GetUpDirection() const;

		/*!***********************************************************************
			\brief
				Get Right direction vector relative to the editor camera
			\return
				Right direction vector
		*************************************************************************/
		glm::vec3 GetRightDirection() const;

		/*!***********************************************************************
			\brief
				Get Forward direction vector relative to the editor camera
			\return
				Forward direction vector
		*************************************************************************/
		glm::vec3 GetForwardDirection() const;

		/*!***********************************************************************
			\brief
				Get Position of the editor camera
			\return
				Position of the editor camera
		*************************************************************************/
		const glm::vec3& GetPosition() const { return mPosition; }

		/*!***********************************************************************
			\brief
				Get Orientation of the Editor Camera
			\return
				Orientation of the Editor Camera
		*************************************************************************/
		glm::quat GetOrientation() const;

		/*!***********************************************************************
			\brief
				Get Pitch of the Editor Camera
			\return
				Pitch of the Editor Camera
		*************************************************************************/
		float GetPitch() const { return mPitch; }

		/*!***********************************************************************
			\brief
				Get Yaw of the Editor Camera
			\return
				Yaw of the Editor Camera
		*************************************************************************/
		float GetYaw() const { return mYaw; }

		void SetFocalPoint(glm::vec3 point);

		float GetFOV() const override { return mFOV; }

		float GetNearPlane() const override { return mNearClip; }
		float GetFarPlane() const override { return mFarClip; }

		float GetAspectRatio() const override { return mAspectRatio; }

		EditorCameraProps GetProperties();
		void SetProperties(EditorCameraProps prop);
	private:

		/*!***********************************************************************
			\brief
				Update Projection Matrix of Editor Camera
		*************************************************************************/
		void UpdateProjection();

		/*!***********************************************************************
			\brief
				Update View Matrix of Editor Camera
		*************************************************************************/
		void UpdateView();

		/*!***********************************************************************
			\brief
				On Mouse Scroll Event function for Editor Camera
			\param e
				Mouse Scroll Event
		*************************************************************************/
		bool OnMouseScroll(EventMouseScrolled& e);

		/*!***********************************************************************
			\brief
				Mouse Panning function for Editor Camera
			\param[in] delta
				Vector of amount to pan
		*************************************************************************/
		void MousePan(const glm::vec2& delta);

		/*!***********************************************************************
			\brief
				Mouse Rotate function for Editor Camera
			\param[in] delta
				Vector of amount to rotate
		*************************************************************************/
		void MouseRotate(const glm::vec2& delta);

		/*!***********************************************************************
			\brief
				Mouse Zoom function for Editor Camera
			\param[in] delta
				Amount to zoom
		*************************************************************************/
		void MouseZoom(float delta);

		/*!***********************************************************************
			\brief
				Calculate Position function for Editor Camera
			\return
				Calculated Position
		*************************************************************************/
		glm::vec3 CalculatePosition() const;

		/*!***********************************************************************
			\brief
				Calculate Pan Speed for Editor Camera
			\return
				Pan Speed for x and y
		*************************************************************************/
		std::pair<float, float> PanSpeed() const;

		/*!***********************************************************************
			\brief
				Calculate Rotation Speed for Editor Camera
			\return
				Rotation Speed
		*************************************************************************/
		float RotationSpeed() const;

		/*!***********************************************************************
			\brief
				Calculate Zoom Speed for Editor Camera
			\return
				Zoom Speed
		*************************************************************************/
		float ZoomSpeed() const;

		glm::mat4 mViewMatrix; /*!< View Matrix of Editor Camera */
		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f }; /*!< Position of Editor Camera */
		glm::vec3 mFocalPoint = { 0.0f, 0.0f, 0.0f }; /*!< Focal Point of Editor Camera */
		glm::vec2 mInitialMousePosition = { 0.0f, 0.0f }; /*!< Initial Mouse Position */

		float mFOV = 45.0f; /*!< Field of View of Editor Camera */
		float mAspectRatio = 1.778f; /*!< Aspect Ratio of Editor Camera */
		float mNearClip = 0.1f; /*!< Near Clipping Plane of Editor Camera */
		float mFarClip = 1000.0f; /*!< Far Clipping Plane of Editor Camera */
		float mDistance = 10.0f; /*!< Distance of Editor Camera */
		float mPitch = 0.0f; /*!< Pitch of Editor Camera */
		float mYaw = 0.0f; /*!< Yaw of Editor Camera */
		float mViewportWidth = 1920; /*!< Viewport Width of Editor Camera */
		float mViewportHeight = 1080; /*!< Viewport Height of Editor Camera */
	}; // class EditorCamera
} // namespace Borealis

#endif