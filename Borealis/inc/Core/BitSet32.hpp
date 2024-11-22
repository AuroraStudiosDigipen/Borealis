/******************************************************************************/
/*!
\file		BitSet32.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	November 17, 2024
\brief		Declares the class for Bitset

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef BITSET32_HPP
#define BITSET32_HPP
#include <stdint.h>
namespace Borealis
{
	class Bitset32
	{
	public:
		Bitset32() = default;
		Bitset32(uint32_t value);

		void set(uint32_t index, bool value = true);
		void reset();
		void reset(uint32_t index);
		void flip(uint32_t index);
		void flip();
		bool test(uint32_t index) const;
		bool all() const;
		bool any() const;
		bool none() const;
		bool operator[](uint32_t index) const;
		uint32_t to_ulong() const;

		std::vector<int> ToBitsList() const;
		uint16_t toUint16();


	private:
		uint32_t data = 1;
	};
}
#endif