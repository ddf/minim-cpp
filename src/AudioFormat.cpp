#include "AudioFormat.h"

#ifdef WINDOWS

Minim::AudioFormat::AudioFormat( float sampleRate, int numberOfChannels )
: mChannels(numberOfChannels)
, mFrameRate( sampleRate )
, mFrameSize( sizeof(float)*numberOfChannels )
, mSampleRate( sampleRate )
, mSampleSizeInBits( sizeof(float) )
, mBigEndian(true)
{
}

#else
#include "CAStreamBasicDescription.h"

Minim::AudioFormat::AudioFormat( float sampleRate, int numberOfChannels )
{
	CAStreamBasicDescription streamDesc;
	streamDesc.mSampleRate = 44100.0f;
	streamDesc.SetAUCanonical( numberOfChannels, true );
	mChannels = mStreamDesc.mChannelsPerFrame;
	mSampleRate = streamDesc.mSampleRate;
	mFrameRate = streamDesc.mSampleRate;
	mFrameSize = mStreamDesc.mBytesPerFrame;
	mSampleSizeInBits = mStreamDesc.mBitsPerChannel;
	mBigEndian = (mStreamDesc.mFormatFlags & kLinearPCMFormatFlagIsBigEndian);	
}

#endif