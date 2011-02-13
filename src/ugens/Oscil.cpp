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

#include "Oscil.h"
#include "Frequency.h"
#include "Waveform.h"
#include <math.h>
#include "CodeTimer.h"

namespace Minim  
{
	Oscil::Oscil( const float freq, float amp, Waveform * wave )
	: UGen( 4 )
	, amplitude( *this, CONTROL ) 
	, frequency( *this, CONTROL )
	, phase( *this, CONTROL )
	, offset( *this, CONTROL )
	, mWaveform(wave)
	, mPrevFreq(freq)
	, mStep(0.f)
	, oneOverSampleRate(0)
	{
		amplitude.setLastValue( amp );
		frequency.setLastValue( freq );
		phase.setLastValue( 0.f );
		offset.setLastValue( 0.f );
	}
	
	Oscil::Oscil( const Frequency & freq, float amp, Waveform * wave )
	: UGen( 4 )
	, amplitude( *this, CONTROL ) 
	, frequency( *this, CONTROL )
	, phase( *this, CONTROL )
	, offset( *this, CONTROL )
	, mWaveform(wave)
	, mPrevFreq(freq.asHz())
	, mStep(0.f)
	, oneOverSampleRate(0)
	{
		amplitude.setLastValue( amp );
		frequency.setLastValue( freq.asHz() );
		phase.setLastValue( 0.f );
		offset.setLastValue( 0.f );
	}
	
	Oscil::~Oscil()
	{
		delete mWaveform;
	}
	
	/////////////////////////////////////////////////////////
	void Oscil::uGenerate(float * channels, const int numChannels) 
	{		
		// CodeTimer timer("Oscil:uGenerate");
		
		// figure out our sample value
		const float outAmp( amplitude.getLastValue() );
		
		float tmpStep = mStep + phase.getLastValue();
		// don't be less than zero
		if ( tmpStep < 0.f )
		{
			tmpStep -= static_cast<int>(tmpStep) - 1.f;
		}
		// don't exceed 1.
		else if ( tmpStep > 1.0f )
		{
			tmpStep -= static_cast<int>(tmpStep);
		}
		
		// calculate the sample values
		const float sample = outAmp * mWaveform->value( tmpStep ) + offset.getLastValue();
		
		for(int i = 0; i < numChannels; i++)
		{
			channels[i] = sample;
		}
		
		// update our step size if the frequency changed.
		if ( frequency.getLastValue() != mPrevFreq )
		{
			updateStepSize();
			mPrevFreq = frequency.getLastValue();
		}
		
		// increase time
		mStep += mStepSize;
		
		// don't be less than zero
		if ( mStep < 0.f )
		{
			mStep -= static_cast<int>(mStep) - 1.f;
		}
		// don't exceed 1.
		else if ( mStep > 1.0f )
		{
			mStep -= static_cast<int>(mStep);
		}	
	}
	
}

