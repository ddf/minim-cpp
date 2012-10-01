//
//  BitCrush.cpp
//  minim-cpp
//
//  Created by Damien Di Fede on 5/19/12.
//

#include "BitCrush.h"
#include <math.h>
#include <string.h>

Minim::BitCrush::BitCrush( float bitResolution, float bitRate )
: UGen()
, audio( *this, AUDIO )
, bitRes( *this, CONTROL, bitResolution )
, bitRate( *this, CONTROL, bitRate )
, sampledFrame(NULL)
, sampleCounter(0)
{
}

void Minim::BitCrush::channelCountChanged() 
{
    if ( sampledFrame )
    {
        delete [] sampledFrame;
    }
    
    sampledFrame = new float[getAudioChannelCount()];
    UGen::fill(sampledFrame, 0, getAudioChannelCount());
    sampleCounter = 0;
}

void Minim::BitCrush::uGenerate(float* out, const int numChannels)
{
    if ( sampleCounter <= 0 )
    {
        memcpy(sampledFrame, audio.getLastValues(), sizeof(float)*numChannels);
        sampleCounter = (int)(sampleRate() / fmax(bitRate.getLastValue(),0));
    }
    
    const int res       = 1 << (int)bitRes.getLastValue();
    for( int i = 0; i < numChannels; ++i )
    {
        int       samp      = (int)(res * sampledFrame[i]);
        out[i]              = (float)samp/res;
    }
    
    --sampleCounter;
}
