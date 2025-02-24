/******************************************************************************
/*!
\file       SceneRendererPanel.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       February 24, 2025
\brief      Declares

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SceneRendererPanel_HPP
#define SceneRendererPanel_HPP

namespace Borealis
{
    class SceneRenderPanel
    {
    public:
        /*!***********************************************************************
        \brief Constructs the Audio Mixer Panel
        *************************************************************************/
        SceneRenderPanel();

        /*!***********************************************************************
        \brief Renders the Audio Mixer Panel in ImGui
        *************************************************************************/
        void ImGuiRender();
    };
}

#endif
