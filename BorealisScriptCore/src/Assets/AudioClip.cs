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
            guid = id;
        }

        public string GetAudioName()
        {
            return InternalCalls.GetAudioClipName(out guid);
        }
        public void SetAudioID(byte[] bytes)
        {
            guid = bytes;
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
