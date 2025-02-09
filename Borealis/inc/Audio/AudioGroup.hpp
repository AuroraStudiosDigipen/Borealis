/******************************************************************************/
/*!
\file		AudioMixerPanel.cpp
\author 	Valerie Koh
\par    	email: v.koh@digipen.edu
\date   	November 11, 2024
\brief		Declares the AudioGroup enum to represent different audio groups 

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
#include <Core/Core.hpp>
#include <Assets/Asset.hpp>

#ifndef AUDIO_GROUP_HPP
#define AUDIO_GROUP_HPP

namespace Borealis {

    // Enum to represent different audio groups
    namespace AudioGroups {
        const std::string Master = "Master";
        const std::string BGM = "BGM";
        const std::string SFX = "SFX";
    }

} // End of Borealis namespace

#endif // AUDIO_GROUP_HPP

