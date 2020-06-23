#include "../bass/bass.h"
#include "../bass/bassmix.h"

int main()
{
	HSTREAM input_channel1;
	HSTREAM input_channel2;
	HSTREAM mixer_channel;
	BASS_MIXER_NODE nodes[6];

	BASS_Init(-1, 44100, 0, 0, NULL);

	input_channel1 = BASS_StreamCreateFile(FALSE, "C:\\personal\\BASS_CROSSFADE\\distribution\\net40\\Media\\01 Botanical Dimensions.m4a", 0, 0, BASS_STREAM_DECODE);
	input_channel2 = BASS_StreamCreateFile(FALSE, "C:\\personal\\BASS_CROSSFADE\\distribution\\net40\\Media\\02 Outer Shpongolia.m4a", 0, 0, BASS_STREAM_DECODE);

	mixer_channel = BASS_Mixer_StreamCreate(44100, 2, 0);

	//Fade in 1.
	nodes[0].pos = 0;
	nodes[0].value = 0;
	nodes[1].pos = BASS_ChannelSeconds2Bytes(mixer_channel, 1);
	nodes[1].value = 0.5;
	nodes[2].pos = BASS_ChannelSeconds2Bytes(mixer_channel, 2);
	nodes[2].value = 1;

	//Fade out 1.
	nodes[3].pos = BASS_ChannelSeconds2Bytes(mixer_channel, 10);
	nodes[3].value = 1;
	nodes[4].pos = BASS_ChannelSeconds2Bytes(mixer_channel, 11);
	nodes[4].value = 0.5;
	nodes[5].pos = BASS_ChannelSeconds2Bytes(mixer_channel, 12);
	nodes[5].value = 0;

	BASS_Mixer_StreamAddChannel(mixer_channel, input_channel1, BASS_MIXER_CHAN_PAUSE | BASS_MIXER_CHAN_NORAMPIN);
	BASS_Mixer_StreamAddChannel(mixer_channel, input_channel2, BASS_MIXER_CHAN_PAUSE | BASS_MIXER_CHAN_NORAMPIN);

	BASS_Mixer_ChannelSetEnvelope(input_channel1, BASS_MIXER_ENV_VOL, nodes, 6);

	BASS_Mixer_ChannelFlags(input_channel1, 0, BASS_MIXER_CHAN_PAUSE);

	BASS_ChannelPlay(mixer_channel, FALSE);

	Sleep(20000);

	BASS_StreamFree(input_channel1);
	BASS_StreamFree(input_channel2);
	BASS_Free();
}