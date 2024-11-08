/******************************************************************************
/*!
\file       D_Delay.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <AI/BehaviourTree/Decorator/D_Delay.hpp>
namespace Borealis
{
    D_Delay::D_Delay() : delay(0.0f)
    {}

    void D_Delay::OnEnter()
    {
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<> distr(1.5f, 2.0f);
        delay = distr(gen);

        BehaviourNode::OnEnter();
    }

    void D_Delay::OnUpdate(float dt, Entity& entity)
    {
        delay -= dt;

        if (delay < 0.0f)
        {
            Ref<BehaviourNode> child = mChildren.front();

            child->Tick(dt, entity);
        }
    }
}

