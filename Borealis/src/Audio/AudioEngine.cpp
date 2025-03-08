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
#include <Audio/AudioGroup.hpp>
#include <vector>

namespace Borealis
{
    struct FadeEffect {
        int channelId;
        float targetVolume;
        float startVolume;
        float currentTime;
        float duration;
        bool fadeIn;

        FadeEffect(int chId, float startVol, float targetVol, float dur, bool in)
            : channelId(chId), startVolume(startVol), targetVolume(targetVol),
            currentTime(0.0f), duration(dur), fadeIn(in) {}
    };


    struct Implementation {
        Implementation();
        ~Implementation();

        void Update();

        FMOD::System* mpSystem;

        int mnNextChannelId;
        typedef std::map<int, FMOD::Channel*> ChannelMap;
        typedef std::map<std::string, FMOD::ChannelGroup*> ChannelGroupMap;

        ChannelMap mChannels;
        ChannelGroupMap mChannelGroups;
        std::vector<FadeEffect> mFades;
        //typedef std::map<FMOD::Sound*, int> AudioGroupMap;
        //AudioGroupMap mAudioGroupMap;

    };

    static FMOD_VECTOR VectorToFmod(const glm::vec3& vPosition)
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
            std::cerr << "FMOD ERROR " << result << std::endl;
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

        FMOD::ChannelGroup* bgmGroup = nullptr;
        FMOD::ChannelGroup* sfxGroup = nullptr;
        FMOD::ChannelGroup* masterGroup = nullptr;

        ErrorCheck(mpSystem->createChannelGroup(Borealis::AudioGroups::BGM.c_str(), &bgmGroup));
        ErrorCheck(mpSystem->createChannelGroup(Borealis::AudioGroups::SFX.c_str(), &sfxGroup));
        ErrorCheck(mpSystem->getMasterChannelGroup(&masterGroup));

        mChannelGroups[Borealis::AudioGroups::BGM] = bgmGroup;
        mChannelGroups[Borealis::AudioGroups::SFX] = sfxGroup;
        mChannelGroups[Borealis::AudioGroups::Master] = masterGroup;

        //FMOD::ChannelGroup* masterGroup = nullptr;
        //ErrorCheck(mpSystem->getMasterChannelGroup(&masterGroup));

