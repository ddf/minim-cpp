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
: UGen( 7 )
, audio( *this, AUDIO )
, delay( *this, CONTROL, delayLength )
, rate( *this, CONTROL, lfoRate )
, depth( *this, CONTROL, delayDepth )
, feedback( *this, CONTROL, feedbackAmplitude )
, dry( *this, CONTROL, dryAmplitude )
, wet( *this, CONTROL, wetAmplitude )
, delayBuffer( NULL )
, outputFrame( 0 )
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

void Minim::Flanger::setAudioChannelCount(int numberOfChannels)
{
    UGen::setAudioChannelCount( numberOfChannels );
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
    assert( numChannels == audio.getChannelCount() );
    
    // generate lfo value
    float lfo = SINE->value( step );
    
    // modulate the delay amount using the lfo value.
    // we always modulate tp a max of 5ms above the input delay.
    float dep = depth.getLastValue() * 0.5f;
    float delMS = delay.getLastValue() + ( lfo * dep + dep );
    
    // how many sample frames is that?
    int delFrame = (int)( delMS * sampleRate() / 1000 );
    
    for ( int i = 0; i < numChannels; ++i )
    {
        int outputIndex = outputFrame * audio.getChannelCount() + i;
        float inSample = audio.getLastValues()[i];
        float wetSample = delayBuffer[outputIndex];
        // eat it
        delayBuffer[outputIndex] = 0;
        
        // figure out where we need to place the delayed sample in our ring buffer
        int delIndex = ( ( outputFrame + delFrame ) * audio.getChannelCount() + i ) % (bufferFrameLength * audio.getChannelCount());
        delayBuffer[delIndex] = inSample + wetSample * feedback.getLastValue();
        
        // the output sample is in plus wet, each scaled by amplitude inputs
        out[i] = inSample * dry.getLastValue() + wetSample * wet.getLastValue();
    }
    
    // next output frame
    ++outputFrame;
    if ( outputFrame == bufferFrameLength )
    {
        outputFrame = 0;
    }
    
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
    int bufferSize  = sampleCount * audio.getChannelCount();
    delayBuffer = new float[bufferSize];
    memset( delayBuffer, 0, sizeof(float)*bufferSize );
    outputFrame = 0;
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
