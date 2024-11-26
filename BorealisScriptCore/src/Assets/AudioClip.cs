using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Borealis
{
    [AssetField(AssetType.Audio)]
    public class AudioClip : Object
    {
        public AudioClip(ulong id)
        {
            InstanceID = id;
        }

        public AudioClip()
        {
            InstanceID = 0;
        }
    }
}
