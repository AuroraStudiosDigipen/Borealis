using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Borealis
{
    [AssetField(AssetType.Animation)]
    public class Animation : Object
    {
        public Animation(ulong id)
        {
            InstanceID = id;
        }

        public Animation()
        {
            InstanceID = 0;
        }
    }
}
