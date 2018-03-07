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
		const int delayFrames = std::min((int)(delTime.getLastValue()*sampleRate()), delayBufferFrames-1);
		const float amp = delAmp.getLastValue();
		const float feed = feedback.getLastValue();
		const float wet = wetMix.getLastValue();
		const float dry = dryMix.getLastValue();

		// apply to each channel
		for (int c = 0; c < numChannels; ++c)
		{
			const float inSample = audio.getLastValues()[c];

			// seek backwards by our delay time
			const int readFrame = (delayBufferFrames + delayBufferWriteFrame - delayFrames)%delayBufferFrames;
			const int readIdx = (readFrame*numChannels + c);

			// grab the sample there
			const float delaySample = delayBuffer[readIdx]*amp;

			// where to record incoming audio and mix with feedback
			const int writeIdx = delayBufferWriteFrame*numChannels + c;
			delayBuffer[writeIdx] = feedBackOn ? inSample + delaySample*feed : inSample;

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
