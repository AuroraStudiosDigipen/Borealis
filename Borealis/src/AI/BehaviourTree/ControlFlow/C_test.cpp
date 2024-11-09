/******************************************************************************
/*!
\file       C_test.cpp
\author     Your Name
\par        email: your.email@example.com
\date       November 09, 2024
\brief      Defines the class for the control flow node C_test

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
******************************************************************************/

#include <BorealisPCH.hpp>
#include  <AI/BehaviourTree/ControlFlow/C_test.hpp>

namespace Borealis
{
    C_test::C_test()
    {
        // Constructor implementation
    }

    void C_test::OnEnter()
    {
        currentIndex = 0;
        BehaviourNode::OnEnter();
    }

    void C_test::OnUpdate(float dt, Entity& entity)
    {
        // if any child fails, the node fails
        // if all children succeed, the node succeeds
        Ref<BehaviourNode> currentNode = mChildren[currentIndex];
        currentNode->Tick(dt);

        if (currentNode->HasFailed() == true)
        {
            OnFailure();
        }
        else if (currentNode->HasSucceeded() == true)
        {
            // move to the next node
            ++currentIndex;

            // if we hit the size, then all nodes suceeded
            if (currentIndex == mChildren.size())
            {
                OnSuccess();
            }
        }
    }
}
            