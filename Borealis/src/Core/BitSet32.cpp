#include "BorealisPCH.hpp"
#include "Core/BitSet32.hpp"


namespace Borealis
{
	Bitset32::Bitset32(uint32_t value)
	{
		data = value;
	}
	void Bitset32::set(uint32_t index, bool value) {
		if (value) {
			data |= 1 << index;
		}
		else {
			data &= ~(1 << index);
		}
	}

	void Bitset32::reset()
	{
		data = 0;
	}

	void Bitset32::reset(uint32_t index) {
		data &= ~(1 << index);
	}

	void Bitset32::flip(uint32_t index) {
		data ^= 1 << index;
	}

	void Bitset32::flip()
	{
		data = ~data;
	}

	bool Bitset32::test(uint32_t index) const {
		return data & (1 << index);
	}

	bool Bitset32::all() const {
		return data == 0xFFFFFFFF;
	}

	bool Bitset32::any() const {
		return data != 0;
	}

	bool Bitset32::none() const {
		return data == 0;
	}

	bool Bitset32::operator[](uint32_t index) const {
		return test(index);
	}

	uint32_t Bitset32::to_ulong() const {
		return data;
	}
}