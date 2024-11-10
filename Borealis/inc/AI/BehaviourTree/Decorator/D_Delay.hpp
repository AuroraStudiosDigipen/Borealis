/******************************************************************************
/*!
\file       D_Delay.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef D_Delay_HPP
#define D_Delay_HPP
#include "AI/BehaviourTree/BehaviourNode.hpp"

namespace Borealis
{
    class D_Delay : public BaseNode<D_Delay>
    {
    public:
        D_Delay();

    protected:
        float delay;

        virtual void OnEnter() override;
        virtual void OnUpdate(float dt, Entity& entity) override;
    };
}

#endif
