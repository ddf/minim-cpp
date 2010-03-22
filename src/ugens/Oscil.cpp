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

namespace Minim  
{
	
	Oscil::Oscil( const Frequency & freq, float amp, Waveform * wave )
	: UGen(4)
	, amplitude( *this, CONTROL ) 
	, amplitudeModulation( *this, CONTROL )
	, frequency( *this, CONTROL )
	, frequencyModulation( *this, CONTROL )
	, mWaveform(wave)
	, mBaseFreq(freq)
	, mFreq(freq)
	, mAmp(amp)
	, mStep(0.f)
	{
	}
	
	Oscil::~Oscil()
	{
		delete mWaveform;
	}
	
	///////////////////////////////////////////////////////
	void Oscil::sampleRateChanged()
	{
		stepSizeChanged();
	}
	
	void Oscil::stepSizeChanged()
	{
		mStepSize = mFreq.asHz() / sampleRate();
	}
	
	///////////////////////////////////////////////////////
	void Oscil::setFrequency( float hz )
	{
		mBaseFreq = Frequency::ofHertz( hz );
		mFreq = mBaseFreq;
		stepSizeChanged();
	}
	/**
	 * Sets the frequency to a Frequency after construction.
	 * @param freq
	 */
	void Oscil::setFrequency( const Frequency & freq )
	{
		mBaseFreq = freq;
		mFreq = mBaseFreq;
		stepSizeChanged();
	}
	
	/////////////////////////////////////////////////////////
	void Oscil::uGenerate(float * channels, const int numChannels) 
	{		
		// figure out our sample value
		float outAmp(mAmp);
		// if something is plugged into amplitude
		if ( amplitude.isPatched() )
		{
			outAmp = amplitude.getLastValues()[0];
		}
		
		// calculte the sample values
		float sample = outAmp * mWaveform->value(mStep);
		
		// if something has been plugged into amplitudeModulation
		if ( amplitudeModulation.isPatched() )
		{
			sample += sample * amplitudeModulation.getLastValues()[0];
		}
		
		for(int i = 0; i < numChannels; i++)
		{
			channels[i] = sample;
		}
		
		// if something is plugged into frequency
		if ( frequency.isPatched() )
		{
			mBaseFreq.setAsHz( frequency.getLastValues()[0] );
			stepSizeChanged();
		}
		
		// if something is plugged into frequencyModulation
		if ( frequencyModulation.isPatched() )
		{
			mFreq.setAsHz( mBaseFreq.asHz() + frequencyModulation.getLastValues()[0] );
			stepSizeChanged();
		}
		
		// increase time
		mStep += mStepSize;
		// make sure we don't exceed 1.0.
		// ideally, we'd use some fast way of dropping
		// the integer part of the number.
		mStep -= (int)mStep;
	}
	
}

