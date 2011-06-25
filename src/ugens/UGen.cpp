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

#ifndef NULL
#define NULL 0
#endif

namespace Minim
{

///////////////////////////////////////////////////////////////
UGen::UGenInput::UGenInput( UGen & outerUGen, UGen::InputType inputType )
: mOuterUGen(outerUGen) 
, mInputType(inputType)
, mIncoming(NULL)
, mChannelCount(1)
{
	mOuterUGen.mInputs[mOuterUGen.mInputCount] = this;
	mOuterUGen.mInputCount++;
	
	mLastValues = new float[1];
	mLastValues[0] = 0.f;
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

///////////////////////////////////////////////////
UGen::UGen()
: mNumOutputs(0)
, mCurrentTick(0)
, mLastValues( new float[1] ) // assume mono
, mChannelCount(1)
, mInputs(NULL)
, mInputCount(0)
{
}
	
UGen::UGen( const int numOfInputs )
: mNumOutputs(0)
, mCurrentTick(0)
, mLastValues( new float[1] ) // assume mono
, mChannelCount(1)
, mInputs( new UGenInput*[numOfInputs] )
, mInputCount(0)
{

}
		  
UGen::~UGen()
{
	if ( mInputs )
	{
		delete [] mInputs;
	}
	
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
	// jam3: This default behavior is that the incoming signal will be added
	// 		to the first input in the uGenInputs list.
	Minim::debug("UGen addInput called.");
	// TODO change input checking to an Exception?
	if ( mInputCount > 0 )
	{
		Minim::debug("Initializing default input on something");	
		UGenInput & firstInput = *mInputs[0];
		firstInput.setIncomingUGen( input );
	}  
	else
	{
		Minim::error("Trying to connect to UGen with no default input.");
	}
}

///////////////////////////////////////////////////
void UGen::removeInput(Minim::UGen *input)
{
	Minim::debug("UGen removeInput called.");
	// see if any of our ugen inputs currently have input as the incoming ugen
	// set their incoming ugen to null if that's the case
	for( int i = 0; i < mInputCount; i++)
	{
		if ( mInputs[i]->getIncomingUGen() == input )
		{
			mInputs[i]->setIncomingUGen( NULL );
		}
	}
}


///////////////////////////////////////////////////
void UGen::tick(float *channels, const int numChannels)
{
	if( mChannelCount != numChannels )
	{
		Minim::error("Tried to tick a UGen with the wrong number of channels!" );
		return;
	}
	
	if (0 == mCurrentTick) 
	{			
		if ( mInputCount > 0 )
		{
			for(int i = 0; i < mInputCount; ++i)
			{		
				mInputs[i]->tick();
			}
		}
		
		// and then uGenerate for this UGen	
		uGenerate( channels, numChannels );
		
		// need to keep the last values generated so we have something to hand multiple outputs 
		memcpy(mLastValues, channels, sizeof(float) * numChannels);
	}
	
	if (mNumOutputs > 0)
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
	for(int i = 0; i < mInputCount; ++i)
	{
		UGenInput & input = *mInputs[i];
		if ( input.isPatched() )
		{
			input.setSampleRate(newSampleRate);
		}			
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
	
	for(int i = 0; i < mInputCount; ++i)
	{
		UGenInput & input = *mInputs[i];
		if ( input.getInputType() == AUDIO )
		{
			input.setChannelCount(numberOfChannels);
		}
	}
}

////////////////////////////////////////////////////
void UGen::printInputs() const
{
	for(int i = 0; i < mInputCount; ++i)
	{
	   char msg[64];
	   sprintf( msg, "uGenInputs %d ", i );
	   Minim::debug(msg);
	   // mInputs[i]->printInput();
	}
}
	
////////////////////////////////////////////////////
void UGen::fill( float * sampleFrame, const float value, const int numChannels )
{
	switch (numChannels)
	{
		case 2:
			sampleFrame[1] = value;
			//fallthrough
		case 1:
			sampleFrame[0] = value;
			break;
			
		default:
			std::fill(sampleFrame, sampleFrame + numChannels, value );
			break;
	}
}

} // namespace Minim
