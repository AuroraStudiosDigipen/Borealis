/******************************************************************************
/*!
\file       C_test.hpp
\author     Your Name
\par        email: your.email@example.com
\date       November 09, 2024
\brief      Declares the class for the leaf node C_test

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
             
******************************************************************************/

#ifndef C_test_HPP
#define C_test_HPP

#include "AI/BehaviourTree/BehaviourNode.hpp"

namespace Borealis
{
    class C_test : public BaseNode<C_test>
    {
    public:
        C_test();

    protected:
        size_t currentIndex;  // Tracks the current child node being executed

        virtual void OnEnter() override;
        virtual void OnUpdate(float dt, Entity& entity) override;
    };
}

#endif
            