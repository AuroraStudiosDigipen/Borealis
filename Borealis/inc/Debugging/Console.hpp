/******************************************************************************/
/*!
\file		Console.hpp
\author 	Liu Chengrong
\par    	email: chengrong.liu\@digipen.edu
\date   	October 30, 2024
\brief      Implements a singleton pattern for logging and managing messages 
            in editor 

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <vector>
#include <string>

namespace Borealis
{
    class Console
    {
    public:
        /***********************************************************
        * @enum MessageType
        * @brief Defines the types of messages that can be logged.
        ***********************************************************/
        enum MessageType { INFO_MESSAGE, WARNING_MESSAGE, ERROR_MESSAGE};

        /***********************************************************
        * @brief Retrieves the singleton instance of the Console.
        * @return Reference to the singleton instance of Console.
        ***********************************************************/
        static Console& GetInstance();

        /***********************************************************
         * @brief Logs a message of a specified type.
         * @param message The message to be logged.
         * @param type The type of the message (INFO, WARNING, or ERROR).
        ***********************************************************/
        void Log(const std::string& message, MessageType type);

        /***********************************************************
        * @brief Clears all logged messages.
        ***********************************************************/
        void Clear();
       
        /***********************************************************
        * @brief Clears all info logged messages.
        ***********************************************************/
        void ClearInfoMessages();

        /***********************************************************
        * @brief Clears all warning logged messages.
        ***********************************************************/
        void ClearWarningMessages();

        /***********************************************************
        * @brief Clears all error logged messages.
        ***********************************************************/
        void ClearErrorMessages();

        /***********************************************************
        * @brief Delete selected info message.
        ***********************************************************/
        void DeleteInfoMessage(int index);

        /***********************************************************
        * @brief Delete selected warning message.
        ***********************************************************/
        void DeleteWarningMessage(int index);

        /***********************************************************
        * @brief Delete selected error messages.
        ***********************************************************/
        void DeleteErrorMessage(int index);

        /***********************************************************
        * @brief Saving logged messages into a file.
        ***********************************************************/
        void SaveMessagesToFile(const std::string& filename);

        /***********************************************************
        * @brief Retrieves all logged info messages.
        * @return A const reference to the vector containing info messages.
        ***********************************************************/
        const std::vector<std::string>& GetInfoMessages() const;

        /***********************************************************
        * @brief Retrieves all logged warning messages.
        * @return A const reference to the vector containing warning messages.
        ***********************************************************/
        const std::vector<std::string>& GetWarningMessages() const;

        /***********************************************************
        * @brief Retrieves all logged error messages.
        * @return A const reference to the vector containing error messages.
        ***********************************************************/
        const std::vector<std::string>& GetErrorMessages() const;

        /***********************************************************
        * @brief Checks if there are any logged error messages.
        * @return True if there are error messages, false otherwise.
        ***********************************************************/
        bool HasError() const;

    private:
        /***********************************************************
        * @brief Private constructor to prevent instantiation.
        ***********************************************************/
        Console() = default;

        /***********************************************************
        * @brief Private destructor.
        ***********************************************************/
        ~Console() = default;

        // Delete copy constructor and assignment operator to ensure singleton behavior.
        Console(const Console&) = delete;
        Console& operator=(const Console&) = delete;

        // Vector to store info messages.
        std::vector<std::string> infoMessages;

        // Vector to store warning messages.
        std::vector<std::string> warningMessages;
        
        // Vector to store  messages.
        std::vector<std::string> errorMessages;
    };
}
#endif CONSOLE_HPP