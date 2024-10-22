/******************************************************************************/
/*!
\file		AudioEngine.hpp
\author 	Valerie Koh
\par    	email: v.koh@digipen.edu
\date   	September 11, 2024
\brief		Declares the AudioEngine class for handling audio functionality

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include <BorealisPCH.hpp>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <Scene/Components.hpp>

namespace Borealis
{
    struct Vector3
    {
        float x;
        float y;
        float z;
    };

    /*!***********************************************************************
    \class AudioEngine
    \brief Handles audio-related functionality, including initialization, playing sounds, and managing channels.
    *************************************************************************/
    class AudioEngine {
    public:

        /*!***********************************************************************
        \brief
            Initializes the audio engine system.
        *************************************************************************/
        static void Init();

        /*!***********************************************************************
        \brief
            Updates the audio engine, managing the state of currently playing sounds.
        *************************************************************************/
        static void Update();

        /*!***********************************************************************
        \brief
            Shuts down and cleans up the audio engine system.
        *************************************************************************/
        static void Shutdown();

        /*!***********************************************************************
        \brief
            Loads an audio file into the engine.
        \param strAudioName
            The name of the audio file.
        \param b3d
            Indicates whether the audio is 3D.
        \param bLooping
            Indicates whether the audio should loop.
        \param bStream
            Indicates whether the audio should be streamed or loaded in memory.
        *************************************************************************/
        static Audio LoadAudio(const std::string& strAudioName, bool b3d = true, bool bLooping = false, bool bStream = false);

        /*!***********************************************************************
        \brief
            Unloads a specific audio file from the engine.
        \param strAudioName
            The name of the audio file.
        *************************************************************************/
        static void UnLoadAudio(const std::string& strAudioName);

        /*!***********************************************************************
        \brief
            Sets the 3D listener's position and orientation.
        \param vPos
            The position of the listener.
        \param fVolumedB
            The volume in decibels for the listener.
        *************************************************************************/
        static void Set3DListenerAndOrientation(const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f);

        /*!***********************************************************************
        \brief
            Plays an audio file at the specified position.
        \param strSoundName
            The name of the sound file.
        \param vPosition
            The 3D position where the sound will be played.
        \param fVolumedB
            The volume in decibels.
        \param bMute
            Whether the sound should be muted.
        \param bLoop
            Whether the sound should loop.
        \return
            The ID of the channel where the sound is played.
        *************************************************************************/
        static int PlayAudio(const AudioSourceComponent& audio, const Vector3& vPosition = Vector3{ 0, 0, 0 }, float fVolumedB = 5.0f, bool bMute = false, bool bLoop = false);
        /*!***********************************************************************
        \brief
            Checks if a sound is playing on the specified channel.
        \param nChannelId
            The ID of the channel to check.
        \return
            True if the sound is playing, false otherwise.
        *************************************************************************/
        static bool isSoundPlaying(int nChannelId);

        /*!***********************************************************************
        \brief
            Stops the sound playing on the specified channel.
        \param nChannelId
            The ID of the channel to stop.
        *************************************************************************/
        static void StopChannel(int nChannelId);

        /*!***********************************************************************
        \brief
            Stops all currently playing audio channels.
        *************************************************************************/
        static void StopAllChannels();

        /*!***********************************************************************
        \brief
            Sets the 3D position of the sound on the specified channel.
        \param nChannelId
            The ID of the channel.
        \param vPosition
            The 3D position to set for the sound.
        *************************************************************************/
        static void SetChannel3DPosition(int nChannelId, const Vector3& vPosition);

        /*!***********************************************************************
        \brief
            Sets the volume for the specified channel.
        \param nChannelId
            The ID of the channel.
        \param fVolumedB
            The volume in decibels.
        *************************************************************************/
        static void SetChannelVolume(int nChannelId, float fVolumedB);

        /*!***********************************************************************
        \brief
            Checks if a sound is currently playing on a specified channel.
        \param nChannelId
            The ID of the channel to check.
        \return
            True if the sound is playing, false otherwise.
        *************************************************************************/
        bool IsPlaying(int nChannelId) const;

        /*!***********************************************************************
        \brief
            Converts a decibel value to a linear volume scale.
        \param db
            The decibel value to convert.
        \return
            The volume in linear scale.
        *************************************************************************/
        static float dbToVolume(float db);

        /*!***********************************************************************
        \brief
            Converts a linear volume scale to decibels.
        \param volume
            The volume in linear scale to convert.
        \return
            The decibel value.
        *************************************************************************/
        static float VolumeTodb(float volume);
    };
} // End of namespace Borealis
#endif
