/******************************************************************************/
/*!
\file		SceneCamera.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 13, 2024
\brief		Declares the class for Scene Camera editor interface

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SCENE_CAMERA_HPP
#define SCENE_CAMERA_HPP
#include <Graphics/Camera.hpp>
namespace Borealis
{
	class SceneCamera : public Camera
	{
	public:
		enum class CameraType
		{
			Perspective = 0,
			Orthographic = 1
		};

		/*!***********************************************************************
			\brief
				Default constructor for SceneCamera
		*************************************************************************/
		SceneCamera();

		/*!***********************************************************************
			\brief
				Destructor for SceneCamera
		*************************************************************************/
		~SceneCamera() = default;

		/*!***********************************************************************
			\brief
				Sets the orthographic projection of the camera
			\param[in] size
				The size of the orthographic projection
			\param[in] nearClip
				The near clipping plane of the orthographic projection
			\param[in] farClip
				The far clipping plane of the orthographic projection
		*************************************************************************/
		void SetOrtho(float size, float nearClip, float farClip);

		/*!***********************************************************************
			\brief
				Sets the perspective projection of the camera
			\param[in] fov
				The field of view of the perspective projection
			\param[in] nearClip
				The near clipping plane of the perspective projection
			\param[in] farClip
				The far clipping plane of the perspective projection
		*************************************************************************/
		void SetPers(float fov, float nearClip, float farClip);

		/*!***********************************************************************
			\brief
				Sets the aspect ratio of the camera
			\param[in] width
				The width of the viewport
			\param[in] height
				The height of the viewport
		*************************************************************************/
		void SetViewportSize(uint32_t width, uint32_t height);

		/*!***********************************************************************
			\brief
				Gets the orthographic size of the camera
			return
				The orthographic size of the camera
		*************************************************************************/
		float GetOrthoSize() const { return mOrthoSize; }

		/*!***********************************************************************
			\brief
				Sets the orthographic size of the camera
			\param[in] size
				The orthographic size of the camera
		*************************************************************************/
		void SetOrthoSize(float size) { mOrthoSize = size; RecalculateProjection(); }

		/*!***********************************************************************
			\brief
				Gets the orthographic near clipping plane of the camera
			return
				The orthographic near clipping plane of the camera
		*************************************************************************/
		float GetOrthoNear() const { return mOrthoNear; }

		/*!***********************************************************************
			\brief
				Sets the orthographic near clipping plane of the camera
			\param[in] nearClip
				The orthographic near clipping plane of the camera
		*************************************************************************/
		void SetOrthoNear(float nearClip) { mOrthoNear = nearClip; RecalculateProjection(); }

		/*!***********************************************************************
			\brief
				Gets the orthographic far clipping plane of the camera
			return
				The orthographic far clipping plane of the camera
		*************************************************************************/
		float GetOrthoFar() const { return mOrthoFar; }

		/*!***********************************************************************
			\brief
				Sets the orthographic far clipping plane of the camera
			\param[in] farClip
				The orthographic far clipping plane of the camera
		*************************************************************************/
		void SetOrthoFar(float farClip) { mOrthoFar = farClip; RecalculateProjection(); }

		/*!***********************************************************************
			\brief
				Gets the perspective field of view of the camera
			return
				The perspective field of view of the camera
		*************************************************************************/
		float GetPerspFOV() const { return mPerspFOV; }

		/*!***********************************************************************
			\brief
				Sets the perspective field of view of the camera
			\param[in] fov
				The perspective field of view of the camera
		*************************************************************************/
		void SetPerspFOV(float fov) { mPerspFOV = fov; RecalculateProjection(); }

		/*!***********************************************************************
			\brief
				Gets the perspective near clipping plane of the camera
			return
				The perspective near clipping plane of the camera
		*************************************************************************/
		float GetPerspNear() const { return mPerspNear; }

		/*!***********************************************************************
			\brief
				Sets the perspective near clipping plane of the camera
			\param[in] nearClip
				The perspective near clipping plane of the camera
		*************************************************************************/
		void SetPerspNear(float nearClip) { mPerspNear = nearClip; RecalculateProjection(); }

		/*!***********************************************************************
			\brief
				Gets the perspective far clipping plane of the camera
			return
				The perspective far clipping plane of the camera
		*************************************************************************/
		float GetPerspFar() const { return mPerspFar; }

		/*!***********************************************************************
			\brief
				Sets the perspective far clipping plane of the camera
			\param[in] farClip
				The perspective far clipping plane of the camera
		*************************************************************************/
		void SetPerspFar(float farClip) { mPerspFar = farClip; RecalculateProjection(); }

		/*!***********************************************************************
			\brief
				Gets the camera type
			\return
				The camera type
		*************************************************************************/
		CameraType GetCameraType() const { return mCameraType; }

		/*!***********************************************************************
			\brief
				Sets the camera type
			\param[in] type
				The camera type
		*************************************************************************/
		void SetCameraType(CameraType type) { mCameraType = type; RecalculateProjection(); }

		glm::vec2 GetViewPortSize() const override;

		float GetFOV() const override;

		float GetNearPlane() const override;
		float GetFarPlane() const override;
		float GetAspectRatio() const override;

	private:
		/*!***********************************************************************
			\brief
				Recalculates the projection matrix of the camera
		*************************************************************************/
		void RecalculateProjection();

		CameraType mCameraType = CameraType::Orthographic;
		float mOrthoSize = 350.0f;
		float mOrthoNear = -1.0f;
		float mOrthoFar = 1000.0f;

		float mPerspFOV = glm::radians(45.0f);
		float mPerspNear = 20.f;
		float mPerspFar = 1000.0f;

		float mAspectRatio = 1.78f;

		float mWidth{};
		float mHeight{};
	};
}

#endif