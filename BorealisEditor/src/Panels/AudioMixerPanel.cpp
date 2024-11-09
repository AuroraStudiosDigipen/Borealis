/******************************************************************************/
/*!
\file		AudioMixerPanel.cpp
\author 	Valerie Koh
\par    	email: v.koh@digipen.edu
\date   	November 2, 2024
\brief		Defines the class Audio Mixer in Level Editor

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "Panels/AudioMixerPanel.hpp"
#include <imgui.h>
#include <Audio/AudioEngine.hpp>

namespace Borealis
{
    AudioMixerPanel::AudioMixerPanel()
    {
        // Initialize some default audio groups
        AddAudioGroup("Music");
        AddAudioGroup("SFX");
        AddAudioGroup("Dialogue");
    }

    void AudioMixerPanel::ImGuiRender()
    {
        ImGui::Begin("Audio Mixer");

        // Master volume
        if (ImGui::SliderFloat("Master Volume", &mMasterVolume, 0.0f, 1.0f))
        {
            AudioEngine::SetMasterVolume(mMasterVolume);
        }

        ImGui::Separator();

        // Render each audio group
        for (auto& [groupName, groupData] : mAudioGroups)
        {
            float& volume = groupData.volume;
            if (ImGui::SliderFloat(groupName.c_str(), &volume, 0.0f, 1.0f))
            {
                SetGroupVolume(groupName, volume);
            }
        }

        // Add group button
        static char newGroupName[32] = "";
        ImGui::InputText("New Group", newGroupName, sizeof(newGroupName));
        if (ImGui::Button("Add Group"))
        {
            AddAudioGroup(newGroupName);
            newGroupName[0] = '\0'; // Clear input
        }

        ImGui::End();
    }

    void AudioMixerPanel::AddAudioGroup(const std::string& groupName)
    {
        if (mAudioGroups.find(groupName) == mAudioGroups.end())
        {
            int groupId = AudioEngine::CreateGroup(groupName);
            mAudioGroups[groupName] = { groupId, 1.0f }; // Store group ID and default volume
        }
    }

    void AudioMixerPanel::SetGroupVolume(const std::string& groupName, float volume)
    {
        auto it = mAudioGroups.find(groupName);
        if (it != mAudioGroups.end())
        {
            int groupId = it->second.id;
            it->second.volume = volume;
            AudioEngine::SetGroupVolume(groupId, volume);
        }
    }
}
