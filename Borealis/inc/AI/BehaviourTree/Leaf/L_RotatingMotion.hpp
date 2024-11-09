/******************************************************************************
/*!
\file       L_RotatingMotion.hpp
\author     Your Name
\par        email: your.email@example.com
\date       November 09, 2024
\brief      Declares the class for the leaf node L_RotatingMotion

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
******************************************************************************/

#ifndef L_RotatingMotion_HPP
#define L_RotatingMotion_HPP

#include "AI/BehaviourTree/BehaviourNode.hpp"

namespace Borealis
{
    class L_RotatingMotion : public BaseNode<L_RotatingMotion>
    {
    public:
        L_RotatingMotion();

    protected:
        // Add member variables and methods specific to the leaf node

        virtual void OnEnter() override;
        virtual void OnUpdate(float dt, Entity& entity) override;
        int direction = 1;  // Direction for this specific entity instance
        const float maxX = 15.0f;  // Maximum world X coordinate
        const float minX = -15.0f; // Minimum world X coordinate
    };
}

#endif
            