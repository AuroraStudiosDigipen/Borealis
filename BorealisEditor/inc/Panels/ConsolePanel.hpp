/******************************************************************************/
/*!
\file		ConsolePanel.hpp
\author 	Liu Chengrong
\par    	email: chengrong.liu\@digipen.edu
\date   	October 30, 2024
\brief		Declares the class responsible for rendering and managing the console 
            panel using ImGui

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/


#ifndef CONSOLEPANEL_HPP
#define CONSOLEPANEL_HPP

#include <Borealis.hpp>

namespace Borealis
{
    class ConsolePanel
    {
    public:
        /***********************************************************
         * @brief Renders the console panel using ImGui.
        ***********************************************************/
        void ImGuiRender();
    };
}


#endif CONSOLEPANEL_HPP