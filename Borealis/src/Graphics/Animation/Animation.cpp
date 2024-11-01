#include <BorealisPCH.hpp>
#include "Graphics/Animation/Animation.hpp"

namespace Borealis
{
	Bone* Animation::FindBone(std::string const& name)
	{
		auto i = std::find_if(mBones.begin(), mBones.end(), [&](Bone const& bone)
			{
				return bone.GetBoneName() == name;
			});

		if (i == mBones.end())
		{
			return nullptr;
		}

		return &(*i);
	}
}