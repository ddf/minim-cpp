/*
 *  Waveshaper.cpp
 *  MinimTouch
 *
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

#include "Waveshaper.h"
#include "Wavetable.h"
#include <math.h> // for modff

namespace Minim 
{
	WaveShaper::WaveShaper( const float outAmp, const float mapAmp, Wavetable * mapShape, const bool bWrapMap )
	: UGen( 3 )
	, audio( *this, AUDIO )
	, outAmplitude( *this, CONTROL )
	, mapAmplitude( *this, CONTROL )
	, m_pMapShape( mapShape )
	, m_bWrapMap(bWrapMap)
	{
		outAmplitude.setLastValue( outAmp );
		mapAmplitude.setLastValue( mapAmp );
	}
	
	WaveShaper::~WaveShaper()
	{
		delete m_pMapShape;
	}
	
	//the input signal is supposed to be less than 1 in amplitude 
	//as Wavetable is basically an array of floats accessed via a 0 to 1.0 index, 
	//some shifting+scaling has to be done	
	//the shape is supposed to be -1 at [0] and +1 at [length].
	void WaveShaper::uGenerate( float * channels, const int numChannels )
	{
		for( int i = 0; i < numChannels; ++i )
		{
			// bring in the audio as index, scale by the map amplitude, and normalize
			float tmpIndex = ( mapAmplitude.getLastValue()*audio.getLastValues()[i] )/2.0f + 0.5f;
			
			// handle the cases where it goes out of bouds
			if ( m_bWrapMap )  // wrap oround
			{
				// get the fractional part
				float tmpIntegral;
				tmpIndex = modff(tmpIndex, &tmpIntegral);
				// I don't like that remaider gives the same sign as the first argument
				if ( tmpIndex < 0.0f )
				{
					tmpIndex += 1.0f;
				}
			}
			else if ( tmpIndex > 1.0f )  // otherwise cap at 1
			{
				tmpIndex = 1.0f;
			} 
			else if ( tmpIndex < 0.0f ) // and cap on the bottom at 0
	        {
	        	tmpIndex = 0.0f;
	        }
			
			// now that tmpIndex is good, look up the wavetable value and multiply by outAmp
			channels[i] = outAmplitude.getLastValue()*m_pMapShape->value( tmpIndex );
		}
	}
}

