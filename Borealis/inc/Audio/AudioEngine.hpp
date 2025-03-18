#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include <BorealisPCH.hpp>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <Scene/Components.hpp>
#include <Assets/AssetMetaData.hpp>
#include <glm/glm.hpp>


namespace Borealis
{

    class DirectoryNode {
    public:
        std::string name;  // Directory name or file name
        std::map<std::string, DirectoryNode*> subdirectories;  // Subdirectories
        std::set<std::string> files;  // Files in this directory

        bool isLeaf() const {
            return subdirectories.empty() && files.empty();
        }

        bool isDirectory() const
        {
            return !isLeaf();
        }

        DirectoryNode(const std::string& name)
            : name(name) {}

        // Function to add a file to the current directory
        void addFile(const std::string& fileName);

        // Function to add a subdirectory
        DirectoryNode* addSubdirectory(const std::string& dirName);
    };

    class DirectoryTree {
    public:
        DirectoryNode* root;

        DirectoryTree();

        // Insert file path into the tree
        void insertPath(const std::string& path);

        // Print the directory structure (for debugging)
        void printStructure(DirectoryNode* node, int depth = 0);

        std::set<std::string> getFilesInDirectory(const std::string& directory);

        std::set<std::string> getFoldersInDirectory(const std::string& directory);

        void clear();
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
        static void Init(std::string path);

        static void Reload(std::string path);

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
        static void Set3DListenerAndOrientation(const glm::mat4& transform = glm::mat4{ 1.f }, float fVolumedB = 0.0f);

        /*!***********************************************************************
        \brief
            Plays an audio file at the specified position, assigned to a group.
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
        \param groupId
            The ID of the audio group.
        \return
            The ID of the channel where the sound is played.
        *************************************************************************/
        static int PlayAudio(std::string audioPath, const glm::mat4& transform = glm::mat4(1.0f));

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
        static void SetChannel3DPosition(int nChannelId, const glm::vec3& vPosition);

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
            Sets the position of the listener in 3D space.
        \param position
            The 3D position of the listener.
        \param forward
            The forward orientation vector.
        \param up
            The upward orientation vector.
        *************************************************************************/
        void SetListenerPosition(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up);

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

        /*!***********************************************************************
        \brief
            Sets the master volume for the entire audio engine.
        \param volume
            The master volume level in linear scale (0.0 - 1.0).
        *************************************************************************/
        static void SetMasterVolume(float fVolumedB);

        /*!***********************************************************************
        \brief
            Creates a new audio group that can manage multiple sounds collectively.
        \param groupName
            The name of the audio group to create.
        \return
            The ID of the created audio group.
        *************************************************************************/
        static int CreateGroup(const std::string& groupName);

        /*!***********************************************************************
        \brief
            Sets the volume for a specific audio group.
        \param groupId
            The ID of the audio group.
        \param volume
            The volume level for the group in linear scale (0.0 - 1.0).
        *************************************************************************/
        static void SetGroupVolume(const std::string& groupName, float fVolumedB);

        /*!***********************************************************************
        \brief
            Retrieves the default audio group ID.
        \return
            The ID of the default audio group.
        //*************************************************************************/
        //static int GetDefaultGroupId();

        //static int GetGroupIdForAudio(FMOD::Sound* fmodSound);

        static int mDefaultGroupI; /*!< The default audio group ID */

        /*!***********************************************************************
        \brief
            Plays an audio asset.
        \param audio
            Reference to the audio asset.
        \param position
            The 3D position where the sound will be played.
        \param volumeDB
            The volume in decibels.
        \param looping
            Whether the sound should loop.
        \param groupName
            The name of the audio group.
        \return
            The ID of the playing channel.
        *************************************************************************/
        static int Play(std::string audioPath, const glm::mat4& transform = glm::mat4{ 1.f });

        /*!***********************************************************************
        \brief
            Plays an audio asset as a one-shot sound.
        \param audio
            Reference to the audio asset.
        \param position
            The 3D position where the sound will be played.
        \param volumeDB
            The volume in decibels.
        \param groupName
            The name of the audio group.
        *************************************************************************/
        static int PlayOneShot(std::string audioPath, const glm::mat4& transform = glm::mat4{ 1.f });
        
        static float dbToVolume2(float sliderValue);

        static void UpdateChannelPosition(int channelID, const glm::mat4& transform = glm::mat4{ 1.f });

        static std::set<std::string> GetAudioList();

        static void EditorUpdate();

        static std::set<std::string> GetAudioListInDirectory(const std::string& directory);
        static std::set<std::string> GetFoldersInDirectory(const std::string& directory);

        static std::set<std::string> GetAudioListSearch( std::string keyword);

    };
} // End of namespace Borealis

#endif
