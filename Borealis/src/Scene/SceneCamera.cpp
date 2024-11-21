/******************************************************************************/
/*!
\file		SceneCamera.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 13, 2024
\brief		Defines the class for Scene Camera editor interface

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Scene/SceneCamera.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace Borealis
{
	SceneCamera::SceneCamera() : mAspectRatio(1.78f)
	{
		RecalculateProjection();
	}
	void SceneCamera::SetOrtho(float size, float nearClip, float farClip)
	{
		mOrthoSize = size;
		mOrthoNear = nearClip;
		mOrthoFar = farClip;
		mCameraType = CameraType::Orthographic;

		RecalculateProjection();
	}
	void SceneCamera::SetPers(float fov, float nearClip, float farClip)
	{
		mPerspFOV = fov;
		mPerspNear = nearClip;
		mPerspFar = farClip;
		mCameraType = CameraType::Perspective;

		RecalculateProjection();
	}
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		mWidth = (float)width;
		mHeight = (float)height;
		mAspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	glm::vec2 SceneCamera::GetViewPortSize() const
	{
		return glm::vec2(mWidth, mHeight);
	}
	float SceneCamera::GetFOV() const
	{
		if (mCameraType == CameraType::Perspective)
			return GetPerspFOV();
		else
			return 90.f;
	}
	float SceneCamera::GetNearPlane() const
	{
		if (mCameraType == CameraType::Perspective)
			return GetPerspNear();
		else
			return GetOrthoNear();
	}
	float SceneCamera::GetFarPlane() const
	{
		if (mCameraType == CameraType::Perspective)
			return GetPerspFar();
		else
			return GetOrthoFar();
	}
	float SceneCamera::GetAspectRatio() const
	{
		return mAspectRatio;
	}
	void SceneCamera::RecalculateProjection()
	{
		if (mCameraType == CameraType::Perspective)
		{
			mProjectionMatrix = glm::perspective(glm::radians(mPerspFOV), mAspectRatio, mPerspNear, mPerspFar);
		}
		else if (mCameraType == CameraType::Orthographic)
		{
			// Orthographic projection matrix (left, right, bottom, top, near, far)
			float orthoLeft = -mOrthoSize * mAspectRatio * 0.5f;
			float orthoRight = mOrthoSize * mAspectRatio * 0.5f;
			float orthoBottom = -mOrthoSize * 0.5f;
			float orthoTop = mOrthoSize * 0.5f;

			mProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, mOrthoNear, mOrthoFar);
		}
	}
}

