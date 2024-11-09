/******************************************************************************
/*!
\file       NodeHeaderCodeFormat.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <AI/BehaviourTree/NodeHeaderCodeFormat.hpp>
#include <Core/LoggerSystem.hpp>

namespace Borealis
{
    std::string NodeHeaderCodeFormat::GenerateHeaderCode(const std::string& className, const std::string& baseClassName, int nodeTypeIndex)
    {
        std::string headerTemplate;

        if (nodeTypeIndex == 2|| nodeTypeIndex == 1) // Leaf and Decorator are same
        {
            headerTemplate = 
            R"(/******************************************************************************
/*!
\file       {ClassName}.hpp
\author     Your Name
\par        email: your.email@example.com
\date       {Date}
\brief      Declares the class for the leaf node {ClassName}

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
******************************************************************************/

#ifndef {ClassName}_HPP
#define {ClassName}_HPP

#include "AI/BehaviourTree/BehaviourNode.hpp"

namespace Borealis
{
    class {ClassName} : public {BaseClassName}
    {
    public:
        {ClassName}();

    protected:
        // Add member variables and methods specific to the leaf node

        virtual void OnEnter() override;
        virtual void OnUpdate(float dt, Entity& entity) override;
    };
}

#endif
            )";
        }       
        else if (nodeTypeIndex == 0) // Control Flow
        {
            headerTemplate =
                R"(/******************************************************************************
/*!
\file       {ClassName}.hpp
\author     Your Name
\par        email: your.email@example.com
\date       {Date}
\brief      Declares the class for the leaf node {ClassName}

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
             
******************************************************************************/

#ifndef {ClassName}_HPP
#define {ClassName}_HPP

#include "AI/BehaviourTree/BehaviourNode.hpp"

namespace Borealis
{
    class {ClassName} : public {BaseClassName}
    {
    public:
        {ClassName}();

    protected:
        size_t currentIndex;  // Tracks the current child node being executed

        virtual void OnEnter() override;
        virtual void OnUpdate(float dt, Entity& entity) override;
    };
}

#endif
            )";
        }

        // Replace placeholders
        std::string dateStr = GetCurrentDate();
        ReplaceAll(headerTemplate, "{ClassName}", className);
        ReplaceAll(headerTemplate, "{BaseClassName}", baseClassName);
        ReplaceAll(headerTemplate, "{Date}", dateStr);

        return headerTemplate;
    }

    std::string NodeHeaderCodeFormat::GenerateSourceCode(const std::string& className, int nodeTypeIndex)
    {
        std::string sourceTemplate;

        if (nodeTypeIndex == 2) // Leaf
        {
            sourceTemplate = 
            R"(/******************************************************************************
/*!
\file       {ClassName}.cpp
\author     Your Name
\par        email: your.email@example.com
\date       {Date}
\brief      Defines the class for the leaf node {ClassName}

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
******************************************************************************/

#include <BorealisPCH.hpp>
#include <AI/BehaviourTree/Leaf/{ClassName}.hpp>

namespace Borealis
{
    {ClassName}::{ClassName}()
    {
        // Constructor implementation
    }

    void {ClassName}::OnEnter()
    {
        // OnEnter implementation
        BehaviourNode::OnLeafEnter();
    }

    void {ClassName}::OnUpdate(float dt, Entity& entity)
    {
        // OnUpdate implementation 
        if(true)
        {
            OnSuccess();
        }
    }
}
            )";
        }
        else if (nodeTypeIndex == 1) // Decorator
        {
            sourceTemplate =
                R"(/******************************************************************************
/*!
\file       {ClassName}.cpp
\author     Your Name
\par        email: your.email@example.com
\date       {Date}
\brief      Defines the class for the decorator node {ClassName}

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
******************************************************************************/

#include <BorealisPCH.hpp>
#include <AI/BehaviourTree/Decorator/{ClassName}.hpp>

namespace Borealis
{
    {ClassName}::{ClassName}()
    {
        // Constructor implementation
    }

    void {ClassName}::OnEnter()
    {
        // OnEnter implementation
        BehaviourNode::OnEnter();
    }

    void {ClassName}::OnUpdate(float dt, Entity& entity)
    {
        // OnUpdate implementation 
        if(true)
        {
            OnEnter();
        }
    }
}
            )";
        }
        else if (nodeTypeIndex == 0) // Control Flow
        {
            sourceTemplate =
                R"(/******************************************************************************
/*!
\file       {ClassName}.cpp
\author     Your Name
\par        email: your.email@example.com
\date       {Date}
\brief      Defines the class for the control flow node {ClassName}

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
******************************************************************************/

#include <BorealisPCH.hpp>
#include  <AI/BehaviourTree/ControlFlow/{ClassName}.hpp>

namespace Borealis
{
    {ClassName}::{ClassName}()
    {
        // Constructor implementation
    }

    void {ClassName}::OnEnter()
    {
        currentIndex = 0;
        BehaviourNode::OnEnter();
    }

    void {ClassName}::OnUpdate(float dt, Entity& entity)
    {
        // if any child fails, the node fails
        // if all children succeed, the node succeeds
        Ref<BehaviourNode> currentNode = mChildren[currentIndex];
        currentNode->Tick(dt,entity);

        if (currentNode->HasFailed() == true)
        {
            OnFailure();
        }
        else if (currentNode->HasSucceeded() == true)
        {
            // move to the next node
            ++currentIndex;

            // if we hit the size, then all nodes suceeded
            if (currentIndex == mChildren.size())
            {
                OnSuccess();
            }
        }
    }
}
            )";
        }

        // Replace placeholders
        std::string dateStr = GetCurrentDate();
        ReplaceAll(sourceTemplate, "{ClassName}", className);
        ReplaceAll(sourceTemplate, "{Date}", dateStr);

        return sourceTemplate;
    }

    void NodeHeaderCodeFormat::ReplaceAll(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    std::string NodeHeaderCodeFormat::GetCurrentDate()
    {
        // Get current date as a string
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        char dateStr[20];
        std::strftime(dateStr, sizeof(dateStr), "%B %d, %Y", &tm);
        return std::string(dateStr);
    }

    void NodeHeaderCodeFormat::WriteToFile(const std::string& filePath, const std::string& content)
    {
        std::ofstream outFile(filePath);
        if (outFile.is_open())
        {
            outFile << content;
            outFile.close();
        }
        else
        {
            // Handle error
            BOREALIS_CORE_ERROR("Create New Node button failed to write to file: {}", filePath);
        }
    }

}

