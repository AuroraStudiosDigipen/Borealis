/******************************************************************************
            /*!
            \file       C_.cpp
            \author     Your Name
            \par        email: your.email@example.com
            \date       October 30, 2024
            \brief      Defines the class for the leaf node C_

            Copyright (C) 2024 DigiPen Institute of Technology.
            Reproduction or disclosure of this file or its contents without the
            prior written consent of DigiPen Institute of Technology is prohibited.
             */
             ******************************************************************************/

            #include <BorealisPCH.hpp>
            #include "C_.hpp"

            namespace Borealis
            {
                C_::C_()
                {
                    // Constructor implementation
                }

                void C_::OnEnter()
                {
                    currentIndex = 0;
                    BehaviourNode::OnEnter();
                }

                void C_::OnUpdate(float dt)
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
            