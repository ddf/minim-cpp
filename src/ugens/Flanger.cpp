//
//  Flanger.cpp
//  MinimTouch
//
//  Created by Damien Di Fede on 2/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Flanger.h"
#include "Waves.h"
#include <string> // for memset
#include <cassert>

static Minim::Wavetable* SINE = Minim::Waves::SINE();

Minim::Flanger::Flanger(float delayLength, float lfoRate, float delayDepth, float feedbackAmplitude, float dryAmplitude, float wetAmplitude)
: UGen()
, audio( *this, AUDIO )
, delay( *this, CONTROL, delayLength )
, rate( *this, CONTROL, lfoRate )
, depth( *this, CONTROL, delayDepth )
, feedback( *this, CONTROL, feedbackAmplitude )
, dry( *this, CONTROL, dryAmplitude )
, wet( *this, CONTROL, wetAmplitude )
, delayBuffer( NULL )
, writeFrame( 0 )
, bufferFrameLength( 0 )
, step( 0 )
, stepSize( 0 )
, prevFreq( 0 )
, oneOverSampleRate( 0 )
{
}

Minim::Flanger::~Flanger()
{
    if ( delayBuffer )
    {
        delete [] delayBuffer;
    }
}

void Minim::Flanger::reset()
{
	if (delayBuffer != nullptr)
	{
		memset(delayBuffer, 0, sizeof(float)*bufferFrameLength*getAudioChannelCount());
	}

	writeFrame = 0;
	step = 0;
}

void Minim::Flanger::channelCountChanged()
{
    resetBuffer();
}

void Minim::Flanger::sampleRateChanged()
{
    resetBuffer();
    
    oneOverSampleRate = 1.0f / sampleRate();
    // don't call updateStepSize because it checks for frequency change
    stepSize = getRate() * oneOverSampleRate;
    prevFreq = getRate();
    // start at the lowest value
    step = 0.25f;
}

void Minim::Flanger::uGenerate(float* out, const int numChannels)
{
    assert( numChannels == getAudioChannelCount() );
    
    // generate lfo value
    const float lfo = SINE->value( step );
    
    // modulate the delay amount using the lfo value.
    // we always modulate to a value larger than the input delay.
    const float dep = depth.getLastValue() * 0.5f;
    const float delMS = delay.getLastValue() + ( lfo * dep + dep );
	const float feed = feedback.getLastValue();
    
    // how many sample frames is that?
	const float delayFrames = (delMS * sampleRate() / 1000);
	// we need to use the fractional part to interpolate between two previous sample frames
	// otherwise we will get artifacts.
	const int firstFrame = (int)delayFrames;
	const int readFrame1 = (bufferFrameLength + writeFrame - firstFrame) % bufferFrameLength;
	const int readFrame2 = (bufferFrameLength + writeFrame - firstFrame - 1) % bufferFrameLength;
	const float frameLerp = delayFrames - firstFrame;
    
    for ( int c = 0; c < numChannels; ++c )
    {
		float inSample = audio.getLastValues()[c];

		// where to record incoming audio and mix with feedback
		const int writeIdx = writeFrame*numChannels + c;

		// first write in sample into the delay buffer
		// so that if readIdx1 is the same as writeIdx, we will use the correct value for interpolation.
		// if we *don't* do this, we will wind up using whatever we put in the buffer bufferFrameLength frames in the past.
		delayBuffer[writeIdx] = inSample;

		// now seek backwards by our delay time to construct our fractional delay sample
		const int readIdx1 = readFrame1*numChannels + c;
		const int readIdx2 = readFrame2*numChannels + c;
		const float delaySample = (1.0 - frameLerp)*delayBuffer[readIdx1] + frameLerp*delayBuffer[readIdx2];
		
		// accumulate feedback into the buffer
		delayBuffer[writeIdx] += delaySample*feed;

		// audible output is the delayed signal scaled by wet mix.
		// plus the incoming signal scaled by dry mix.
		out[c] = inSample*dry.getLastValue() + delaySample*wet.getLastValue();
    }
    
    // next output frame
	writeFrame = (writeFrame + 1) % bufferFrameLength;
    
    updateStepSize();
    
    // step the LFO
    step += stepSize;
    if ( step > 1 )
    {
        step -= 1;
    }
}

void Minim::Flanger::resetBuffer()
{
    if ( delayBuffer )
    {
        delete [] delayBuffer;
        delayBuffer = NULL;
    }
    
    // support up to a full second
    int sampleCount = (int)( sampleRate() );
    int bufferSize  = sampleCount * getAudioChannelCount();
    delayBuffer = new float[bufferSize];
    memset( delayBuffer, 0, sizeof(float)*bufferSize );
    writeFrame = 0;
    bufferFrameLength = sampleCount;
}

void Minim::Flanger::updateStepSize()
{
    float currFreq = getRate();
    if ( prevFreq != currFreq )
    {
        stepSize = currFreq * oneOverSampleRate;
        prevFreq = currFreq;
    }
}

float Minim::Flanger::getRate() const
{
    float r = rate.getLastValue();
    return r > 0.001f ? r : 0.001f;
}
