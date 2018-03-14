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

#ifndef	WAVES_H
#define WAVES_H

#include "Wavetable.h"

namespace Minim  
{
	namespace Waves 
	{
		enum
		{
			kDefaultTableSize = 8192
		};

		Wavetable * PHASOR(int tableSize = kDefaultTableSize);
		Wavetable * SINE(int tableSize = kDefaultTableSize);
		Wavetable * SAW(int tableSize = kDefaultTableSize);
		Wavetable * SQUARE(int tableSize = kDefaultTableSize);
		Wavetable * TRIANGLE(int tableSize = kDefaultTableSize);
		Wavetable * QUARTERPULSE(int tablesSize = kDefaultTableSize);
		
		Wavetable * sawh     ( int numberOfHarmonics, int tableSize = kDefaultTableSize );
		Wavetable * squareh  ( int numberOfHarmonics, int tableSize = kDefaultTableSize );
		Wavetable * triangleh( int numberOfHarmonics, int tableSize = kDefaultTableSize );
        
		// #TODO duty cycle versions of basic waves
		Wavetable *	pulse   ( float dutyCycle, int tableSize = kDefaultTableSize );
		Wavetable * triangle( float dutyCycle, int tableSize = kDefaultTableSize );
		Wavetable * saw     ( float dutyCycle, int tableSize = kDefaultTableSize );
		Wavetable * square  ( float dutyCycle, int tableSize = kDefaultTableSize );
        
        // returns a wavetable built out of sine waves
        // with the requested number of harmonics, 
        // where the amplitude of each harmonic is random
        Wavetable * randomNHarms( int numberOfHarmonics, int tableSize = kDefaultTableSize );
		
		Wavetable * gen7( const int size, const float * val, const int nVal, const int * dist, const int nDist );
		Wavetable * gen9( int size, 
						  float * partials, int nPartials, 
						  float * amps, int nAmps, 
						  float * phases, int nPhases 
						 );
		Wavetable * gen10( int size, float * amps, int nAmps );
	}
}

#endif // WAVES_H
