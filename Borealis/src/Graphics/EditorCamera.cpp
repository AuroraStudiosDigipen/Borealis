/******************************************************************************/
/*!
\file		EditorCamera.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 15, 2024
\brief		Defines the class for Editor Camera

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "BorealisPCH.hpp"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <Core/Core.hpp>
#include <Core/KeyCodes.hpp>
#include <Core/InputSystem.hpp>
#include <Graphics/EditorCamera.hpp>
#include <Graphics/Camera.hpp>



namespace Borealis
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: mFOV(fov), mAspectRatio(aspectRatio), mNearClip(nearClip), mFarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
	}

	void EditorCamera::SetFocalPoint(glm::vec3 point)
	{
		mFocalPoint = point;

		// Calculate bounding box to set distance in the future
		mDistance = 10; 
		mPosition = point - GetForwardDirection() * mDistance;
	}

	EditorCameraProps EditorCamera::GetProperties()
	{
		return { mDistance, mPitch, mYaw, mPosition, mFocalPoint };
	}

	void EditorCamera::SetProperties(EditorCameraProps prop)
	{
		mDistance = prop.mDistance;
		mPitch = prop.mPitch;
		mYaw = prop.mYaw;
		mPosition = prop.mPosition;
		mFocalPoint = prop.mFocalPoint;
		UpdateView();
	}

	void EditorCamera::UpdateProjection()
	{
		mAspectRatio = mViewportWidth / mViewportHeight;
		mProjectionMatrix = glm::perspective(glm::radians(mFOV), mAspectRatio, mNearClip, mFarClip);
	}

	void EditorCamera::UpdateView()
	{
		// m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
		mPosition = CalculatePosition();

		glm::quat orientation = GetOrientation();
		mViewMatrix = glm::translate(glm::mat4(1.0f), mPosition) * glm::mat4(orientation);
		mViewMatrix = glm::inverse(mViewMatrix);
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(mViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(mViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = mDistance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}


	void EditorCamera::UpdateFn(float dt)
	{
		if (InputSystem::IsKeyPressed(Key::LeftAlt))
		{
			auto mouse =  InputSystem::GetMousePos() ;
			glm::vec2 delta;
			
			delta.x = (mouse.first - mInitialMousePosition.x) * 0.003f;
			delta.y = (mouse.second - mInitialMousePosition.y) * 0.003f;
			mInitialMousePosition.x = mouse.first;
			mInitialMousePosition.y = mouse.second;

			if (InputSystem::IsMouseButtonPressed(Mouse::ButtonMiddle))
				MousePan(delta);
			else if (InputSystem::IsMouseButtonPressed(Mouse::ButtonLeft))
				MouseRotate(delta);
			else if (InputSystem::IsMouseButtonPressed(Mouse::ButtonRight))
				MouseZoom(delta.y);
		}

		UpdateView();
	}

	void EditorCamera::EventFn(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<EventMouseScrolled>(BIND_EVENT(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(EventMouseScrolled& e)
	{
		float delta = static_cast<float>(e.GetYOffset()) * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		mFocalPoint += -GetRightDirection() * delta.x * xSpeed * mDistance;
		mFocalPoint += GetUpDirection() * delta.y * ySpeed * mDistance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		mYaw += yawSign * delta.x * RotationSpeed();
		mPitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		mDistance -= delta * ZoomSpeed();
		if (mDistance < 1.0f)
		{
			mFocalPoint += GetForwardDirection();
			mDistance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return mFocalPoint - GetForwardDirection() * mDistance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-mPitch, -mYaw, 0.0f));
	}

}
