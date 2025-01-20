/******************************************************************************/ 
/*! 
\file       AudioMixerPanel.cpp 
\author     Valerie Koh 
\par        email: v.koh@digipen.edu 
\date       November 2, 2024 
\brief      Defines the class Audio Mixer in Level Editor 

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
        AddAudioGroup("BGM");
        AddAudioGroup("SFX");
    }

    void AudioMixerPanel::ImGuiRender()
    {
        ImGui::Begin("Audio Mixer");

        // Master Volume Control
        if (ImGui::SliderFloat("Master Volume (dB)", &mMasterVolume, -80.0f, 0.0f))
        {
            AudioEngine::SetMasterVolume(mMasterVolume);
        }

        // Individual Group Volume Controls
        //if (ImGui::SliderFloat("BGM Volume (dB)", &mAudioGroups["BGM"].volume, -80.0f, 0.0f))
        //{
        //    AudioEngine::SetGroupVolume("BGM", mAudioGroups["BGM"].volume);
        //}
        //if (ImGui::SliderFloat("SFX Volume (dB)", &mAudioGroups["SFX"].volume, -80.0f, 0.0f))
        //{
        //    AudioEngine::SetGroupVolume("SFX", mAudioGroups["SFX"].volume);
        //}

        ImGui::Separator();

        // Render dynamic controls for each audio group
        for (auto& [groupName, groupData] : mAudioGroups)
        {
            float& volume = groupData.volume;
            if (ImGui::SliderFloat((groupName + " Volume (dB)").c_str(), &volume, -80.0f, 0.0f))
            {
                AudioEngine::SetGroupVolume(groupName, volume);
            }
        }

        // Add new audio group dynamically
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
            // Create a new audio group dynamically
            AudioEngine::CreateGroup(groupName);
            mAudioGroups[groupName] = { 1 }; // Default volume is 1.0 (full volume)
        }
    }
}
