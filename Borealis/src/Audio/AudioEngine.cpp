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
#include <FMOD/fmod_studio.hpp>
#include "Audio/AudioEngine.hpp"
#include <Scene/Components.hpp>
#include <Core/Project.hpp>
#include <vector>
#include <sstream>
#include <cctype>
#include <windows.h>

namespace Borealis
{
    static bool HasFileChanged(const std::wstring& filePath, FILETIME& lastWriteTime) {
        HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            std::wcerr << L"Failed to open file: " << filePath << std::endl;
            return false;
        }

        FILETIME ftWrite;
        if (GetFileTime(hFile, NULL, NULL, &ftWrite)) {
            if (CompareFileTime(&ftWrite, &lastWriteTime) != 0) {
                lastWriteTime = ftWrite;
                CloseHandle(hFile);
                return true; // File has been modified
            }
        }

        CloseHandle(hFile);
        return false; // No change detected
    }


    static bool HasInit = false;
    struct Implementation {
        Implementation(std::string path);
        ~Implementation();

        void Update();

        FMOD::System* mpSystem;
        FMOD::Studio::System* mpStudioSystem;
        FMOD::Studio::Bank* mpMasterBank = nullptr;
        FMOD::Studio::Bank* mpStringsBank = nullptr;
        std::set<std::string> mAudioList;
        DirectoryTree treeData;


        int mnNextChannelId;
        typedef std::map<int, FMOD::Studio::EventInstance*> ChannelMap;
        ChannelMap mChannels;
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
            BOREALIS_CORE_ERROR("FMOD: {}", result);
            return 1;
        }
        return 0;
    }

    static FILETIME lastWriteTime1 = {};
    static FILETIME lastWriteTime2 = {};
    static bool stringFileChanged = false;
    static bool masterFileChanged = false;

    Implementation::Implementation(std::string path)
    {
        mpSystem = nullptr;
        mnNextChannelId = 1;
        ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
        ErrorCheck(mpStudioSystem->initialize(128, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_3D_RIGHTHANDED, nullptr));
        // Create FMOD Core system
        ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
        // Initialize FMOD Core system
        ErrorCheck(mpSystem->set3DSettings(1.0, 1.f, 1.0f));

        ErrorCheck(mpStudioSystem->loadBankFile((path + "\\" + "Master.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &mpMasterBank));
        ErrorCheck(mpStudioSystem->loadBankFile((path + "\\" + "Master.strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &mpStringsBank));
        DirectoryTree tree;
        FMOD::Studio::EventDescription** eventArray;
        int eventCount = 0;
        mpMasterBank->getEventCount(&eventCount);
        eventArray = new FMOD::Studio::EventDescription * [eventCount];
        mpMasterBank->getEventList(eventArray, eventCount, &eventCount);
        for (int i = 0; i < eventCount; i++) {
            char path[256];
            eventArray[i]->getPath(path, sizeof(path), nullptr);
            mAudioList.insert(path);
            std::string eventPath(path);
            treeData.insertPath(eventPath.substr(7));
        }
        delete[] eventArray;

        HasFileChanged(std::filesystem::path(path + "\\" + "Master.bank"), lastWriteTime1);
        HasFileChanged(std::filesystem::path(path + "\\" + "Master.strings.bank"), lastWriteTime2);
    }

    Implementation::~Implementation()
    {
        // Release FMOD Core system
        ErrorCheck(mpStudioSystem->unloadAll());
        ErrorCheck(mpStudioSystem->release());
    }

    void Implementation::Update()
    {
        ErrorCheck(mpStudioSystem->update());
    }

    Implementation* sgpImplementation = nullptr;

    void AudioEngine::Init(std::string path)
    {
        sgpImplementation = new Implementation(path);
        HasInit = true;
    }

    void AudioEngine::Reload(std::string path)
    {
        sgpImplementation->mAudioList.clear();
        sgpImplementation->treeData.clear();
        sgpImplementation->mChannels.clear();
        sgpImplementation->mnNextChannelId = 1;
        ErrorCheck(sgpImplementation->mpStudioSystem->unloadAll());
        ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile((path + "\\" + "Master.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &sgpImplementation->mpMasterBank));
        ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile((path + "\\" + "Master.strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &sgpImplementation->mpStringsBank));
        DirectoryTree tree;
        FMOD::Studio::EventDescription** eventArray;
        int eventCount = 0;
        ErrorCheck(sgpImplementation->mpMasterBank->getEventCount(&eventCount));
        eventArray = new FMOD::Studio::EventDescription * [eventCount];
        ErrorCheck(sgpImplementation->mpMasterBank->getEventList(eventArray, eventCount, &eventCount));
        for (int i = 0; i < eventCount; i++) {
            char path[256];
            eventArray[i]->getPath(path, sizeof(path), nullptr);
            sgpImplementation->mAudioList.insert(path);
            std::string eventPath(path);
            sgpImplementation->treeData.insertPath(eventPath.substr(7));
        }
        delete[] eventArray;
    }



    void AudioEngine::Update()
    {
        if (HasInit)
            sgpImplementation->Update();
    }

    void AudioEngine::UnLoadAudio(const std::string& strSoundName)
    {
        // Here you can add your logic for unloading sounds
    }

    glm::vec3 CalculateVelocity(const glm::vec3& previousPosition, const glm::vec3& currentPosition, float deltaTime) {
        return (currentPosition - previousPosition) / deltaTime;
    }

    void AudioEngine::Set3DListenerAndOrientation(const glm::mat4& transform, float fVolumedB)
    {

        glm::vec3 vPos = transform[3];
        FMOD_VECTOR fmodPosition = VectorToFmod(vPos);

        glm::vec3 forward = glm::normalize(glm::vec3(transform[2]));
        glm::vec3 up = glm::normalize(glm::vec3(transform[1]));     

        FMOD_VECTOR fmodForward = VectorToFmod(forward);
        FMOD_VECTOR fmodUp = VectorToFmod(up);
        ErrorCheck(sgpImplementation->mpSystem->set3DNumListeners(1));
        ErrorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(
            0,
            &fmodPosition,
            nullptr,
            &fmodForward,
            &fmodUp
        ));
    }

    int AudioEngine::PlayAudio(std::string audioPath, const glm::mat4& transform)
    {
        auto vPosition = glm::vec3(transform[3]);
        glm::vec3 forward = glm::normalize(glm::vec3(transform[2]));
        glm::vec3 up = glm::normalize(glm::vec3(transform[1]));
        FMOD_VECTOR fmodForward = VectorToFmod(forward);
        FMOD_VECTOR fmodUp = VectorToFmod(up);
        FMOD::Studio::EventInstance* eventInstance;
        FMOD::Studio::EventDescription* eventDesc;
        sgpImplementation->mpStudioSystem->getEvent(audioPath.c_str(), &eventDesc);
        eventDesc->createInstance(&eventInstance);
        FMOD_3D_ATTRIBUTES attr;
        attr.position = VectorToFmod(vPosition);
        attr.forward = fmodForward;
        attr.up = fmodUp;
        attr.velocity = { 0.0f, 0.0f, 0.0f };

        ErrorCheck(eventInstance->set3DAttributes(&attr));
        ErrorCheck(eventInstance->setPaused(false));
        ErrorCheck(eventInstance->start());
        
        sgpImplementation->mChannels[sgpImplementation->mnNextChannelId++] = eventInstance;


        return sgpImplementation->mnNextChannelId - 1;
    }

    bool AudioEngine::isSoundPlaying(int channelID)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(channelID);

        if (tFoundIt != sgpImplementation->mChannels.end())
        {
            FMOD_STUDIO_PLAYBACK_STATE state;
            ErrorCheck(tFoundIt->second->getPlaybackState(&state));
            return state == FMOD_STUDIO_PLAYBACK_PLAYING;
        }
        return false;
    }

    void AudioEngine::StopChannel(int nChannelId)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);

        if (tFoundIt != sgpImplementation->mChannels.end())
        {
            FMOD::Studio::EventInstance* pChannel = tFoundIt->second;
            bool bIsPlaying = false;

            if (isSoundPlaying(nChannelId))
            {
                ErrorCheck(pChannel->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT));
                sgpImplementation->mChannels.erase(nChannelId);
            }
        }
    }

    void AudioEngine::StopAllChannels()
    {
        // Stop all tracked channels
        for (auto it = sgpImplementation->mChannels.begin(); it != sgpImplementation->mChannels.end(); ++it)
        {
            FMOD::Studio::EventInstance* pChannel = it->second;
            ErrorCheck(pChannel->stop(FMOD_STUDIO_STOP_IMMEDIATE));
        }

        sgpImplementation->mChannels.clear(); // Clear the map as all channels are stopped
    }

    void AudioEngine::SetChannel3DPosition(int nChannelId, const glm::vec3& vPosition)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
        if (tFoundIt == sgpImplementation->mChannels.end())
            return;

        FMOD_VECTOR position = VectorToFmod(vPosition);
        FMOD_3D_ATTRIBUTES attr;
        attr.position = VectorToFmod(vPosition);
        ErrorCheck(tFoundIt->second->set3DAttributes(&attr));
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

#pragma optimize("", off) 
    void AudioEngine::UpdateChannelPosition(int channelID, const glm::mat4& transform)
    {
        auto it = sgpImplementation->mChannels.find(channelID);
        if (it == sgpImplementation->mChannels.end())
        {
            return;
        }

        FMOD::Studio::EventInstance* channel = it->second;
        if (!channel) {
            return;
        }

        glm::vec3 position = glm::vec3(transform[3]);
        glm::vec3 forward = glm::normalize(glm::vec3(transform[2]));
        glm::vec3 up = glm::normalize(glm::vec3(transform[1]));

        FMOD_VECTOR fmodForward = VectorToFmod(forward);
        FMOD_VECTOR fmodUp = VectorToFmod(up);
        FMOD_VECTOR fmodPosition = VectorToFmod(position);
        FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
        FMOD_3D_ATTRIBUTES attr;
        attr.forward = fmodForward;
        attr.up = fmodUp;
        attr.velocity = velocity;
        attr.position = VectorToFmod(position);
        ErrorCheck(channel->set3DAttributes(&attr));
    }
    std::set<std::string> AudioEngine::GetAudioList()
    {
        return sgpImplementation->mAudioList;

    }

    void AudioEngine::EditorUpdate()
    {

        if (sgpImplementation == nullptr) return;
        
        if (HasFileChanged(std::filesystem::path(Project::GetProjectPath() + "\\master.bank"), lastWriteTime1)) masterFileChanged = true;
        if (HasFileChanged(std::filesystem::path(Project::GetProjectPath() + "\\master.strings.bank"), lastWriteTime2)) stringFileChanged = true;

        if (masterFileChanged && stringFileChanged)
        {
			Reload(Project::GetProjectPath());
			masterFileChanged = false;
			stringFileChanged = false;
		}
    }
    std::set<std::string> AudioEngine::GetAudioListInDirectory(const std::string& directory)
    {
        return sgpImplementation->treeData.getFilesInDirectory(directory);
    }
    std::set<std::string> AudioEngine::GetFoldersInDirectory(const std::string& directory)
    {
        return sgpImplementation->treeData.getFoldersInDirectory(directory);
    }
    std::set<std::string> AudioEngine::GetAudioListSearch(std::string keyword)
    {
        std::set<std::string> list;

        for (auto audioPath : sgpImplementation->mAudioList)
        {
            // find last '/'
            auto pos = audioPath.find_last_of('/');
            std::string audioFile = audioPath.substr(pos + 1); // file name
            // check if audio file contains keyword
            // put  audio file to all lower case
            std::transform(audioFile.begin(), audioFile.end(), audioFile.begin(), ::tolower);

            std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
            if (audioFile.find(keyword) != std::string::npos)
            {
				list.insert(audioPath);
			}
        }
        return list;
    }
#pragma optimize("", on)

    float AudioEngine::VolumeTodb(float volume)
    {
        return 20.0f * log10f(volume);
    }

    void AudioEngine::Shutdown()
    {
        if (HasInit)
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
        return 0; // Failure
    }

    void AudioEngine::SetGroupVolume(const std::string& groupName, float fVolumedB)
    {
        float volume = dbToVolume2(fVolumedB);

        FMOD::Studio::Bus* bus;
        std::string busName = "bus:/" + groupName;
        auto result = sgpImplementation->mpStudioSystem->getBus(busName.c_str(), &bus);

        if (result == FMOD_OK)
        {
			ErrorCheck(bus->setVolume(volume));
		}
        else
        {
			std::cerr << "Error: Group \"" << groupName << "\" not found!" << std::endl;
		}
    }

    int AudioEngine::Play(std::string audioPath, const glm::mat4& transform)
    {
        auto position = glm::vec3(transform[3]);
        glm::vec3 forward = glm::normalize(glm::vec3(transform[2]));
        glm::vec3 up = glm::normalize(glm::vec3(transform[1]));
        FMOD_VECTOR fmodForward = VectorToFmod(forward);
        FMOD_VECTOR fmodUp = VectorToFmod(up);


        FMOD::Studio::EventInstance* eventInstance;
        FMOD::Studio::EventDescription* eventDesc;
        ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(audioPath.c_str(), &eventDesc));
        eventDesc->createInstance(&eventInstance);
        FMOD_3D_ATTRIBUTES attr;
        attr.position = VectorToFmod(position);
        attr.forward = fmodForward;
        attr.up = fmodUp;
        attr.velocity = { 0.0f, 0.0f, 0.0f };
        ErrorCheck(eventInstance->set3DAttributes(&attr));
        ErrorCheck(eventInstance->setPaused(false));
        ErrorCheck(eventInstance->start());

        sgpImplementation->mChannels[sgpImplementation->mnNextChannelId++] = eventInstance;

        return sgpImplementation->mnNextChannelId-1; // Return the channel ID for tracking
    }

//#pragma optimize("", off)
    int AudioEngine::PlayOneShot(std::string audioPath, const glm::mat4& transform)
    {
       return Play(audioPath, transform);
    }


//#pragma optimize("", on)

    DirectoryTree::DirectoryTree()
    {
        root = new DirectoryNode("/");
    }

    void DirectoryTree::insertPath(const std::string& path)
    {
        std::stringstream ss(path);
        std::string token;
        DirectoryNode* currentNode = root;
        std::vector<std::string> pathParts;

        while (std::getline(ss, token, '/')) {
            if (token.empty()) continue;
            pathParts.push_back(token);
        }

        // Traverse or create directories in the path
        for (size_t i = 0; i < pathParts.size(); ++i) {
            const std::string& part = pathParts[i];
            if (i == pathParts.size() - 1) { // Last part (file)
                currentNode->addFile(part);
            }
            else { // Intermediate directory
                currentNode = currentNode->addSubdirectory(part);
            }
        }
    }

    void DirectoryTree::printStructure(DirectoryNode* node, int depth)
    {
        if (node == nullptr) return;

        // Indentation for the current level
        for (int i = 0; i < depth; i++) {
            std::cout << "  ";
        }

        // Print directories first
        if (node->isDirectory()) {
            std::cout << "[" << node->name << "]" << std::endl;
            for (auto& subdir : node->subdirectories) {
                printStructure(subdir.second, depth + 1);
            }
        }

        // Then print files
        for (const auto& file : node->files) {
            for (int j = 0; j < depth + 1; j++) {
                std::cout << "  ";
            }
            std::cout << file << std::endl;
        }
    }

    std::set<std::string> DirectoryTree::getFilesInDirectory(const std::string& directory)
    {
        std::stringstream ss(directory);
        std::string token;
        std::vector<std::string> pathParts;

        while (std::getline(ss, token, '/')) {
            if (token.empty()) continue;
            pathParts.push_back(token);
        }

        DirectoryNode* currentNode = root;
        for (size_t i = 0; i < pathParts.size(); ++i) {
			const std::string& part = pathParts[i];
			auto it = currentNode->subdirectories.find(part);
            if (it == currentNode->subdirectories.end()) {
				return std::set<std::string>();
			}
			currentNode = it->second;
		}

        return currentNode->files;
    }

    std::set<std::string> DirectoryTree::getFoldersInDirectory(const std::string& directory)
    {
        std::stringstream ss(directory);
        std::string token;
        std::vector<std::string> pathParts;

        while (std::getline(ss, token, '/')) {
            if (token.empty()) continue;
            pathParts.push_back(token);
        }

        DirectoryNode* currentNode = root;
        for (size_t i = 0; i < pathParts.size(); ++i) {
            const std::string& part = pathParts[i];
            auto it = currentNode->subdirectories.find(part);
            if (it == currentNode->subdirectories.end()) {
                return std::set<std::string>();
            }
            currentNode = it->second;
        }

        std::set<std::string> folders;
        for (const auto& subdir : currentNode->subdirectories) {
			folders.insert(subdir.first);
		}
        return folders;
    }

    void DirectoryTree::clear()
    {
        delete root;
		root = new DirectoryNode("/");
    }


    void DirectoryNode::addFile(const std::string& fileName)
    {
        files.insert(fileName);
    }

    DirectoryNode* DirectoryNode::addSubdirectory(const std::string& dirName)
    {
        if (subdirectories.find(dirName) == subdirectories.end()) {
            subdirectories[dirName] = new DirectoryNode(dirName);
        }
        return subdirectories[dirName];
    }

}