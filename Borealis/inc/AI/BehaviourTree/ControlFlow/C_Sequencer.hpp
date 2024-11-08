/******************************************************************************
/*!
\file       C_Sequencer.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Sample of sequencer node

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef C_SEQUENCER_HPP
#define C_SEQUENCER_HPP
#include "AI/BehaviourTree/BehaviourNode.hpp"
namespace Borealis
{
    class C_Sequencer : public BaseNode<C_Sequencer>
    {
    public:
        /*!***********************************************************************
            \brief
                Default constructor for the C_Sequencer node.
        *************************************************************************/
        C_Sequencer();

    protected:
        size_t currentIndex;  // Tracks the current child node being executed

        /*!***********************************************************************
            \brief
                Called when the sequencer node starts execution. This method 
                resets the currentIndex to begin processing child nodes.
        *************************************************************************/
        virtual void OnEnter() override;

        /*!***********************************************************************
            \brief
                Called to update the sequencer node during execution. It processes 
                child nodes in sequence, moving to the next one when the current 
                child succeeds.
            \param[in] dt
                The delta time for the update.
        *************************************************************************/
        virtual void OnUpdate(float dt, Entity& entity) override;
    };
}

#endif