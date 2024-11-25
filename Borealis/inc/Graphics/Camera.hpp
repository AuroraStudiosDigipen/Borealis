/******************************************************************************/
/*!
\file		Camera.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 7, 2024
\brief		Declares the class for Camera

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <glm/glm.hpp>

namespace Borealis
{
	class Camera
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor for Camera
		*************************************************************************/
		Camera() : mProjectionMatrix(glm::mat4(1.0f)) {};

		/*!***********************************************************************
			\brief
				Constructor for Camera
			\param projection
				Projection matrix for the camera
		*************************************************************************/
		Camera(glm::mat4 projection) : mProjectionMatrix(projection) {};

		/*!***********************************************************************
			\brief
				Destructor for Camera
		*************************************************************************/
		virtual ~Camera() = default;

		/*!***********************************************************************
			\brief
				Sets the projection matrix for the camera
			\param projectionMatrix
				Projection matrix for the camera
		*************************************************************************/
		void SetProjectionMatrix(const glm::mat4& projectionMatrix) { mProjectionMatrix = projectionMatrix; };

		/*!***********************************************************************
			\brief
				Gets the projection matrix for the camera
			\return
				Projection matrix for the camera
		*************************************************************************/
		const glm::mat4& GetProjectionMatrix() const { return mProjectionMatrix; };

		virtual glm::vec2 GetViewPortSize() const = 0;

		virtual float GetFOV() const = 0;
		virtual float GetNearPlane() const = 0;
		virtual float GetFarPlane() const = 0;

		virtual float GetAspectRatio() const = 0;
	protected:
		glm::mat4 mProjectionMatrix; //!< Projection matrix for the camera
	}; // class Camera
} // namespace Borealis

#endif