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

#include "Waves.h"
#include "AudioSystem.h"
#include <vector>

#define  _USE_MATH_DEFINES // required in windows to get constants like M_PI
#include <math.h>

static const int tableSize = 8192;
static const int tSby2 = tableSize/2;
static const int tSby4 = tableSize/4;

namespace Minim
{
	
	namespace Waves 
	{
		Wavetable * PHASOR()
		{
			float val[] = { 0, 1 };
			int   dist[] = { tableSize };
			return gen7( tableSize, val, 2, dist, 1 );
		}
		
		Wavetable * SINE() 
		{
			float amp[] = { 1.f };
			return gen10(tableSize, amp, 1);
		}
		
		Wavetable * SAW()
		{
			float val[] = { 0, -1, 1, 0 };
			int dist[] = { tSby2, 0, tableSize - tSby2 };
			return gen7(tableSize, val, 4, dist, 3);
		}
		
		Wavetable * SQUARE()
		{
			float val[] = { -1, -1, 1, 1 };
			int dist[] = { tSby2, 0, tableSize - tSby2 };
			return gen7(tableSize, val, 4, dist, 3);
		}
		
		Wavetable * TRIANGLE()
		{
			float val[] = { 0, 1, -1, 0 };
			int dist[] = { tSby4, tSby2, tableSize - tSby2 - tSby4 };
			return gen7(tableSize, val, 4, dist, 3);
		}
		
		Wavetable * QUARTERPULSE()
		{
			float val[] = { -1, -1, 1, 1};
			int dist[] = { tSby4, 0, tableSize - tSby4 };
			return gen7(tableSize, val, 4, dist, 3);
		}
		
		Wavetable * pulse( float dutyCycle )
		{
			//TODO exception for floats higher than 1
			float val[] = { 1, 1, -1, -1 };
			int dist[] = { (int)(dutyCycle*tableSize), 0, tableSize-(int)(dutyCycle*tableSize) };
			return gen7(tableSize, val, 4, dist, 3);
		}
		
		Wavetable * gen7( const int size, const float * vals, const int nVals, const int * dists, const int nDists )
		{
			
			// check lenghts of arrays
			if ( nVals - 1 != nDists )
			{
				Minim::error("Input arrays of invalid sizes!");
				return NULL;
			}
			
			// check if size is sum of dists
			int sum = 0;
			for (int i = 0; i < nDists; i++)
			{
				sum += dists[i];
			}
			if (size != sum)
			{
				Minim::error("Distances do not sum to size!");
				return NULL;
			}
			
			Wavetable * table = new Wavetable(size);
			float * waveform = table->getWaveform();
			
			int i = 0;
			for (int j = 1; j < nVals; j++)
			{
				waveform[i] = vals[j - 1];
				float m = (vals[j] - vals[j - 1]) / (float)(dists[j - 1]);
				for (int k = i + 1; k < i + dists[j - 1]; k++)
				{
					waveform[k] = m * (k - i) + vals[j - 1];
				}
				i += dists[j - 1];
			}
			waveform[size - 1] = vals[nVals - 1];
			
			// for(int n = 0; n < waveform.length; n++)
			// System.out.println(waveform[n]);
			
			return table;
		}
		
		Wavetable * gen9( int size, 
						  float * partials, int nPartials, 
						  float * amps, int nAmps, 
						  float * phases, int nPhases 
						)
		{
			if ( nPartials != nAmps 
				|| nPartials != nPhases
				|| nAmps != nPhases )
			{
				Minim::error("Input arrays of different size!");
				return NULL;
			}
			
			Wavetable * table = new Wavetable( size );
			float * waveform = table->getWaveform();
			
			float index = 0;
			for (int i = 0; i < size; i++)
			{
				index = (float)i / (size - 1);
				for (int j = 0; j < nPartials; j++)
				{
					waveform[i] += amps[j] * (float)sin(2 * M_PI * partials[j] * index + phases[j]);
				}
			}
			
			return table;
		}
		
		Wavetable * gen10( int size, float * amps, int nAmps )
		{
			Wavetable * table = new Wavetable(size);
			float * waveform = table->getWaveform();
			
			float index = 0;
			for (int i = 0; i < size; i++)
			{
				index = (float)i / (size - 1);
				for (int j = 0; j < nAmps; j++)
				{
					waveform[i] += amps[j] * (float)sin( 2 * M_PI * (j + 1) * index );
				}
			}
			
			return table;
		}
	}
	
}