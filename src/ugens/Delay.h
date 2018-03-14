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
		explicit Delay( const float maxDelayTime = 0.25f, const float amplitudeFactor = 0.5f, const bool feedbackOn = false);
		virtual ~Delay();
		
		UGenInput audio;
		UGenInput delTime;
		// amplitude applied to delayed signal before outputting it
		UGenInput delAmp;
		// amplitude applied to delayed signal before feeding it back.
		// this is stacked on top of delAmp and is only used if feedback is enabled
		UGenInput feedback;
		UGenInput dryMix;
		UGenInput wetMix;

		inline void setFeedbackOn(const bool on) { feedBackOn = on;  }

		// zeroes the delay buffer and resets the write position
		void reset();
		
	protected:
		virtual void sampleRateChanged() override;
		virtual void channelCountChanged() override { allocateDelayBuffer(); }
		virtual void uGenerate( float * out, const int numChannels ) override;
		
	private:
		// called if sample rate or channel count change
		void allocateDelayBuffer();

		// the maximum time in seconds that our delay buffere represents
		const float maxDelayTime;
		
		// an buffer of samples frames that represents maxDelayTime (channels are interleaved)
		float* delayBuffer;
		// the number of samples frames in delayBuffer (note: not the array size!)
		int    delayBufferFrames;

		// the next output frame in delayBuffer
		int delayBufferWriteFrame;
		
		// flag to include feedback
		bool   feedBackOn;
	};
};

#endif // MINIM_DELAY_H