/*
 *  Delay.h
 *  Unity-iPhone
 *
 *  Created by Damien Di Fede on 8/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MINIM_DELAY_H
#define MINIM_DELAY_H

#include "UGen.h"

namespace Minim
{
	class Delay : public UGen
	{
	public:
		explicit Delay( const float maxDelayTime = 0.25f, const float amplitudeFactor = 0.5f, const bool feedbackOn = false, const bool passAudioOn = true );
		virtual ~Delay();
		
		UGenInput audio;
		UGenInput delTime;
		UGenInput delAmp;
		UGenInput dryMix;
		UGenInput wetMix;
		
	protected:
		virtual void sampleRateChanged();
		virtual void channelCountChanged();
		virtual void uGenerate( float * out, const int numChannels );
		
	private:
		void bufferSizeChanged();
		
		float  delayTime;
		float  maxDelayTime;
		float  amplitudeFactor;
		float* delayBuffer;
		int    bufferSize;
		int    maxBufferSize;
		int    iBufferIn, iBufferOut;
		bool   feedBackOn;
		bool   passAudioOn;
	};
};

#endif // MINIM_DELAY_H