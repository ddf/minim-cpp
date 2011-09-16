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

#ifndef OSCIL_H
#define OSCIL_H

#include "UGen.h"
#include "Frequency.h"

namespace Minim 
{
	
	class Waveform;
	
	class Oscil : public UGen 
	{
	public:
		// TODO: I kind of hate that I'm gonna write it such that you 
		//       have to make a new waveform for each oscillator, but 
		//       I can't wrap my head around how to do it better right now.
		Oscil( const float freqInHz, float amplitude, Waveform * wave );
		Oscil( const Frequency & freq, float amplitude, Waveform * wave );
		virtual ~Oscil();
		
		UGenInput amplitude;
		UGenInput frequency;
		UGenInput phase;
		UGenInput offset;
		
		/**
		 * Resets the time-step used by the oscillator to zero.  
		 * You will typically use this when starting a 
		 * new note with an Oscil that you have already used so that the 
		 * waveform will begin sounding at the beginning of its period, 
		 * which will typically be a zero-crossing. In other words, use this
		 * to prevent clicks when starting Oscils that have been used before.
		 */
		void reset()
		{
			mStep = 0;
		}
		
	protected:
		// UGen override
		inline virtual void sampleRateChanged()
		{
			oneOverSampleRate = 1.f / sampleRate();
			updateStepSize();
		}
		
		// UGen impl
		virtual void uGenerate( float * channels, const int numChannels );
		
	private:
		
		inline void updateStepSize()
		{
			mStepSize = frequency.getLastValue() * oneOverSampleRate;
		}
		
		float mPrevFreq;
		
		Waveform * mWaveform;
		
		float mStep;
		float mStepSize;
		float oneOverSampleRate;
	};
	
}

#endif // OSCIL_H
