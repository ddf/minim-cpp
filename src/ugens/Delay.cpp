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

Minim::Delay::Delay( const float maxDT, const float ampFactor, const bool feedback, const bool passAudio )
: UGen(5)
, audio( *this, AUDIO )
, delTime( *this, CONTROL )
, delAmp( *this, CONTROL )
, dryMix( *this, CONTROL )
, wetMix( *this, CONTROL )
, maxDelayTime( maxDT )
, amplitudeFactor( ampFactor )
, feedBackOn( feedback )
, passAudioOn( passAudio )
, iBufferIn( 0 )
, iBufferOut( 0 )
, bufferSize( 0 )
, maxBufferSize( 0 )
, delayBuffer( NULL )
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
	if ( delayBuffer )
	{
		delete[] delayBuffer;
        delayBuffer = NULL;
	}
	
	if ( getAudioChannelCount() > 0 )
	{
		maxBufferSize = (int)( maxDelayTime*sampleRate()*getAudioChannelCount() );
		delayBuffer = new float[maxBufferSize];
		memset(delayBuffer, 0, sizeof(float)*maxBufferSize);
		bufferSizeChanged();
	}
}

void Minim::Delay::channelCountChanged()
{
	if ( delayBuffer )
	{
		delete[] delayBuffer;
        delayBuffer = NULL;
	}
	
	if ( sampleRate() > 0 )
	{
		maxBufferSize = (int)( maxDelayTime*sampleRate()*getAudioChannelCount() );
		delayBuffer = new float[maxBufferSize];
		memset(delayBuffer, 0, sizeof(float)*maxBufferSize);
		bufferSizeChanged();
	}
}

void Minim::Delay::bufferSizeChanged()
{
	const int oldBufferSize = bufferSize;
	const int newBufferSize = (int)( delayTime*sampleRate()*getAudioChannelCount() );
	if ( oldBufferSize != newBufferSize && newBufferSize > 0 )
	{
		if ( newBufferSize < oldBufferSize )
		{
			std::fill( delayBuffer + newBufferSize, delayBuffer + oldBufferSize, 0.0f );
		}
		bufferSize = newBufferSize;
		iBufferOut = ( iBufferIn + getAudioChannelCount() ) % bufferSize;
	}
}

void Minim::Delay::uGenerate( float * out, const int numChannels )
{
	// update the buffer indexes
	delayTime = delTime.getLastValue();
	bufferSizeChanged();
	
	// update the feedbackFactor
	amplitudeFactor = delAmp.getLastValue();
	
	// apply to each channel
	for (int c = 0; c < numChannels; ++c )
	{
		float tmpIn = audio.getLastValues()[c];
	
		// pull sound out of the buffer
		float tmpOut = amplitudeFactor*delayBuffer[ iBufferOut ];
	
		// put sound into the buffer
		delayBuffer[ iBufferIn ] = tmpIn;
		if ( feedBackOn ) 
		{
			delayBuffer[ iBufferIn ] += tmpOut; 
		}
	
		tmpOut *= wetMix.getLastValue();
	
		iBufferIn  = ( iBufferIn  + 1 )%bufferSize;
		iBufferOut = ( iBufferOut + 1 )%bufferSize;
	
		// pass the audio if necessary
		if ( passAudioOn )
		{
			tmpOut += tmpIn * dryMix.getLastValue();
		}
		
		out[c] = tmpOut;
	}
}