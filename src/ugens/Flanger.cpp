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
    int delFrame = (int)( delMS * sampleRate() / 1000 );
    
    for ( int c = 0; c < numChannels; ++c )
    {
		float inSample = audio.getLastValues()[c];

		// seek backwards by our delay time
		const int readFrame = (bufferFrameLength + writeFrame - delFrame) % bufferFrameLength;
		const int readIdx = (readFrame*numChannels + c);

		// grab the sample there
		const float delaySample = delayBuffer[readIdx];

		// where to record incoming audio and mix with feedback
		const int writeIdx = writeFrame*numChannels + c;
		delayBuffer[writeIdx] = inSample + delaySample*feed;

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
