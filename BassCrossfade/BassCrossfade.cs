using ManagedBass.Mix;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;

namespace ManagedBass.Crossfade
{
    public class BassCrossfade
    {
        const string DllName = "bass_crossfade";

        public static readonly object SyncRoot = new object();

        public BassCrossfade()
        {
            this.Queue = new List<int>();
        }

        public IList<int> Queue { get; private set; }

        public int ChannelHandle { get; set; }

        public bool Contains(int channelHandle)
        {
            lock (SyncRoot)
            {
                var channelHandles = BassMix.MixerGetChannels(this.ChannelHandle);
                return channelHandles.Contains(channelHandle) || this.Queue.Contains(channelHandle);
            }
        }

        public int Position(int channelHandle)
        {
            lock (SyncRoot)
            {
                var channelHandles = BassMix.MixerGetChannels(this.ChannelHandle);
                if (channelHandles.Contains(channelHandle))
                {
                    return 0;
                }
                var index = this.Queue.IndexOf(channelHandle);
                if (index == -1)
                {
                    return -1;
                }
                return index + 1;
            }
        }

        public bool Add(int channelHandle)
        {
            lock (SyncRoot)
            {
                Bass.ChannelSetAttribute(channelHandle, ChannelAttribute.Volume, 0);
                if (!this.AddToMixer(channelHandle))
                {
                    if (this.Queue.Contains(channelHandle))
                    {
                        return false;
                    }
                    this.Queue.Add(channelHandle);
                }
                StreamRegister(channelHandle);
                return true;
            }
        }

        public bool Remove(int channelHandle)
        {
            lock (SyncRoot)
            {
                StreamUnregister(channelHandle);
                if (this.RemoveFromMixer(channelHandle))
                {
                    return true;
                }
                return this.Queue.Remove(channelHandle);
            }
        }

        public void Reset()
        {
            lock (SyncRoot)
            {
                var channelHandles = BassMix.MixerGetChannels(this.ChannelHandle);
                foreach (var channelHandle in channelHandles)
                {
                    BassMix.MixerRemoveChannel(channelHandle);
                }
                this.Queue.Clear();
            }
        }

        protected virtual bool AddToMixer()
        {
            do
            {
                if (this.Queue.Count == 0)
                {
                    return false;
                }
                var channelHandle = this.Queue[0];
                this.Queue.RemoveAt(0);
                if (BassMix.MixerAddChannel(this.ChannelHandle, channelHandle, BassFlags.Default))
                {
                    return true;
                }
            } while (true);
        }

        protected virtual bool AddToMixer(int channelHandle)
        {
            var channelHandles = BassMix.MixerGetChannels(this.ChannelHandle);
            if (channelHandles.Length == 0)
            {
                if (BassMix.MixerAddChannel(this.ChannelHandle, channelHandle, BassFlags.Default))
                {
                    FadeIn(channelHandle);
                    while (Bass.ChannelGetAttribute(channelHandle, ChannelAttribute.Volume) < 1)
                    {
                        Thread.Sleep(10);
                    }
                    return true;
                }
            }
            return false;
        }

        protected virtual bool RemoveFromMixer(int channelHandle)
        {
            if (BassMix.ChannelGetMixer(channelHandle) == this.ChannelHandle)
            {
                this.AddToMixer();
                FadeOut(channelHandle);
                while (Bass.ChannelGetAttribute(channelHandle, ChannelAttribute.Volume) > 0)
                {
                    Thread.Sleep(10);
                }
                return BassMix.MixerRemoveChannel(channelHandle);
            }
            return false;
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_HANDLER_Init();

        public static bool Init()
        {
            return BASS_CROSSFADE_HANDLER_Init();
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_HANDLER_Free();

        public static bool Free()
        {
            return BASS_CROSSFADE_HANDLER_Free();
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
        static extern bool BASS_CROSSFADE_StreamRegister(int Handle);

        public static bool StreamRegister(int Handle)
        {
            return BASS_CROSSFADE_StreamRegister(Handle);
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_StreamUnregister(int Handle);

        public static bool StreamUnregister(int Handle)
        {
            return BASS_CROSSFADE_StreamUnregister(Handle);
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
        Period = 1,
        Type = 2
    }

    public enum BassCrossfadeType
    {
        None = 0,
        Linear = 1,
        Logarithmic = 2,
        Exponential = 3
    }
}
