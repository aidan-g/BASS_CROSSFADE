﻿using NUnit.Framework;
using System;
using System.IO;
using System.Linq;
using System.Threading;

namespace ManagedBass.Crossfade.Tests
{
    [TestFixture]
    public class Tests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(Tests).Assembly.Location);


        [TestCase(100, 100, BassCrossfadeType.OutQuad, BassCrossfadeType.OutQuad, false)]
        [TestCase(100, 100, BassCrossfadeType.OutExpo, BassCrossfadeType.OutExpo, false)]
        [TestCase(1000, 1000, BassCrossfadeType.Linear, BassCrossfadeType.Linear, true)]
        [TestCase(2000, 2000, BassCrossfadeType.Linear, BassCrossfadeType.Linear, true)]
        public void Test001(int inPeriod, int outPeriod, BassCrossfadeType inType, BassCrossfadeType outType, bool mix)
        {
            if (!Bass.Init(Bass.DefaultDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassCrossfade.Init())
            {
                Assert.Fail("Failed to initialize CROSSFADE.");
            }

            BassCrossfade.Mode = BassCrossfadeMode.Always;
            BassCrossfade.InPeriod = inPeriod;
            BassCrossfade.OutPeriod = outPeriod;
            BassCrossfade.InType = inType;
            BassCrossfade.OutType = outType;
            BassCrossfade.Mix = mix;

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
                Assert.Fail("Registered channel should not have been removed.");
            }

            Bass.StreamFree(sourceChannel1);
            Bass.StreamFree(sourceChannel2);
            Bass.StreamFree(playbackChannel);
            BassCrossfade.Free();
            Bass.Free();
        }

        [TestCase(100, 100, BassCrossfadeType.OutQuad, BassCrossfadeType.OutQuad, false)]
        [TestCase(100, 100, BassCrossfadeType.OutExpo, BassCrossfadeType.OutExpo, false)]
        [TestCase(1000, 1000, BassCrossfadeType.Linear, BassCrossfadeType.Linear, true)]
        [TestCase(2000, 2000, BassCrossfadeType.Linear, BassCrossfadeType.Linear, true)]
        public void Test002(int inPeriod, int outPeriod, BassCrossfadeType inType, BassCrossfadeType outType, bool mix)
        {
            if (!Bass.Init(Bass.DefaultDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassCrossfade.Init())
            {
                Assert.Fail("Failed to initialize CROSSFADE.");
            }

            BassCrossfade.Mode = BassCrossfadeMode.Manual;
            BassCrossfade.InPeriod = inPeriod;
            BassCrossfade.OutPeriod = outPeriod;
            BassCrossfade.InType = inType;
            BassCrossfade.OutType = outType;
            BassCrossfade.Mix = mix;

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

            Thread.Sleep(8000);

            if (!BassCrossfade.ChannelRemove(sourceChannel1))
            {
                Assert.Fail("Failed to remove registered channel.");
            }

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

            if (BassCrossfade.ChannelRemove(sourceChannel1))
            {
                Assert.Fail("Registered channel should not have been removed.");
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

        [Test]
        public void Test003()
        {
            for (var position = 0; position < 10; position++)
            {
                if (!Bass.Init(Bass.DefaultDevice))
                {
                    Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                if (!BassCrossfade.Init())
                {
                    Assert.Fail("Failed to initialize CROSSFADE.");
                }

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

                var playbackChannel = BassCrossfade.StreamCreate(channelInfo.Frequency, channelInfo.Channels, BassFlags.Default | BassFlags.Float);
                if (playbackChannel == 0)
                {
                    Assert.Fail(string.Format("Failed to create playback stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                if (!Bass.ChannelPlay(playbackChannel))
                {
                    Assert.Fail(string.Format("Failed to play stream:  {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                Thread.Sleep(5000);

                if (!BassCrossfade.ChannelRemove(sourceChannel1))
                {
                    Assert.Fail("Registered channel should not have been removed.");
                }

                Thread.Sleep(5000);

                if (!BassCrossfade.ChannelRemove(sourceChannel2))
                {
                    Assert.Fail("Registered channel should not have been removed.");
                }

                Bass.StreamFree(sourceChannel1);
                Bass.StreamFree(sourceChannel2);
                Bass.StreamFree(playbackChannel);
                BassCrossfade.Free();
                Bass.Free();
            }
        }

        [Test]
        public void Test004()
        {
            if (!Bass.Init(Bass.DefaultDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassCrossfade.Init())
            {
                Assert.Fail("Failed to initialize CROSSFADE.");
            }

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

            var playbackChannel = BassCrossfade.StreamCreate(channelInfo.Frequency, channelInfo.Channels, BassFlags.Default | BassFlags.Float);
            if (playbackChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create playback stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!Bass.ChannelPlay(playbackChannel))
            {
                Assert.Fail(string.Format("Failed to play stream:  {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            Thread.Sleep(8000);

            for (var a = 0; a < 10; a++)
            {
                BassCrossfade.StreamFadeOut();

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

                Thread.Sleep(2000);

                BassCrossfade.StreamFadeIn();

                sourceChannelCount = default(int);
                sourceChannels = BassCrossfade.GetChannels(out sourceChannelCount);

                if (sourceChannelCount != 2)
                {
                    Assert.Fail("Crossfade reports unexpected queued channel count.");
                }

                if (sourceChannels[0] != sourceChannel1 || sourceChannels[1] != sourceChannel2)
                {
                    Assert.Fail("Crossfade reports unexpected queued channel handles.");
                }
            }

            Thread.Sleep(8000);

            if (!BassCrossfade.ChannelRemove(sourceChannel1))
            {
                Assert.Fail("Registered channel should not have been removed.");
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

        [Test]
        public void Test005()
        {
            if (!Bass.Init(Bass.DefaultDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassCrossfade.Init())
            {
                Assert.Fail("Failed to initialize CROSSFADE.");
            }

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

            var playbackChannel = BassCrossfade.StreamCreate(channelInfo.Frequency, channelInfo.Channels, BassFlags.Default | BassFlags.Float);
            if (playbackChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create playback stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!Bass.ChannelPlay(playbackChannel))
            {
                Assert.Fail(string.Format("Failed to play stream:  {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            Thread.Sleep(8000);

            Bass.ChannelPause(playbackChannel);

            BassCrossfade.StreamFadeOut();

            BassCrossfade.StreamFadeIn();

            Bass.ChannelPlay(playbackChannel);

            if (BassCrossfade.ChannelRemove(sourceChannel1))
            {
                Assert.Fail("Registered channel should not have been removed.");
            }

            if (!BassCrossfade.ChannelRemove(sourceChannel2))
            {
                Assert.Fail("Registered channel should have been removed.");
            }

            Bass.StreamFree(playbackChannel);
            BassCrossfade.Free();
            Bass.Free();
        }

        [Test]
        public void Test006()
        {
            for (var position = 0; position < 10; position++)
            {
                if (!BassCrossfade.Init())
                {
                    Assert.Fail("Failed to initialize CROSSFADE.");
                }

                var count = default(int);
                var channels = default(int[]);

                for (var a = 1; a <= BassCrossfade.MAX_CHANNELS; a++)
                {
                    Assert.IsTrue(BassCrossfade.ChannelEnqueue(a));
                    Assert.IsFalse(BassCrossfade.ChannelEnqueue(a));
                    count = default(int);
                    channels = BassCrossfade.GetChannels(out count);
                    Assert.AreEqual(a, count);
                    for (var b = 1; b <= a; b++)
                    {
                        Assert.AreEqual(b, channels[b - 1]);
                    }
                }

                Assert.IsFalse(BassCrossfade.ChannelEnqueue(100));

                count = default(int);
                channels = BassCrossfade.GetChannels(out count);

                Assert.AreEqual(BassCrossfade.MAX_CHANNELS, count);

                foreach (var channel in channels.OrderBy(_ => Guid.NewGuid()).ToArray())
                {
                    Assert.IsTrue(BassCrossfade.ChannelRemove(channel));
                    Assert.IsFalse(BassCrossfade.ChannelRemove(channel));
                    count = default(int);
                    channels = BassCrossfade.GetChannels(out count);
                    Assert.IsFalse(channels.Contains(channel));
                }

                count = default(int);
                channels = BassCrossfade.GetChannels(out count);

                Assert.AreEqual(0, count);

                BassCrossfade.Free();
            }
        }
    }
}
