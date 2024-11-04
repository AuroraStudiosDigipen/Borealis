/******************************************************************************
            /*!
            \file       C_.hpp
            \author     Your Name
            \par        email: your.email@example.com
            \date       October 30, 2024
            \brief      Declares the class for the leaf node C_

            Copyright (C) 2024 DigiPen Institute of Technology.
            Reproduction or disclosure of this file or its contents without the
            prior written consent of DigiPen Institute of Technology is prohibited.
             */
             ******************************************************************************/

            #ifndef C__HPP
            #define C__HPP

            #include "AI/BehaviourTree/BehaviourNode.hpp"

            namespace Borealis
            {
                class C_ : public BaseNode<C_>
                {
                public:
                    C_();

                protected:
                    size_t currentIndex;  // Tracks the current child node being executed

                    virtual void OnEnter() override;
                    virtual void OnUpdate(float dt) override;
                };
            }

            #endif
            