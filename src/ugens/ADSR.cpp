//
//  ADSR.cpp
//  MinimTouch
//
//  Created by Damien Di Fede on 12/9/11.
//  Copyright (c) 2011 Damien Di Fede. All rights reserved.
//

#include "ADSR.h"
#include "AudioOutput.h"

//-----------------------------
Minim::ADSR::ADSR( float maxAmp, float attTime, float decTime, float susLvl, float relTime, float befAmp, float aftAmp )
: UGen()
, audio( *this, AUDIO )
, maxAmplitude(maxAmp)
, attackTime(attTime)
, decayTime(decTime)
, sustainLevel(susLvl)
, releaseTime(relTime)
, beforeAmplitude(befAmp)
, afterAmplitude(aftAmp)
, isTurnedOn(false)
, isTurnedOff(false)
, timeFromOn(-1)
, timeFromOff(-1)
, bUnpatchAfterRelease(false)
, outputToUnpatch(NULL)
, ugenOutputToUnpatch(NULL)
{
    
}

//-----------------------------
void Minim::ADSR::setParameters( float maxAmp, float attTime, float decTime, float susLvl, float relTime, float befAmp, float aftAmp )
{
    maxAmplitude    = maxAmp;
    attackTime      = attTime;
    decayTime       = decTime;
    sustainLevel    = susLvl;
    releaseTime     = relTime;
    beforeAmplitude = befAmp;
    afterAmplitude  = aftAmp;
}

//-----------------------------
void Minim::ADSR::noteOn()
{
    timeFromOn  = 0;
    isTurnedOn  = true;
    
    timeFromOff = -1;
    isTurnedOff = false;
}

//-----------------------------
void Minim::ADSR::noteOff()
{
    timeFromOff = 0;
    isTurnedOff = true;
}

//-----------------------------
void Minim::ADSR::unpatchAfterRelease( Minim::AudioOutput* from )
{
    bUnpatchAfterRelease = true;
    outputToUnpatch      = from;
}

//-----------------------------
void Minim::ADSR::unpatchAfterRelease( Minim::UGen* from )
{
    bUnpatchAfterRelease = true;
    ugenOutputToUnpatch  = from; 
}

//-----------------------------
void Minim::ADSR::uGenerate( float* channels, int numChannels )
{
    if ( !isTurnedOn )
    {
        for( int i = 0; i < numChannels; ++i )
        {
            channels[i] = beforeAmplitude*audio.getLastValues()[i];
        }
    }
    else if ( timeFromOff > releaseTime )
    {
        for ( int i = 0; i < numChannels; ++i )
        {
            channels[i] = afterAmplitude*audio.getLastValues()[i];
        }
        
        if ( bUnpatchAfterRelease )
        {
            if ( outputToUnpatch )
            {
                unpatch( *outputToUnpatch );
                outputToUnpatch = NULL;
            }
            if ( ugenOutputToUnpatch )
            {
                unpatch( *ugenOutputToUnpatch );
                ugenOutputToUnpatch = NULL;
            }
            
            bUnpatchAfterRelease = false;
        }
    }
    else
    {
        if ( isTurnedOn && !isTurnedOff )
        {
            // ATTACK
            if (timeFromOn <= attackTime)
            {
                // use time remaining until maxAmplitude to change amplitude
                float timeRemain = (attackTime - timeFromOn);
                amplitude += (maxAmplitude - amplitude)*timeStepSize/timeRemain;				
            }
            // DECAY
            else if ((timeFromOn > attackTime) && (timeFromOn <= (attackTime+decayTime)))
            {
                // use time remaining until sustain to change to sustain level
                float timeRemain = (attackTime + decayTime - timeFromOn);
                amplitude += (sustainLevel*maxAmplitude - amplitude)*timeStepSize/timeRemain;			
            }
            // SUSTAIN
            else if (timeFromOn > (attackTime+decayTime))
            {
                // hold the sustain level
                amplitude = sustainLevel*maxAmplitude;
            }
            timeFromOn += timeStepSize;
        }
        // RELEASE
        else //isTurnedOn and isTurnedOFF and timeFromOff < releaseTime
        {
            // use remaining time to get to afterAmplitude
            float timeRemain = (releaseTime - timeFromOff);
            amplitude += (afterAmplitude - amplitude)*timeStepSize/timeRemain;
            timeFromOff += timeStepSize;
        }
        // finally multiply the input audio to generate the output
        for(int i = 0; i < numChannels; i++)
        {
            channels[i] = amplitude*audio.getLastValues()[i];
        }	        
    }
}
