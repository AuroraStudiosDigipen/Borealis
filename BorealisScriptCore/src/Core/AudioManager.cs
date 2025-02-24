using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Borealis
{
    public class AudioManager
    {
        public static void SetMusicVolume(float volume)
        {
            InternalCalls.SetMusicVolume(volume);
        }

        public static void SetSFXVolume(float volume)
        {
            InternalCalls.SetSFXVolume(volume);
        }

        public static void SetMasterVolume(float volume)
        {
            InternalCalls.SetMasterVolume(volume);
        }
    }
}
