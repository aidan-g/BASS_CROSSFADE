using System;
using System.Runtime.InteropServices;

namespace ManagedBass.Crossfade
{
    public class BassCrossfade
    {
        const string DllName = "bass_crossfade";

        public static readonly object SyncRoot = new object();

        public BassCrossfade(int channelHandle)
        {
            this.ChannelHandle = channelHandle;
        }

        public int ChannelHandle { get; private set; }

        public bool Contains(int channelHandle)
        {
            lock (SyncRoot)
            {
                throw new NotImplementedException();
            }
        }

        public int Position(int channelHandle)
        {
            lock (SyncRoot)
            {
                throw new NotImplementedException();
            }
        }

        public bool Add(int channelHandle)
        {
            lock (SyncRoot)
            {
                throw new NotImplementedException();
            }
        }

        public bool Remove(int channelHandle)
        {
            lock (SyncRoot)
            {
                throw new NotImplementedException();
            }
        }

        public void Reset()
        {
            lock (SyncRoot)
            {
                throw new NotImplementedException();
            }
        }


        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_Init();

        public static bool Init()
        {
            return BASS_CROSSFADE_Init();
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_Free();

        public static bool Free()
        {
            return BASS_CROSSFADE_Free();
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_SetConfig(BassCrossfadeAttribute Attrib, int Value);

        public static bool SetConfig(BassCrossfadeAttribute Attrib, int Value)
        {
            return BASS_CROSSFADE_SetConfig(Attrib, Value);
        }

        public static bool SetConfig(BassCrossfadeAttribute Attrib, bool Value)
        {
            return BASS_CROSSFADE_SetConfig(Attrib, Value ? 1 : 0);
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_GetConfig(BassCrossfadeAttribute Attrib, out int Value);

        public static bool GetConfig(BassCrossfadeAttribute Attrib, out int Value)
        {
            return BASS_CROSSFADE_GetConfig(Attrib, out Value);
        }

        [DllImport(DllName)]
        static extern IntPtr BASS_CROSSFADE_GetChannels(out int Count);

        public static int[] GetChannels(out int Count)
        {
            var channels = BASS_CROSSFADE_GetChannels(out Count);
            var result = new int[Count];
            Marshal.Copy(channels, result, 0, Count);
            return result;
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_StreamEnqueue(int Handle);

        public static bool StreamEnqueue(int Handle)
        {
            return BASS_CROSSFADE_StreamEnqueue(Handle);
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_StreamRemove(int Handle);

        public static bool StreamRemove(int Handle)
        {
            return BASS_CROSSFADE_StreamRemove(Handle);
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_FadeIn(int Handle);

        public static bool FadeIn(int Handle)
        {
            return BASS_CROSSFADE_FadeIn(Handle);
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_FadeOut(int Handle);

        public static bool FadeOut(int Handle)
        {
            return BASS_CROSSFADE_FadeOut(Handle);
        }
    }

    public enum BassCrossfadeAttribute
    {
        None = 0,
        Mixer = 1,
        Period = 2,
        Type = 3
    }

    public enum BassCrossfadeType
    {
        None = 0,
        Linear = 1,
        Logarithmic = 2,
        Exponential = 3
    }
}
