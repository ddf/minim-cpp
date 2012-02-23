/*
 *   Author: Damien Di Fede <ddf@compartmental.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "UGen.h"
#include "AudioSystem.h"
#include "AudioOutput.h"
#include <stdio.h> // for sprintf
#include <string.h> // for memcpy
#include <algorithm> // for fill
#include <cassert>

#ifndef NULL
#define NULL 0
#endif

namespace Minim
{

///////////////////////////////////////////////////////////////
UGen::UGenInput::UGenInput( UGen & outerUGen, UGen::InputType inputType, float defaultValue )
: mOuterUGen(outerUGen) 
, mInputType(inputType)
, mIncoming(NULL)
, mChannelCount(1)
, mNextInput(NULL)
{
    mNextInput         = mOuterUGen.mInputs;
	mOuterUGen.mInputs = this;
	
	mLastValues = new float[1];
	mLastValues[0] = defaultValue;
}
	
///////////////////////////////////////////////////
UGen::UGenInput::~UGenInput()
{
	delete [] mLastValues;
}

///////////////////////////////////////////////////
const char * UGen::UGenInput::getInputTypeAsString() const
{
	switch (mInputType)
	{
	case AUDIO :
		return "AUDIO";
		break;
	case CONTROL :
		return "CONTROL";
		break;
	}
	return "";
}
	
///////////////////////////////////////////////////
void UGen::UGenInput::setChannelCount( const int numChannels )
{
	if ( mChannelCount < numChannels )
	{
		delete [] mLastValues;
		mLastValues = new float[numChannels];
		memset(mLastValues, 0, sizeof(float)*numChannels);
	}
	
	mChannelCount = numChannels;
	
	if ( mInputType == AUDIO && isPatched() )
	{
		mIncoming->setAudioChannelCount( numChannels );
	}
}
    
void UGen::UGenInput::setIncomingUGen(Minim::UGen* inUGen )
{
    mIncoming = inUGen;
    if ( mInputType == AUDIO )
    {
        if ( mIncoming )
        {
            mIncoming->setAudioChannelCount( mChannelCount );
        }
        
        // nuke old values
        memset(mLastValues, 0, sizeof(float)*mChannelCount);
    }
}

///////////////////////////////////////////////////
UGen::UGen()
: mInputs(NULL) 
, mChannelCount(1) // assume mono
, mLastValues( new float[mChannelCount] ) 
, mSampleRate(0)
, mNumOutputs(0)
, mCurrentTick(0)
{
    // don't start with garbage
    mLastValues[0] = 0;
}
		  
UGen::~UGen()
{	
	delete [] mLastValues;
}

///////////////////////////////////////////////////
UGen & UGen::patch( UGen & connectToUGen )
{
	connectToUGen.addInput( this );

	// TODO jam3: nOutputs should only increase when this chain will be ticked!
	mNumOutputs += 1;
	
	setSampleRate( connectToUGen.sampleRate() );

	return connectToUGen;
}

////////////////////////////////////////////////////
UGen & UGen::patch( UGenInput & connectToInput )
{
	connectToInput.setIncomingUGen( this );

	mNumOutputs += 1;
	
	setSampleRate( connectToInput.getOuterUGen().sampleRate() );

	return connectToInput.getOuterUGen();
}

///////////////////////////////////////////////////
void UGen::patch( AudioOutput & output )
{
	patch( output.mSummer );
	setSampleRate( output.sampleRate() );
	setAudioChannelCount( output.getFormat().getChannels() );
}

///////////////////////////////////////////////////
void UGen::unpatch( AudioOutput & output )
{
	unpatch( output.mSummer );
}

///////////////////////////////////////////////////
void UGen::unpatch( UGen & disconnectFrom )
{
	disconnectFrom.removeInput( this );
	mNumOutputs -= 1;
}

//////////////////////////////////////////////////
void UGen::addInput( UGen * input )
{
    // default behavior is to attach to first audio input we find
    UGenInput* in = mInputs;
    while( in )
	{
        if ( in->getInputType() == AUDIO )
        {
            in->setIncomingUGen( input );
            return;
        }
        
        in = in->next();
	}  

    Minim::error("Trying to connect to UGen with no audio input.");
}

///////////////////////////////////////////////////
void UGen::removeInput(Minim::UGen *input)
{
	Minim::debug("UGen removeInput called.");
	// see if any of our ugen inputs currently have input as the incoming ugen
	// set their incoming ugen to null if that's the case
    UGenInput* in = mInputs;
    while( in )
	{
		if ( in->getIncomingUGen() == input )
		{
			in->setIncomingUGen( NULL );
		}
        
        in = in->next();
	}
}


///////////////////////////////////////////////////
void UGen::tick(float *channels, const int numChannels)
{
    assert( "Tried to tick a UGen with the wrong number of channels!" );
	
	if (0 == mCurrentTick) 
	{	
		UGenInput * in = mInputs;
        while ( in )
		{		
			in = in->tick();
		}
		
		// and then uGenerate for this UGen	
		uGenerate( mLastValues, numChannels );
	}

	memcpy(channels, mLastValues, sizeof(float) * numChannels);
	
	if (mNumOutputs > 1)
	{
		// only tick once per sampleframe when multiple outputs
		mCurrentTick = (mCurrentTick + 1)%(mNumOutputs);
	}
}

/////////////////////////////////////////////////////
void UGen::setSampleRate(float newSampleRate)
{
	if ( mSampleRate != newSampleRate)
	{
		mSampleRate = newSampleRate;
		sampleRateChanged();
	}
	
    UGenInput* in = mInputs;
    while( in )
	{
		UGenInput & input = *in;
		if ( input.isPatched() )
		{
			input.setSampleRate(newSampleRate);
		}			
        
        in = in->next();
	}
}

/////////////////////////////////////////////////////
void UGen::setAudioChannelCount( int numberOfChannels )
{
    if ( mChannelCount != numberOfChannels )
	{
		if ( mChannelCount < numberOfChannels )
		{
			delete [] mLastValues;
			mLastValues = new float[numberOfChannels];
			memset(mLastValues, 0, sizeof(float)*numberOfChannels);
		}
		
		mChannelCount = numberOfChannels;
		channelCountChanged();
	}
	
	UGenInput* in = mInputs;
    while( in )
	{
		UGenInput & input = *in;
		if ( input.getInputType() == AUDIO )
		{
			input.setChannelCount(numberOfChannels);
		}
        
        in = in->next();
	}
}

////////////////////////////////////////////////////
void UGen::printInputs() const
{
	UGenInput* in = mInputs;
    while( in )
	{
//	   char msg[64];
//	   sprintf( msg, "uGenInputs %d ", i );
//	   Minim::debug(msg);
	   // mInputs[i]->printInput();
        
        in = in->next();
	}
}
	
////////////////////////////////////////////////////
void UGen::fill( float * sampleFrame, const float value, const int numChannels )
{
	switch (numChannels)
	{
		case 1:
            sampleFrame[0] = value;
            break;
            
		case 2:
            sampleFrame[0] = sampleFrame[1] = value;
			break;
			
		default:
			std::fill(sampleFrame, sampleFrame + numChannels, value );
			break;
	}
}
    
////////////////////////////////////////////////////
void UGen::accum( float * accumFrame, const float * sampleFrame, const int numChannels, const float scale )
{
    switch (numChannels)
    {
        case 1:
            accumFrame[0] += sampleFrame[0] * scale;
            break;
            
        case 2:
            accumFrame[0] += sampleFrame[0] * scale;
            accumFrame[1] += sampleFrame[1] * scale;
            break;
            
        default:
            for( int c = 0; c < numChannels; ++c )
            {
                accumFrame[c] += sampleFrame[c] * scale;
            }
            break;
    }
}

} // namespace Minim
