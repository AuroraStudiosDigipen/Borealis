/******************************************************************************
/*!
\file       RuntimeLayer.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares the Runtime Layer class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef RUNTIME_LAYER_HPP
#define RUNTIME_LAYER_HPP
#include <Borealis.hpp>
#include <Assets/AssetManager.hpp>

namespace BorealisRuntime
{
	class RuntimeLayer : public Borealis::Layer
	{
	public:
		void Init() override;
		void UpdateFn(float dt) override;
		void Free() override;
	private:
		Borealis::Ref<Borealis::FrameBuffer> mRuntimeFrameBuffer;
		Borealis::EditorAssetManager mEditorAssetManager;
	};
}

#endif
