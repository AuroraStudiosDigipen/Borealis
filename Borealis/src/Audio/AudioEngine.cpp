/******************************************************************************/
/*!
\file		AudioEngine.cpp
\author 	Valerie Koh
\par    	email: v.koh@digipen.edu
\date   	September 11, 2024
\brief      Implements the AudioEngine class for handling audio functionality

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <FMOD/fmod.hpp>
#include <Audio/Audio.hpp>
#include "Audio/AudioEngine.hpp"
#include <Scene/Components.hpp>

namespace Borealis
{
    struct Implementation {
        Implementation();
        ~Implementation();

        void Update();

        FMOD::System* mpSystem;

        int mnNextChannelId;
        typedef std::map<int, FMOD::Channel*> ChannelMap;
        typedef std::map<int, FMOD::ChannelGroup*> ChannelGroupMap;

        ChannelMap mChannels;
        ChannelGroupMap mChannelGroups;
        typedef std::map<FMOD::Sound*, int> AudioGroupMap;
        AudioGroupMap mAudioGroupMap;

    };

    static FMOD_VECTOR VectorToFmod(const Vector3& vPosition)
    {
        FMOD_VECTOR fVec;
        fVec.x = vPosition.x;
        fVec.y = vPosition.y;
        fVec.z = vPosition.z;
        return fVec;
    }

    static int ErrorCheck(FMOD_RESULT result)
    {
        if (result != FMOD_OK)
        {
            std::cout << "FMOD ERROR " << result << std::endl;
            return 1;
        }
        return 0;
    }

    Implementation::Implementation()
    {
        mpSystem = nullptr;
        mnNextChannelId = 1;

        // Create FMOD Core system
        ErrorCheck(FMOD::System_Create(&mpSystem));

        // Initialize FMOD Core system
        ErrorCheck(mpSystem->init(32, FMOD_INIT_PROFILE_ENABLE, nullptr));
    }

    Implementation::~Implementation()
    {
        // Release FMOD Core system
        ErrorCheck(mpSystem->release());
    }

    void Implementation::Update()
    {
        std::vector<ChannelMap::iterator> pStoppedChannels;
        for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
        {
            bool bIsPlaying = false;
            it->second->isPlaying(&bIsPlaying);
            if (!bIsPlaying)
            {
                pStoppedChannels.push_back(it);
            }
        }

        for (auto& it : pStoppedChannels)
        {
            mChannels.erase(it);
        }
        ErrorCheck(mpSystem->update());
    }

    Implementation* sgpImplementation = nullptr;
    int AudioEngine::mNextGroupId = 0;

    void AudioEngine::Init()
    {
        sgpImplementation = new Implementation;
    }

    void AudioEngine::Update()
    {
        sgpImplementation->Update();
    }

    Audio AudioEngine::LoadAudio(const std::string& strAudioName, bool b3d, bool bLooping, bool bStream)
    {
        FMOD_MODE eMode = FMOD_DEFAULT;
        eMode |= b3d ? FMOD_3D : FMOD_2D;
        eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

        FMOD::Sound* pSound = nullptr;
        ErrorCheck(sgpImplementation->mpSystem->createSound(strAudioName.c_str(), eMode, nullptr, &pSound));

        if (pSound)
        {
            Audio audio;
            audio.AudioPath = strAudioName;
            audio.audioPtr = pSound;
            return audio;
        }

        return Audio();
    }

    void AudioEngine::UnLoadAudio(const std::string& strSoundName)
    {
        // Here you can add your logic for unloading sounds
    }

    int AudioEngine::PlayAudio(const AudioSourceComponent& audio, const Vector3& vPosition, float fVolumedB, bool bMute, bool bLoop, int groupId)
    {
        int nChannelId = sgpImplementation->mnNextChannelId++;

        FMOD::Sound* fmodSound = audio.audio->audioPtr;
        if (!fmodSound) return -1;

        FMOD::Channel* pChannel = nullptr;
        ErrorCheck(sgpImplementation->mpSystem->playSound(fmodSound, nullptr, true, &pChannel));

        if (pChannel)
        {
            // Check if the sound is in 3D mode
            FMOD_MODE currMode;
            fmodSound->getMode(&currMode);
            if (currMode & FMOD_3D)
            {
                FMOD_VECTOR fmodPosition = VectorToFmod(vPosition);
                FMOD_VECTOR fmodVelocity = { 0.0f, 0.0f, 0.0f };

                // Set the 3D attributes for the sound
                ErrorCheck(pChannel->set3DAttributes(&fmodPosition, &fmodVelocity));
            }

            // Other settings (volume, mute, etc.)
            ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
            ErrorCheck(pChannel->setPaused(false));
            ErrorCheck(pChannel->setMute(bMute));
            ErrorCheck(pChannel->setMode(bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));

            // Assign the channel to the specified group using setChannelGroup
            if (groupId != -1)
            {
                auto itGroup = sgpImplementation->mChannelGroups.find(groupId);
                if (itGroup != sgpImplementation->mChannelGroups.end())
                {
                    FMOD::ChannelGroup* pGroup = itGroup->second;
                    ErrorCheck(pChannel->setChannelGroup(pGroup));
                }

                // Record the group for this audio
                sgpImplementation->mAudioGroupMap[fmodSound] = groupId;
            }

            sgpImplementation->mChannels[nChannelId] = pChannel;
        }

        return nChannelId;
    }



    bool AudioEngine::isSoundPlaying(int nChannelId)
    {
        auto tFoundChannel = sgpImplementation->mChannels.find(nChannelId);
        if (tFoundChannel == sgpImplementation->mChannels.end())
            return false;

        bool bIsPlaying = false;
        ErrorCheck(tFoundChannel->second->isPlaying(&bIsPlaying));

        return bIsPlaying;
    }

    void AudioEngine::StopChannel(int nChannelId)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);

        if (tFoundIt != sgpImplementation->mChannels.end())
        {
            FMOD::Channel* pChannel = tFoundIt->second;
            bool bIsPlaying = false;

            pChannel->isPlaying(&bIsPlaying);

            if (bIsPlaying)
            {
                ErrorCheck(pChannel->stop());
                sgpImplementation->mChannels.erase(nChannelId);
            }
        }
    }

    void AudioEngine::StopAllChannels()
    {
        for (auto& channelPair : sgpImplementation->mChannels)
        {
            StopChannel(channelPair.first);
        }
    }

    void AudioEngine::SetChannel3DPosition(int nChannelId, const Vector3& vPosition)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
        if (tFoundIt == sgpImplementation->mChannels.end())
            return;

        FMOD_VECTOR position = VectorToFmod(vPosition);
        ErrorCheck(tFoundIt->second->set3DAttributes(&position, nullptr));
    }

    void AudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
        if (tFoundIt == sgpImplementation->mChannels.end())
            return;

        ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
    }

    void AudioEngine::SetListenerPosition(const Vector3& position, const Vector3& forward, const Vector3& up)
    {
        FMOD_VECTOR pos = VectorToFmod(position);
        FMOD_VECTOR fwd = VectorToFmod(forward);
        FMOD_VECTOR upVec = VectorToFmod(up);

        ErrorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(0, &pos, nullptr, &fwd, &upVec));
    }

    float AudioEngine::dbToVolume(float dB)
    {
        return powf(10.0f, 0.05f * dB);
    }

    float AudioEngine::VolumeTodb(float volume)
    {
        return 20.0f * log10f(volume);
    }

    void AudioEngine::Shutdown()
    {
        delete sgpImplementation;
    }

    void AudioEngine::SetMasterVolume(float fVolumedB)
    {
        // Convert from dB to linear volume
        float volume = dbToVolume(fVolumedB);

        // Get the master channel group from the system
        FMOD::ChannelGroup* masterGroup = nullptr;
        ErrorCheck(sgpImplementation->mpSystem->getMasterChannelGroup(&masterGroup));

        // Set the volume for the master channel group
        ErrorCheck(masterGroup->setVolume(volume));
    }


    int AudioEngine::CreateGroup(const std::string& groupName)
    {
        FMOD::ChannelGroup* pGroup = nullptr;
        ErrorCheck(sgpImplementation->mpSystem->createChannelGroup(groupName.c_str(), &pGroup));

        int groupId = mNextGroupId++;  // Use the static variable to generate unique group IDs
        sgpImplementation->mChannelGroups[groupId] = pGroup;

        return groupId;
    }


    void AudioEngine::SetGroupVolume(int groupId, float fVolumedB)
    {
        // Find the channel group in the map using groupId
        auto it = sgpImplementation->mChannelGroups.find(groupId);
        if (it != sgpImplementation->mChannelGroups.end())
        {
            // Set the volume for this channel group, converting from dB to linear scale
            ErrorCheck(it->second->setVolume(dbToVolume(fVolumedB)));
        }
        else
        {
            // Handle the case where the groupId doesn't exist (optional logging or error handling)
            std::cerr << "Error: Group ID " << groupId << " not found." << std::endl;
        }
    }

    int AudioEngine::GetGroupIdForAudio(FMOD::Sound* fmodSound)
    {
        auto it = sgpImplementation->mAudioGroupMap.find(fmodSound);
        if (it != sgpImplementation->mAudioGroupMap.end())
        {
            return it->second;  // Return the group ID
        }

        // If no group is found, return -1 or handle the case accordingly
        return -1;
    }
}
