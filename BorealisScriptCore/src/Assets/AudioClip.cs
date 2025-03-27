using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Borealis
{
    public class AudioClip : Object
    {
        public byte[] guid;
        public AudioClip(byte[] id)
        {
            guid = new byte[16];
            for (int i = 0; i < 16; i++)
            {
                guid[i] = id[i];
            }
        }

        public string GetAudioName()
        {
            return InternalCalls.GetAudioClipName(out guid);
        }
        public void SetAudioID(byte[] bytes)
        {
            for (int i = 0; i < 16; i++)
            {
                guid[i] = bytes[i];
            }
        }

        public byte[] GetAudioID()
        {
            return guid;
        }
        public void SetAudioName(string name)
        {
            InternalCalls.SetAudioClipName(name, ref guid);
        }
        public AudioClip()
        {
            guid = new byte[16];
        }
    }
}
