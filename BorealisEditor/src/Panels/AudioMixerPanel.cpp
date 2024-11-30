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

        // Render each audio group and control its volume
        /*for (auto& [group, groupData] : mAudioGroups)
        {
            const char* groupName = GetAudioGroupName(group);
            float& volume = groupData.volume;
            if (ImGui::SliderFloat(groupName, &volume, 0.0f, 1.0f))
            {
                SetGroupVolume(group, volume);
            }
        }*/

        //// Add group assignment for each AudioSourceComponent
        //for (auto& entity : mRegistry.view<AudioSourceComponent>())
        //{
        //    auto& audioSource = mRegistry.get<AudioSourceComponent>(entity);
        //    
        //    // Create dropdown for selecting the group
        //    if (ImGui::BeginCombo("Select Group", audioSource.groupName.c_str()))
        //    {
        //        for (auto& [groupName, groupData] : mAudioGroups)
        //        {
        //            bool isSelected = (groupName == audioSource.groupName);
        //            if (ImGui::Selectable(groupName.c_str(), isSelected))
        //            {
        //                audioSource.groupName = groupName;  // Assign the selected group to the audio source
        //            }
        //        }
        //        ImGui::EndCombo();
        //    }
        //}

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
            int groupId = (groupName == "Music") ? 0 : AudioEngine::CreateGroup(groupName);  // Set group ID to 0 for "Music"
            mAudioGroups[groupName] = { groupId, 1.0f }; // Store group ID and default volume
        }
    }

    //void AudioMixerPanel::SetGroupVolume(const AudioGroup group, float volume)
    //{
    //    int groupId = static_cast<int>(group);  // Convert enum to integer
    //    auto it = mAudioGroups.find(groupId);
    //    if (it != mAudioGroups.end())
    //    {
    //        it->second.volume = volume;
    //        AudioEngine::SetGroupVolume(groupId, volume);
    //    }
    //}
}
