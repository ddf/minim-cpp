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
#include "Logging.h"
#include <vector>

#define  _USE_MATH_DEFINES // required in windows to get constants like M_PI
#include <math.h>
#include <stdlib.h>

namespace Minim
{
	
	namespace Waves 
	{
		Wavetable * PHASOR(int tableSize)
		{
			float val[] = { 0, 1 };
			int   dist[] = { tableSize };
			return gen7( tableSize, val, 2, dist, 1 );
		}
		
		Wavetable * SINE(int tableSize) 
		{
			float amp[] = { 1.f };
			return gen10(tableSize, amp, 1);
		}
		
		Wavetable * SAW(int tableSize)
		{
			float val[] = { 0, -1, 1, 0 };
			int dist[] = { tableSize/2, 0, tableSize - tableSize/2 };
			return gen7(tableSize, val, 4, dist, 3);
		}
		
		Wavetable * SQUARE(int tableSize)
		{
			float val[] = { -1, -1, 1, 1 };
			int dist[] = { tableSize/2, 0, tableSize - tableSize/2 };
			return gen7(tableSize, val, 4, dist, 3);
		}
		
		Wavetable * TRIANGLE(int tableSize)
		{
			float val[] = { 0, 1, -1, 0 };
			int dist[] = { tableSize/4, tableSize/2, tableSize - tableSize/2 - tableSize/4 };
			return gen7(tableSize, val, 4, dist, 3);
		}
		
		Wavetable * QUARTERPULSE(int tableSize)
		{
			float val[] = { -1, -1, 1, 1};
			int dist[] = { tableSize/4, 0, tableSize - tableSize/4 };
			return gen7(tableSize, val, 4, dist, 3);
		}
		
		Wavetable * pulse( float dutyCycle, int tableSize )
		{
			//TODO exception for floats higher than 1
			float val[] = { 1, 1, -1, -1 };
			int dist[] = { (int)(dutyCycle*tableSize), 0, tableSize-(int)(dutyCycle*tableSize) };
			return gen7(tableSize, val, 4, dist, 3);
		}
        
		Wavetable * sawh(int numberOfHarmonics, int tableSize)
		{
			float* content = new float[numberOfHarmonics];
			for (int i = 0; i < numberOfHarmonics; ++i)
			{
				content[i] = (float)((-2) / ((i + 1) * M_PI) * pow(-1, i + 1));
			}
			Wavetable* result = gen10(tableSize, content, numberOfHarmonics);
			delete[] content;
			return result;
		}

		Wavetable * squareh(int numberOfHarmonics, int tableSize)
		{
			float* content = new float[numberOfHarmonics + 1];
			for (int i = 0; i < numberOfHarmonics; i += 2)
			{
				content[i] = 1.0f / (i + 1);
				content[i + 1] = 0;
			}
			Wavetable* result = gen10(tableSize, content, numberOfHarmonics+1);
			delete[] content;
			return result;
		}

		Wavetable* triangleh(int numberOfHarmonics, int tableSize)
		{
			float* content = new float[numberOfHarmonics + 1];
			for (int i = 0; i < numberOfHarmonics; i += 2)
			{
				content[i] = (float)(pow(-1, i / 2) * 8 / M_PI / M_PI / pow(i + 1, 2));
				content[i + 1] = 0;
			}
			Wavetable* result = gen10(tableSize, content, numberOfHarmonics + 1);
			delete[] content;
			return result;
		}

		Wavetable* randomNHarms(int numberOfHarms, int tableSize)
        {            
            float* harmAmps = new float[numberOfHarms];
            for( int i = 0; i < numberOfHarms; i++ )
            {
                harmAmps[i] = 2.0f * rand() / (RAND_MAX + 1.0f) - 1;
            }
            Wavetable* builtWave = gen10(tableSize, harmAmps, numberOfHarms);
            builtWave->normalize();
            delete[] harmAmps;
            return builtWave;
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