        if (masterGroup)
        {
            ErrorCheck(masterGroup->addGroup(bgmGroup));
            ErrorCheck(masterGroup->addGroup(sfxGroup));
        }
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
                BOREALIS_CORE_INFO("Stop channel {}", it->first);
            }
        }

        for (auto& it : pStoppedChannels)
        {
            mChannels.erase(it);
        }

        float deltaTime = 0.016f; // Assume ~60 FPS

        auto it = mFades.begin();
        while (it != mFades.end())
        {
            it->currentTime += deltaTime;
            float progress = glm::clamp(it->currentTime / it->duration, 0.0f, 1.0f);

            float newVolume;
            if (it->fadeIn)
            {
                // Linear interpolation for fade-in (as before)
                newVolume = glm::mix(it->startVolume, it->targetVolume, progress);
            }
            else
            {
                // Convert volumes to dB for logarithmic fade-out
                constexpr float MIN_VOLUME = 0.0001f; // -80 dB (near silence)
                float startVol = glm::max(it->startVolume, MIN_VOLUME);
                float targetVol = glm::max(it->targetVolume, MIN_VOLUME);

                float startDB = 20.0f * log10f(startVol);
                float targetDB = 20.0f * log10f(targetVol);
                float currentDB = glm::mix(startDB, targetDB, progress);
                newVolume = powf(10.0f, currentDB / 20.0f);
            }

            auto chIt = mChannels.find(it->channelId);
            if (chIt != mChannels.end())
            {
                ErrorCheck(chIt->second->setVolume(newVolume));
            }

            // Stop when fade-out completes (progress >=1)
            if (progress >= 1.0f)
            {
                if (!it->fadeIn)
                {
                    if (chIt != mChannels.end())
                    {
                        chIt->second->stop();
                        mChannels.erase(chIt);
                    }
                }
                it = mFades.erase(it);
            }
            else
            {
                ++it;
            }
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

    void AudioEngine::Set3DListenerAndOrientation(const glm::vec3& vPos, float fVolumedB)
    {
        FMOD_VECTOR fmodPosition = VectorToFmod(vPos);

        // Default orientation: Forward (looking along +Z), Up (pointing along +Y)
        FMOD_VECTOR fmodForward = { 0.0f, 0.0f, 1.0f };
        FMOD_VECTOR fmodUp = { 0.0f, 1.0f, 0.0f };

        // Update FMOD Listener Position
        ErrorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(0, &fmodPosition, nullptr, &fmodForward, &fmodUp));
    }

    int AudioEngine::PlayAudio(AudioSourceComponent& audio, const glm::vec3& vPosition, float fVolumedB, bool bMute, bool bLoop, const std::string& groupName)
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
            auto itGroup = sgpImplementation->mChannelGroups.find(groupName);
            if (itGroup != sgpImplementation->mChannelGroups.end())
            {
                ErrorCheck(pChannel->setChannelGroup(itGroup->second));
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
        // Stop all tracked channels
        for (auto it = sgpImplementation->mChannels.begin(); it != sgpImplementation->mChannels.end(); ++it)
        {
            FMOD::Channel* pChannel = it->second;
            pChannel->stop();
        }

        sgpImplementation->mChannels.clear(); // Clear the map as all channels are stopped

        // Stop all channels globally via the master group
        FMOD::ChannelGroup* masterGroup = nullptr;
        ErrorCheck(sgpImplementation->mpSystem->getMasterChannelGroup(&masterGroup));
        if (masterGroup)
        {
            ErrorCheck(masterGroup->stop());
        }
    }

    void AudioEngine::SetChannel3DPosition(int nChannelId, const glm::vec3& vPosition)
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

    void AudioEngine::SetListenerPosition(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up)
    {
        FMOD_VECTOR pos = VectorToFmod(position);
        FMOD_VECTOR fwd = VectorToFmod(forward);
        FMOD_VECTOR upVec = VectorToFmod(up);

        ErrorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(0, &pos, nullptr, &fwd, &upVec));
    }

    float AudioEngine::dbToVolume(float dB)
    {
        return powf(10.0f, dB / 20.0f);  // Correct dB to linear conversion
    }

    float AudioEngine::dbToVolume2(float sliderValue)
    {
        // Clamp sliderValue between 0 and 1
        sliderValue = glm::clamp(sliderValue, 0.0f, 1.0f);

        // Apply a curve to make volume changes more gradual
        float curvedValue = powf(sliderValue, 0.5f); // sqrt to slow down the drop-off

        // Convert the curved slider to a dB range (-80 dB to 0 dB)
        float minDB = -80.0f;  // Minimum volume
        float maxDB = 0.0f;    // Maximum volume
        float dB = minDB + curvedValue * (maxDB - minDB);

        // Convert dB to linear volume
        float volume = powf(10.0f, dB / 20.0f);

        // Prevent invalid volumes
        if (std::isnan(volume) || std::isinf(volume))
        {
            volume = 0.0001f;
        }

        return volume;
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
        float volume = dbToVolume2(fVolumedB);

        FMOD::ChannelGroup* masterGroup = nullptr;
        if (ErrorCheck(sgpImplementation->mpSystem->getMasterChannelGroup(&masterGroup)) == 0 && masterGroup)
        {
            ErrorCheck(masterGroup->setVolume(volume));
        }
        else
        {
            std::cerr << "Error: Could not retrieve master channel group!" << std::endl;
        }
    }


    int AudioEngine::CreateGroup(const std::string& groupName)
    {
        FMOD::ChannelGroup* pGroup = nullptr;
        if (ErrorCheck(sgpImplementation->mpSystem->createChannelGroup(groupName.c_str(), &pGroup)) == 0)
        {
            sgpImplementation->mChannelGroups[groupName] = pGroup;
            return 0; // Success
        }

        std::cerr << "Error creating group: " << groupName << std::endl;
        return -1; // Failure
    }

    void AudioEngine::SetGroupVolume(const std::string& groupName, float fVolumedB)
    {
        float volume = dbToVolume2(fVolumedB);

        auto it = sgpImplementation->mChannelGroups.find(groupName);
        if (it != sgpImplementation->mChannelGroups.end())
        {
            ErrorCheck(it->second->setVolume(volume));

            float appliedVolume = 0.0f;
            it->second->getVolume(&appliedVolume);
        }
        else
        {
            std::cerr << "Error: Group \"" << groupName << "\" not found!" << std::endl;
        }
    }
    Ref<Asset> AudioEngine::Load(std::filesystem::path const& cachePath, AssetMetaData const& assetMetaData)
    {
        Audio audio = LoadAudio((cachePath / std::to_string(assetMetaData.Handle)).string());
        return MakeRef<Audio>(audio);
    }

    int AudioEngine::Play(Ref<Audio> audio, const glm::vec3& position, float volumeDB, bool looping, const std::string& groupName)
    {
        if (!audio || !audio->audioPtr)
        {
            std::cerr << "Invalid audio reference" << std::endl;
            return -1;
        }

        int channelId = sgpImplementation->mnNextChannelId++;

        FMOD::Channel* channel = nullptr;
        FMOD::Sound* sound = audio->audioPtr;

        ErrorCheck(sgpImplementation->mpSystem->playSound(sound, nullptr, true, &channel));
        int chIndex = -1;
        channel->getIndex(&chIndex);

        // Play the sound with pausing enabled initially

        if (channel)
        {
            // Set 3D attributes if the sound is in 3D mode
            FMOD_MODE mode;
            sound->getMode(&mode);
            if (mode & FMOD_3D) {
                FMOD_VECTOR fmodPosition = VectorToFmod(position);
                FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
                ErrorCheck(channel->set3DAttributes(&fmodPosition, &velocity));
            }

            // Set additional properties
            ErrorCheck(channel->setVolume(dbToVolume(volumeDB)));
            ErrorCheck(channel->setPaused(false));
            ErrorCheck(channel->setMode(looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));

            // Assign channel to a group
            auto itGroup = sgpImplementation->mChannelGroups.find(groupName);
            if (itGroup != sgpImplementation->mChannelGroups.end())
            {
                ErrorCheck(channel->setChannelGroup(itGroup->second));
            }

            // Store the channel
            sgpImplementation->mChannels[channelId] = channel;
        }

        return channelId; // Return the channel ID for tracking
    }

    void AudioEngine::PlayOneShot(Ref<Audio> audio, const glm::vec3& position, float volumeDB, const std::string& groupName)
    {
        if (!audio || !audio->audioPtr)
        {
            std::cerr << "Invalid audio reference" << std::endl;
            return;
        }

        FMOD::Channel* channel = nullptr;
        FMOD::Sound* sound = audio->audioPtr;

        int channelId = sgpImplementation->mnNextChannelId++;

        // Play the sound with no need for tracking
        ErrorCheck(sgpImplementation->mpSystem->playSound(sound, nullptr, true, &channel));

        int chIndex = -1;
        channel->getIndex(&chIndex);

        if (channel)
        {
            // Set 3D attributes if the sound is in 3D mode
            FMOD_MODE mode;
            sound->getMode(&mode);
            if (mode & FMOD_3D)
            {
                FMOD_VECTOR fmodPosition = VectorToFmod(position);
                FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
                ErrorCheck(channel->set3DAttributes(&fmodPosition, &velocity));
            }

            // Set additional properties
            ErrorCheck(channel->setVolume(dbToVolume(volumeDB)));
            ErrorCheck(channel->setPaused(false));
            ErrorCheck(channel->setMode(FMOD_LOOP_OFF)); // PlayOneShot is always non-looping

            // Assign to group if applicable
            auto itGroup = sgpImplementation->mChannelGroups.find(groupName);
            if (itGroup != sgpImplementation->mChannelGroups.end())
            {
                ErrorCheck(channel->setChannelGroup(itGroup->second));
            }
        }
    }

    void AudioEngine::ApplyFadeIn(int channelId, float fadeInTime, float targetVolumeDB)
    {
        auto it = sgpImplementation->mChannels.find(channelId);
        if (it == sgpImplementation->mChannels.end())
        {
            std::cerr << "Error: Invalid channel ID " << channelId << std::endl;
            return;
        }

        FMOD::Channel* channel = it->second;

        // ✅ Convert dB to linear volume
        float startVolume = 0.0f;  // Typically starts from silence
        float targetVolume = dbToVolume(targetVolumeDB);  // Convert dB to linear

        // Ensure FMOD starts with proper volume
        ErrorCheck(channel->setVolume(startVolume));

        // ✅ Use linear values in fade effect
        sgpImplementation->mFades.emplace_back(channelId, startVolume, targetVolume, fadeInTime, true);
    }

    void AudioEngine::ApplyFadeOut(int channelId, float fadeOutTime)
    {
        auto it = sgpImplementation->mChannels.find(channelId);
        if (it == sgpImplementation->mChannels.end())
        {
            std::cerr << "Error: Invalid channel ID " << channelId << std::endl;
            return;
        }

        FMOD::Channel* channel = it->second;
        bool isPlaying = false;
        channel->isPlaying(&isPlaying);

        if (isPlaying)
        {
            float currentVolume = 1.0f;
            channel->getVolume(&currentVolume);
            // Use a lower target volume (0.0001f = -80 dB)
            sgpImplementation->mFades.emplace_back(channelId, currentVolume, 0.0001f, fadeOutTime, false);
        }
    }



}