using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Borealis
{
    public class AudioClip : Object
    {
        public string audioName;
        public AudioClip(string name)
        {
            audioName = name;
        }

        public string GetAudioName()
        {
            return audioName;
        }

        public void SetAudioName(string name)
        {
            audioName = name;
        }
        public AudioClip()
        {
            audioName = "";
        }
    }
}
