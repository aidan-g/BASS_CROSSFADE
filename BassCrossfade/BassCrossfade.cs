using System;
using System.Runtime.InteropServices;

namespace ManagedBass.Crossfade
{
    public class BassCrossfade
    {
        const BassCrossfadeMode DEFAULT_MODE = BassCrossfadeMode.Always;

        const int DEFAULT_PERIOD = 100;

        const BassCrossfadeType DEFAULT_TYPE = BassCrossfadeType.EaseIn;

        const string DllName = "bass_crossfade";

        public static BassCrossfadeMode Mode
        {
            get
            {
                var mode = default(int);
                if (!GetConfig(BassCrossfadeAttribute.Mode, out mode))
                {
                    return DEFAULT_MODE;
                }
                return (BassCrossfadeMode)mode;
            }
            set
            {
                SetConfig(BassCrossfadeAttribute.Mode, Convert.ToInt32(value));
            }
        }

        public static int InPeriod
        {
            get
            {
                var period = default(int);
                if (!GetConfig(BassCrossfadeAttribute.InPeriod, out period))
                {
                    return DEFAULT_PERIOD;
                }
                return period;
            }
            set
            {
                SetConfig(BassCrossfadeAttribute.InPeriod, value);
            }
        }

        public static int OutPeriod
        {
            get
            {
                var period = default(int);
                if (!GetConfig(BassCrossfadeAttribute.OutPeriod, out period))
                {
                    return DEFAULT_PERIOD;
                }
                return period;
            }
            set
            {
                SetConfig(BassCrossfadeAttribute.OutPeriod, value);
            }
        }

        public static int OverlapPeriod
        {
            get
            {
                var period = default(int);
                if (!GetConfig(BassCrossfadeAttribute.OverlapPeriod, out period))
                {
                    return DEFAULT_PERIOD;
                }
                return period;
            }
            set
            {
                SetConfig(BassCrossfadeAttribute.OverlapPeriod, value);
            }
        }

        public static BassCrossfadeType InType
        {
            get
            {
                var type = default(int);
                if (!GetConfig(BassCrossfadeAttribute.InType, out type))
                {
                    return DEFAULT_TYPE;
                }
                return (BassCrossfadeType)type;
            }
            set
            {
                SetConfig(BassCrossfadeAttribute.InType, Convert.ToInt32(value));
            }
        }

        public static BassCrossfadeType OutType
        {
            get
            {
                var type = default(int);
                if (!GetConfig(BassCrossfadeAttribute.OutType, out type))
                {
                    return DEFAULT_TYPE;
                }
                return (BassCrossfadeType)type;
            }
            set
            {
                SetConfig(BassCrossfadeAttribute.OutType, Convert.ToInt32(value));
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

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_GetConfig(BassCrossfadeAttribute Attrib, out int Value);

        public static bool GetConfig(BassCrossfadeAttribute Attrib, out int Value)
        {
            return BASS_CROSSFADE_GetConfig(Attrib, out Value);
        }

        [DllImport(DllName)]
        static extern int BASS_CROSSFADE_StreamCreate(int Frequency, int Channels, BassFlags Flags, IntPtr User = default(IntPtr));

        public static int StreamCreate(int Frequency, int Channels, BassFlags Flags, IntPtr User = default(IntPtr))
        {
            return BASS_CROSSFADE_StreamCreate(Frequency, Channels, Flags, User);
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
        static extern bool BASS_CROSSFADE_ChannelEnqueue(int Handle);

        public static bool ChannelEnqueue(int Handle)
        {
            return BASS_CROSSFADE_ChannelEnqueue(Handle);
        }

        [DllImport(DllName)]
        static extern bool BASS_CROSSFADE_ChannelRemove(int Handle);

        public static bool ChannelRemove(int Handle)
        {
            return BASS_CROSSFADE_ChannelRemove(Handle);
        }
    }

    public enum BassCrossfadeAttribute
    {
        None = 0,
        Mixer = 1,
        Mode = 2,
        InPeriod = 3,
        OutPeriod = 4,
        OverlapPeriod = 5,
        InType = 6,
        OutType = 7
    }

    public enum BassCrossfadeMode
    {
        None = 0,
        Always = 1,
        Manual = 2
    }

    public enum BassCrossfadeType
    {
        None = 0,
        Linear = 1,
        Logarithmic = 2,
        Exponential = 3,
        EaseIn = 4,
        EaseOut = 5
    }
}
