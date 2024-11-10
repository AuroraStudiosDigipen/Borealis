/******************************************************************************
/*!
\file       L_RotatingMotion.cpp
\author     Your Name
\par        email: your.email@example.com
\date       November 09, 2024
\brief      Defines the class for the leaf node L_RotatingMotion

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
******************************************************************************/

#include <BorealisPCH.hpp>
#include <AI/BehaviourTree/Leaf/L_RotatingMotion.hpp>

namespace Borealis
{
    L_RotatingMotion::L_RotatingMotion()
    {
        // Constructor implementation
    }

    void L_RotatingMotion::OnEnter()
    {
        // OnEnter implementation
        BehaviourNode::OnLeafEnter();
    }

    void L_RotatingMotion::OnUpdate(float dt, Entity& entity)
    {
        float speed = 2.0f;
        if (dt == 0.0f) dt = 0.016f;  // Default to roughly 60 FPS

        // Access the entity's transform component
        auto& transforms = entity.GetComponent<TransformComponent>();
        glm::vec3 position = transforms.Translate;

        // Move position based on current direction
        position.x += direction * speed * dt;

        // Check if we need to flip direction
        if (position.x >= maxX)
        {
            position.x = maxX;  // Clamp to max boundary
            direction = -1;     // Switch to move left
            OnSuccess();        // Trigger success when reaching max boundary
        }
        else if (position.x <= minX)
        {
            position.x = minX;  // Clamp to min boundary
            direction = 1;      // Switch to move right
            OnSuccess();        // Trigger success when reaching min boundary
        }

        // Update the entity's translation position
        transforms.Translate = position;
    }
}
            