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

#include "Wavetable.h"
#include <math.h>
#include <cassert>
#include "CodeTimer.h"
#include <cstring> // for memcpy

namespace Minim
{
	bool Wavetable::s_opt(false);
	
	Wavetable::Wavetable( const int size )
	: mWaveform( new float[size] )
	, mSize(size)
	, mSizeForLookup(size-1)
	{
	}
	
	Wavetable::Wavetable( const float * waveform, const int size )
	: mWaveform( new float[size] )
	, mSize(size)
	, mSizeForLookup(size-1)
	{
		memcpy(mWaveform, waveform, sizeof(float)*size);
	}
	
	Wavetable::~Wavetable()
	{
		delete [] mWaveform;
	}
	
	////////////////////////////////////////////
	void Wavetable::setWaveform( const float * waveform, const int size )
	{
		if ( mSize < size )
		{
			delete [] mWaveform;
			mWaveform = new float[size];
		}
		
		memcpy(mWaveform, waveform, sizeof(float)*size);
		mSize = size;
		mSizeForLookup = size - 1;
	}
	
	////////////////////////////////////////////
	float Wavetable::get( int index ) const
	{
		assert( index >= 0 && index < size() && "Index out of bounds!" );
		
		return mWaveform[index];
	}
	
	////////////////////////////////////////////
	void Wavetable::set( int index, float value )
	{
		assert( index >= 0 && index < size() && "Index out of bounds!" );

		mWaveform[index] = value;
	}
	
	//////////////////////////////////////////////
	float Wavetable::value( const float at ) const
	{
		// CodeTimer timer("Wavetable::value");
		
		const float whichSample = mSizeForLookup * at;
		
		// linearaly interpolate between the two samples we want.
		// TODO: can we do a faster truncation than a cast?
		// See:  http://chrishecker.com/images/f/fb/Gdmfp.pdf
		const int lowSamp = (int)whichSample;
		
		// opt: just return the low sample instead of interping
		if ( s_opt )
		{
			return mWaveform[lowSamp];
		}

		const float rem = (whichSample - lowSamp);
		
		if ( rem == 0.f )
		{
			return mWaveform[lowSamp];
		}
		
		const int hiSamp = lowSamp+1;
		
		const float sample = mWaveform[lowSamp] * (1.f-rem) + mWaveform[hiSamp] * rem;
		
		return sample;
	}
	
	void Wavetable::scale( float scaleBy )
	{
		for (int i = 0; i < size(); i++)
		{
			mWaveform[i] *= scaleBy;
		}
	}
	
	void Wavetable::normalize()
	{
		float max = fabs(mWaveform[0]);
		for (int i = 0; i < size(); i++)
		{
			if ( fabs(mWaveform[i]) > max )
			{
				max = fabs(mWaveform[i]);
			}
		}
		scale(1.f / max);
	}
	
	void Wavetable::invert()
	{
		flip(0);
	}
	
	void Wavetable::flip(float in)
	{
		for (int i = 0; i < size(); i++)
		{
			if (mWaveform[i] > in)
			{
				mWaveform[i] = in - (mWaveform[i] - in);
			}
			else
			{
				mWaveform[i] = in + (in - mWaveform[i]);
			}
		}
	}
	
	void Wavetable::rectify()
	{
		for (int i = 0; i < size(); i++)
		{
			if ( mWaveform[i] < 0 )
			{
				mWaveform[i] *= -1.f;
			}
		}
	}
	
//	void Wavetable::smooth(int windowLength)
//	{
//		if (windowLength < 1)
//			return;
//		
//		float[] temp = (float[])waveform.clone();
//		for (int i = windowLength; i < waveform.length; i++)
//		{
//			float avg = 0;
//			for (int j = i - windowLength; j <= i; j++)
//			{
//				avg += temp[j] / windowLength;
//			}
//			waveform[i] = avg;
//		}
//	}
}