/******************************************************************************/
/*!
\file		AudioEngine.cpp
\author 	Valerie Koh
\par    	email: v.koh\@digipen.edu
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

        //typedef std::map<std::string, FMOD::Sound*> SoundMap;
        typedef std::map<int, FMOD::Channel*> ChannelMap;

        //SoundMap mSounds;
        ChannelMap mChannels;
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
        // cout << "FMOD all good" << endl;
        return 0;
    }

    Implementation::Implementation()
    {
        mpSystem = NULL;
        mnNextChannelId = 1;
        // Create FMOD Core system
        ErrorCheck(FMOD::System_Create(&mpSystem));
        // Initialize FMOD Core system
        ErrorCheck(mpSystem->init(32, FMOD_INIT_PROFILE_ENABLE, NULL));
    }

    Implementation::~Implementation()
    {
        // Release FMOD Core system
        ErrorCheck(mpSystem->release());
    }

    void Implementation::Update()
    {
        /*AudioEngine::PlayAudio(".\assets\audio\meow.mp3");*/
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
        //auto tFoundIt = sgpImplementation->mSounds.find(strAudioName);
        //if (tFoundIt != sgpImplementation->mSounds.end())
        //    return Audio();

        FMOD_MODE eMode = FMOD_DEFAULT;
        eMode |= b3d ? FMOD_3D : FMOD_2D;
        eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

        FMOD::Sound* pSound = nullptr;
        ErrorCheck(sgpImplementation->mpSystem->createSound(strAudioName.c_str(), eMode, nullptr, &pSound));
        if (pSound)
        {
            //sgpImplementation->mSounds[strAudioName] = pSound;
            Audio audio;
            audio.AudioPath = strAudioName;
            audio.audioPtr = pSound;

            return audio;
        }

        return Audio();
    }

    void AudioEngine::UnLoadAudio(const std::string& strSoundName)
    {
        //auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        //if (tFoundIt == sgpImplementation->mSounds.end())
        //    return;

        //ErrorCheck(tFoundIt->second->release());
        //sgpImplementation->mSounds.erase(tFoundIt);
    }

    int AudioEngine::PlayAudio(const AudioSourceComponent& audio, const Vector3& vPosition, float fVolumedB, bool bMute, bool bLoop)
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
                FMOD_VECTOR fmodVelocity = { 0.0f, 0.0f, 0.0f };  // Set velocity if the sound is moving

                // Set the 3D attributes for the sound
                ErrorCheck(pChannel->set3DAttributes(&fmodPosition, &fmodVelocity));
            }

            // Other settings (volume, mute, etc.)
            ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
            ErrorCheck(pChannel->setPaused(false));
            ErrorCheck(pChannel->setMute(bMute));
            ErrorCheck(pChannel->setMode(bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));

            sgpImplementation->mChannels[nChannelId] = pChannel;
        }

        return nChannelId;
    }

    bool AudioEngine::isSoundPlaying(int nChannelId)
    {
        // Find the channel using the channel ID
        auto tFoundChannel = sgpImplementation->mChannels.find(nChannelId);
        if (tFoundChannel == sgpImplementation->mChannels.end())
            return false; // Channel not found

        bool bIsPlaying = false;
        // Check if the channel is currently playing
        ErrorCheck(tFoundChannel->second->isPlaying(&bIsPlaying));

        return bIsPlaying;
    }

    void AudioEngine::StopChannel(int nChannelId)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId); // Attempt to find the channel by ID

        // Check if the channel is found
        if (tFoundIt != sgpImplementation->mChannels.end())
        {
            FMOD::Channel* pChannel = tFoundIt->second;
            bool bIsPlaying = false;

            // Check if the channel is currently playing
            pChannel->isPlaying(&bIsPlaying);

            // If the channel is playing, stop it
            if (bIsPlaying)
            {
                ErrorCheck(pChannel->stop()); // Stop the channel
                sgpImplementation->mChannels.erase(nChannelId); // Remove channel from map
            }
        }
    }

    void AudioEngine::SetChannel3DPosition(int nChannelId, const Vector3& vPosition)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
        if (tFoundIt == sgpImplementation->mChannels.end())
            return;

        FMOD_VECTOR position = VectorToFmod(vPosition);
        ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
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
} // End of namespace Borealis