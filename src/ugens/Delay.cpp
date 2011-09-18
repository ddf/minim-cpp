/*
 *  Delay.cpp
 *  MinimOSX
 *
 *  Created by Damien Di Fede on 9/17/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Delay.h"
#include <algorithm>

Minim::Delay::Delay( float inMaxDelayTime /* = 0.25f */, float inAmplitudeFactor /* = 0.5f */, bool inFeedBackOn /* = false */, bool inPassAudioOn /* = true */ )	
: UGen(3)
, audio( *this, AUDIO )
, delTime( *this, CONTROL )
, delAmp( *this, CONTROL )
, maxDelayTime( inMaxDelayTime )
, delayTime( inMaxDelayTime )
, amplitudeFactor( inAmplitudeFactor )
, feedBackOn( inFeedBackOn )
, passAudioOn( inPassAudioOn )
, delayBuffer( NULL )
{		
	delTime.setLastValue( maxDelayTime );
	delAmp.setLastValue( amplitudeFactor );
	
	iBufferIn = 0;
	iBufferOut = 0;
	bufferSize = 0;
}

Minim::Delay::~Delay()
{
	if ( delayBuffer )
	{
		delete [] delayBuffer;
	}
}

void Minim::Delay::sampleRateChanged()
{
	if ( delayBuffer )
	{
		delete [] delayBuffer;
		delayBuffer = NULL;
	}
	
	int size = (int)( maxDelayTime*sampleRate() );
	delayBuffer = new double [ size ];
	std::fill( delayBuffer, delayBuffer+size, 0.0 );

	bufferSize = size;
	//bufferSizeChanged();
}

void Minim::Delay::bufferSizeChanged()
{
	int oldBufferSize = bufferSize;
	int newBufferSize = (int)( delayTime * sampleRate() );
	if ( newBufferSize > 0 )
	{
		if ( newBufferSize < oldBufferSize )
		{
			std::fill( delayBuffer+newBufferSize, delayBuffer+(int)( maxDelayTime*sampleRate() ), 0.0 );
		}
		bufferSize = newBufferSize;
		iBufferOut = ( iBufferIn + 1 )%bufferSize;
	}
}

void Minim::Delay::uGenerate(float * channels, const int numChannels) 
{
	// mono-ize the signal
	float tmpIn = 0;
	for( int i = 0; i < numChannels; i++ )
	{
		tmpIn += audio.getLastValues()[ i ]/numChannels;
	}
	
	// update the feedbackFactor
	amplitudeFactor = delAmp.getLastValue();
	
	// pull sound out of the buffer
	float tmpOut = amplitudeFactor*(float)delayBuffer[ iBufferOut ];
	delayBuffer[ iBufferOut ] = 0;
	
	int delSamp = (int)(delTime.getLastValue()*sampleRate());
	int iBufferIn = (iBufferOut + delSamp) % bufferSize;
	
	// put sound into the buffer
	delayBuffer[ iBufferIn ] = tmpIn;
	if ( feedBackOn ) 
	{
		delayBuffer[ iBufferIn ] +=tmpOut; 
	}
	
	// update the buffer indexes
	//delayTime = delTime.getLastValue();
	//bufferSizeChanged();
	
	//iBufferIn = ( iBufferIn + 1 )%bufferSize;
	iBufferOut = ( iBufferOut + 1 )%bufferSize;
	
	// pass the audio if necessary
	if ( passAudioOn )
	{
		tmpOut += tmpIn;
	}
	
	// put the delay signal out on all channels
	for( int i = 0; i < numChannels; i++ )
	{
		channels[ i ] = tmpOut;
	}
} 