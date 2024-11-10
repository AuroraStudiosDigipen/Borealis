/******************************************************************************
/*!
\file       L_RotatingMotion.hpp
\author     Your Name
\par        email: your.email@example.com
\date       November 09, 2024
\brief      Declares the class for the leaf node L_RotatingMotion, which handles
            oscillating motion for an entity between defined X-axis boundaries.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
******************************************************************************/

#ifndef L_RotatingMotion_HPP
#define L_RotatingMotion_HPP

#include "AI/BehaviourTree/BehaviourNode.hpp"

namespace Borealis
{
    /*!***********************************************************************
    \class      L_RotatingMotion
    \brief      A leaf node in the behavior tree that handles oscillating motion
                for an entity. It moves the entity back and forth along the X-axis
                between predefined maximum and minimum boundaries.
    *************************************************************************/
    class L_RotatingMotion : public BaseNode<L_RotatingMotion>
    {
    public:
        /*!***********************************************************************
        \brief
            Constructor for the L_RotatingMotion leaf node.
        *************************************************************************/
        L_RotatingMotion();

    protected:
        /*!***********************************************************************
        \brief
            Called when the node is first entered in the behavior tree execution.
            Used for initialization of any state specific to this node.
        *************************************************************************/
        virtual void OnEnter() override;

        /*!***********************************************************************
        \brief
            Updates the entity's position by moving it left or right based on the
            current direction and X-axis boundaries.
        \param dt
            The delta time since the last update, used for smooth movement.
        \param entity
            A reference to the entity being controlled by the behavior tree.
        *************************************************************************/
        virtual void OnUpdate(float dt, Entity& entity) override;

        int direction = 1; /* < Direction of movement: 1 for right, -1 for left. */
        const float maxX = 15.0f; /*!< Maximum X boundary for oscillating motion. */
        const float minX = -15.0f; /*!< Minimum X boundary for oscillating motion. */
    };
}

#endif
