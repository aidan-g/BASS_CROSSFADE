using ManagedBass.Mix;
using NUnit.Framework;
using System;
using System.IO;
using System.Threading;

namespace ManagedBass.Crossfade.Tests
{
    [TestFixture]
    public class Tests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(Tests).Assembly.Location);

        [TestCase(1000, BassCrossfadeType.Linear)]
        [TestCase(2000, BassCrossfadeType.Linear)]
        [TestCase(1000, BassCrossfadeType.Logarithmic)]
        [TestCase(2000, BassCrossfadeType.Logarithmic)]
        [TestCase(1000, BassCrossfadeType.Exponential)]
        [TestCase(2000, BassCrossfadeType.Exponential)]
        [TestCase(1000, BassCrossfadeType.EaseIn)]
        [TestCase(2000, BassCrossfadeType.EaseIn)]
        [TestCase(1000, BassCrossfadeType.EaseOut)]
        [TestCase(2000, BassCrossfadeType.EaseOut)]
        public void Test001(int period, BassCrossfadeType type)
        {
            if (!Bass.Init(Bass.DefaultDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassCrossfade.Init())
            {
                Assert.Fail("Failed to initialize CROSSFADE.");
            }

            BassCrossfade.Period = period;
            BassCrossfade.Type = type;

            var sourceChannel1 = Bass.CreateStream(Path.Combine(Location, "Media", "01 Botanical Dimensions.m4a"), 0, 0, BassFlags.Decode | BassFlags.Float);
            if (sourceChannel1 == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var sourceChannel2 = Bass.CreateStream(Path.Combine(Location, "Media", "02 Outer Shpongolia.m4a"), 0, 0, BassFlags.Decode | BassFlags.Float);
            if (sourceChannel2 == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var channelInfo = default(ChannelInfo);
            if (!Bass.ChannelGetInfo(sourceChannel1, out channelInfo))
            {
                Assert.Fail(string.Format("Failed to get channel info: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassCrossfade.ChannelEnqueue(sourceChannel1))
            {
                Assert.Fail("Failed to add stream to crossfade queue.");
            }

            if (!BassCrossfade.ChannelEnqueue(sourceChannel2))
            {
                Assert.Fail("Failed to add stream to crossfade queue.");
            }

            var sourceChannelCount = default(int);
            var sourceChannels = BassCrossfade.GetChannels(out sourceChannelCount);

            if (sourceChannelCount != 2)
            {
                Assert.Fail("Crossfade reports unexpected queued channel count.");
            }

            if (sourceChannels[0] != sourceChannel1 || sourceChannels[1] != sourceChannel2)
            {
                Assert.Fail("Crossfade reports unexpected queued channel handles.");
            }

            var playbackChannel = BassCrossfade.StreamCreate(channelInfo.Frequency, channelInfo.Channels, BassFlags.Default | BassFlags.Float);
            if (playbackChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create playback stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!Bass.ChannelPlay(playbackChannel))
            {
                Assert.Fail(string.Format("Failed to play stream:  {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var channelLength = Bass.ChannelGetLength(sourceChannel1);
            var channelLengthSeconds = Bass.ChannelBytes2Seconds(sourceChannel1, channelLength);

            Bass.ChannelSetPosition(sourceChannel1, Bass.ChannelSeconds2Bytes(sourceChannel1, channelLengthSeconds - 10));

            do
            {
                var channelActive = Bass.ChannelIsActive(playbackChannel);
                if (channelActive == PlaybackState.Stopped)
                {
                    break;
                }

                var channelPosition = Bass.ChannelGetPosition(playbackChannel);
                var channelPositionSeconds = Bass.ChannelBytes2Seconds(playbackChannel, channelPosition);

                if (channelPositionSeconds >= 20)
                {
                    break;
                }

                Thread.Sleep(1000);
            } while (true);

            if (!BassCrossfade.ChannelRemove(sourceChannel1))
            {
                Assert.Fail("Registered channel should have been removed.");
            }

            if (!BassCrossfade.ChannelRemove(sourceChannel2))
            {
                Assert.Fail("Registered channel should have been removed.");
            }

            Bass.StreamFree(sourceChannel1);
            Bass.StreamFree(sourceChannel2);
            Bass.StreamFree(playbackChannel);
            BassCrossfade.Free();
            Bass.Free();
        }
    }
}
