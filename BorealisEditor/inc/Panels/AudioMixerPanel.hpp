/******************************************************************************/
/*!
\file		AudioMixerPanel.hpp
\author 	Valerie Koh
\par    	email: v.koh@digipen.edu
\date   	November 2, 2024
\brief		Declares the class Audio Mixer in Level Editor

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef AUDIO_MIXER_PANEL_HPP
#define AUDIO_MIXER_PANEL_HPP

#include <unordered_map>
#include <string>
#include <imgui.h>
#include <Audio/AudioEngine.hpp>
#include <Scene/scene.hpp>
#include <Scene/Entity.hpp>
#include <Core/core.hpp>

namespace Borealis
{
    class AudioMixerPanel
    {
    public:
        /*!***********************************************************************
        \brief Constructs the Audio Mixer Panel
        *************************************************************************/
        AudioMixerPanel();

        /*!***********************************************************************
        \brief Renders the Audio Mixer Panel in ImGui
        *************************************************************************/
        void ImGuiRender();

    private:
        /*!***********************************************************************
        \brief Stores group data including group ID and volume level
        *************************************************************************/
        struct GroupData
        {
            int id;         /*!< Group ID assigned by AudioEngine */
            float volume;   /*!< Current volume level (0.0 to 1.0) */
        };

        /*!***********************************************************************
        \brief Adds a new audio group
        \param groupName The name of the audio group
        *************************************************************************/
        void AddAudioGroup(const std::string& groupName);

        /*!***********************************************************************
        \brief Adjusts volume of an audio group
        \param groupName The name of the audio group
        \param volume The volume level to set (0.0 to 1.0)
        *************************************************************************/
        void SetGroupVolume(const std::string& groupName, float volume);

        // Data

        std::unordered_map<std::string, GroupData> mAudioGroups; /*!< Map of audio group names to GroupData */
        float mMasterVolume = 1.0f; /*!< Master volume level (0.0 to 1.0) */
        //int mDefaultGroupId;
    };
}

#endif // AUDIO_MIXER_PANEL_HPP
