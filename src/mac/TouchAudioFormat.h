/*
 *  TouchAudioFormat.h
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 11/26/10.
 *  Copyright 2010 Compartmental. All rights reserved.
 *
 */

#ifndef TOUCHAUDIOFORMAT_H
#define TOUCHAUDIOFORMAT_H

#include "AudioFormat.h"
#include "CAStreamBasicDescription.h"

class TouchAudioFormat : public Minim::AudioFormat
{
public:
	TouchAudioFormat( const int numberOfChannels )
	: AudioFormat()
	{
		mStreamDesc.mSampleRate = 44100.f;
		mStreamDesc.SetAUCanonical( numberOfChannels, true );
		
		SetFormatFromMyDesc();
	}
	
	inline void SetWithDesc( const CAStreamBasicDescription & desc )
	{
		mStreamDesc = desc;
		SetFormatFromMyDesc();
	}
	
	inline const CAStreamBasicDescription & getStreamDescription() const { return mStreamDesc; }
	
private:
	
	inline void SetFormatFromMyDesc()
	{
		mChannels = mStreamDesc.mChannelsPerFrame;
		mSampleRate = mStreamDesc.mSampleRate;
		mFrameRate = mSampleRate;
		mFrameSize = mStreamDesc.mBytesPerFrame;
		mSampleSizeInBits = mStreamDesc.mBitsPerChannel;
		mBigEndian = (mStreamDesc.mFormatFlags & kLinearPCMFormatFlagIsBigEndian);	
	}
	
	CAStreamBasicDescription mStreamDesc;
};

#endif // TOUCHAUDIOFORMAT_H