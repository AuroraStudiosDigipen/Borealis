/******************************************************************************
/*!
\file       NodesDef.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares macros to register behaviouor nodes

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef NodesDef_HPP
#define NodesDef_HPP
#include <Core/Core.hpp>
#include <AI/BehaviourTree/RegisterNodes.hpp>
// Include all the nodes here
#include <AI/BehaviourTree/ControlFlow/C_Sequencer.hpp>

#include<AI/BehaviourTree/Decorator/D_Delay.hpp>

#include <AI/BehaviourTree/Leaf/L_Idle.hpp>
#include <AI/BehaviourTree/Leaf/L_CheckMouseClick.hpp>
#include <AI/BehaviourTree/Leaf/L_RotatingMotion.hpp>
namespace Borealis
{
#ifndef REGISTER_ALL_NODES
	// Macro definitions to register nodes dynamically, add "\" at the end of non-last lines
#define REGISTER_ALL_NODES \
		REGISTER_CONTROLFLOW(C_Sequencer) \
		REGISTER_DECORATOR(D_Delay) \
		REGISTER_LEAF(L_Idle) \
		REGISTER_LEAF(L_CheckMouseClick)\
		REGISTER_LEAF(L_RotatingMotion)

#endif
}

#endif
