/******************************************************************************/
/*!
\file		Console.cpp
\author 	Liu Chengrong
\par    	email: chengrong.liu\@digipen.edu
\date   	October 30, 2024
\brief		Provides the implementation of the Console class methods, handling 
            message logging, message retrieval, and singleton instance creation.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/


#include <BorealisPCH.hpp>
#include <Debugging/Console.hpp>
#include <fstream>

namespace Borealis
{
    // Singleton instance method
    Console& Console::GetInstance()
    {
        static Console instance;
        return instance;
    }

    // Method to log messages
    void Console::Log(const std::string& message, MessageType type)
    {
        if (message == "") { return; }
        switch (type)
        {
        case INFO_MESSAGE:
            infoMessages.push_back(message);
            break;
        case WARNING_MESSAGE:
            warningMessages.push_back(message);
            break;
        case ERROR_MESSAGE:
            errorMessages.push_back(message);
            break;
        }
    }

    // Method to clear all messages
    void Console::Clear()
    {
        infoMessages.clear();
        warningMessages.clear();
        errorMessages.clear();
    }

    // Method to clear info messages
    void Console::ClearInfoMessages()
    {
        infoMessages.clear();
    }

    // Method to clear warning messages
    void Console::ClearWarningMessages()
    {
        warningMessages.clear();
    }

    // Method to clear error messages
    void Console::ClearErrorMessages()
    {
        errorMessages.clear();
    }

    //Method to delete selected info message
    void Console::DeleteInfoMessage(int index)
    {
        if (index >= 0 && index < infoMessages.size())
        {
            infoMessages.erase(infoMessages.begin() + index);
        }
    }

    //Method to delete selected warning message
    void Console::DeleteWarningMessage(int index)
    {
        if (index >= 0 && index < warningMessages.size())
        {
            warningMessages.erase(warningMessages.begin() + index);
        }
    }

    //Method to delete selected error message
    void Console::DeleteErrorMessage(int index)
    {
        if (index >= 0 && index < errorMessages.size())
        {
            errorMessages.erase(errorMessages.begin() + index);
        }
    }

    //Method to save logged messages into file
    void Console::SaveMessagesToFile(const std::string& filename)
    {
        std::ofstream outFile(filename);

        if (!outFile.is_open())
        {
            // Handle file open error
            std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
            return;
        }

        Console& console = Console::GetInstance();

        // Save Info Messages
        outFile << "Info Messages:\n";
        for (const auto& msg : console.GetInfoMessages())
        {
            outFile << msg;
        }
        outFile << "\n";

        // Save Warning Messages
        outFile << "Warning Messages:\n";
        for (const auto& msg : console.GetWarningMessages())
        {
            outFile << msg;
        }
        outFile << "\n";

        // Save Error Messages
        outFile << "Error Messages:\n";
        for (const auto& msg : console.GetErrorMessages())
        {
            outFile << msg;
        }
        outFile << "\n";

        outFile.close();
        std::cout << "Messages saved to " << filename << std::endl;
    }

    // Getters for message vectors
    const std::vector<std::string>& Console::GetInfoMessages() const
    {
        return infoMessages;
    }

    const std::vector<std::string>& Console::GetWarningMessages() const
    {
        return warningMessages;
    }

    const std::vector<std::string>& Console::GetErrorMessages() const
    {
        return errorMessages;
    }
    bool Console::HasError() const
    {
        return !errorMessages.empty();
    }
}