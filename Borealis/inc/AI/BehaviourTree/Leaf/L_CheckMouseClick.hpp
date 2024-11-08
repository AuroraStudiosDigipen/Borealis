/******************************************************************************
/*!
\file       L_CheckMouseClick.cpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Sample of mouse click node

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef L_CHECKMOUSECLICK_HPP
#define L_CHECKMOUSECLICK_HPP
#include "AI/BehaviourTree/BehaviourNode.hpp"
namespace Borealis
{
    class L_CheckMouseClick : public BaseNode<L_CheckMouseClick>
    {
    protected:
         /*!***********************************************************************
            \brief
                Updates the node's logic to check for a mouse click event.
                This function will be called on each frame to detect if a mouse 
                click occurred. If a click is detected, it will proceed with 
                node-specific logic.
            \param[in] dt
                The delta time for the update.
         *************************************************************************/
        virtual void OnUpdate(float dt, Entity& entity) override;
    };
}

#endif