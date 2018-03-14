/*
 *  Delay.cpp
 *  Unity-iPhone
 *
 *  Created by Damien Di Fede on 8/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Delay.h"
#include <algorithm>

Minim::Delay::Delay( const float maxDT, const float ampFactor, const bool feedback )
: UGen()
, audio( *this, AUDIO )
, delTime( *this, CONTROL )
, delAmp( *this, CONTROL )
, feedback(*this, CONTROL )
, dryMix( *this, CONTROL )
, wetMix( *this, CONTROL )
, maxDelayTime( maxDT )
, delayBuffer(nullptr)
, delayBufferFrames(0)
, delayBufferWriteFrame(0)
, feedBackOn( feedback )
{
	delTime.setLastValue(maxDT);
	delAmp.setLastValue(ampFactor);
	dryMix.setLastValue(1);
	wetMix.setLastValue(1);
}

Minim::Delay::~Delay()
{
	if ( delayBuffer )
	{
		delete[] delayBuffer;
	}
}

void Minim::Delay::reset()
{
	if (delayBuffer != nullptr)
	{
		memset(delayBuffer, 0, sizeof(float)*delayBufferFrames*getAudioChannelCount());
	}
	delayBufferWriteFrame = 0;
}

void Minim::Delay::sampleRateChanged()
{
	allocateDelayBuffer();
}

void Minim::Delay::uGenerate( float * out, const int numChannels )
{
	if (delayBuffer == nullptr)
	{
		UGen::fill(out, 0, numChannels);
	}
	else
	{
		// how many samples to delay the input - clamping to the maximum number of frames we can handle
		const float delayFrames = std::min(delTime.getLastValue()*sampleRate(), (float)delayBufferFrames-1);

		// we need to use the fractional part to interpolate between two previous sample frames
		// otherwise we will get artifacts.
		const int firstFrame = (int)delayFrames;
		const float frameLerp = delayFrames - firstFrame;
		// since we are seeking backwards in the ring buffer, we first offset from the end of the buffer,
		// this way an index that would be negative winds up as a positive number at the end.
		// by doing % delayBufferFrames, we wrap positive indices into the buffer.
		const int readFrame1 = (delayBufferFrames + delayBufferWriteFrame - firstFrame) % delayBufferFrames;
		const int readFrame2 = (delayBufferFrames + delayBufferWriteFrame - firstFrame - 1) % delayBufferFrames;

		const float amp = delAmp.getLastValue();
		const float feed = feedback.getLastValue();
		const float wet = wetMix.getLastValue();
		const float dry = dryMix.getLastValue();

		// apply to each channel
		for (int c = 0; c < numChannels; ++c)
		{
			const float inSample = audio.getLastValues()[c];

			// where to record incoming audio and mix with feedback.
			// we write first in case our first read index is the same as the write index,
			// which can happen the delay time is very small.
			const int writeIdx = delayBufferWriteFrame*numChannels + c;
			delayBuffer[writeIdx] = inSample;

			// seek backwards by our delay time			
			const int readIdx1 = readFrame1*numChannels + c;
			const int readIdx2 = readFrame2*numChannels + c;

			// grab the sample there
			const float delaySample = amp * (delayBuffer[readIdx1] + frameLerp*(delayBuffer[readIdx2] - delayBuffer[readIdx1]));

			// add feedback signal if necessary
			if (feedBackOn)
			{
				delayBuffer[writeIdx] += delaySample*feed;
			}

			// audible output is the delayed signal scaled by wet mix.
			// plus the incoming signal scaled by dry mix.
			out[c] = inSample*dry + delaySample*wet;
		}

		// advance the output frame
		delayBufferWriteFrame = (delayBufferWriteFrame + 1) % delayBufferFrames;
	}
}

void Minim::Delay::allocateDelayBuffer()
{
	if (delayBuffer != nullptr)
	{
		delete[] delayBuffer;
	}

	delayBufferFrames = (int)(maxDelayTime*sampleRate());
	const size_t len = delayBufferFrames*audio.getChannelCount();
	delayBuffer = new float[len];
	memset(delayBuffer, 0, len * sizeof(float));
	delayBufferWriteFrame = 0;
}
