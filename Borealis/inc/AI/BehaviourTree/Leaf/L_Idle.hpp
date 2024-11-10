/******************************************************************************
/*!
\file       L_Idle.cpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares the class for leaf node idle

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef L_IDLE_HPP
#define L_IDLE_HPP

#include "AI/BehaviourTree/BehaviourNode.hpp"
namespace Borealis
{
    class L_Idle : public BaseNode<L_Idle>
    {
    public:
        /*!***********************************************************************
            \brief
                Constructor for the L_Idle node. Initializes the idle node, setting 
                up its internal state.
         *************************************************************************/
        L_Idle();

    protected:
        float timer;  // Tracks the time the node has been idle

        /*!***********************************************************************
            \brief
                Called when the idle node starts execution.
                Resets the timer and prepares the node for idle behavior.
         *************************************************************************/
        virtual void OnEnter() override;

        /*!***********************************************************************
            \brief
                Updates the idle node's state. Keeps track of the time spent idle, 
                allowing the node to take action based on the elapsed time.
            \param[in] dt
            	The delta time for the update.
         *************************************************************************/
        virtual void OnUpdate(float dt, Entity& entity) override;
    };
}

#endif