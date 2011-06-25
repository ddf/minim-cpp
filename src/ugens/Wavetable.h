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


#ifndef	WAVETABLE_H
#define WAVETABLE_H

#include "Waveform.h"

namespace Minim  
{
	
	class Wavetable : public Waveform
	{
	public:
		/// create a wavetable with a waveform that is size samples long.
		/// does NOT initialize the waveform values.
		Wavetable( const int size );
		/// create a wavetable that holds a copy of the first size samples of waveform
		Wavetable( const float * waveform, const int size );
		virtual ~Wavetable();
		
		/// change our waveform to be a copy of the first size samples of our waveform.
		void setWaveform( const float * waveform, const int size );
		
		float get( const int index ) const;
		void  set( const int index, const float value );
		
		inline int size() const { return mSize; }
		
		// Waveform impl: at should be between 0 and 1.
		virtual float value( const float at ) const;
		
		inline float * getWaveform() { return mWaveform; }
		inline const float * getWaveform() const { return mWaveform; }
		
		// tranforms
		void scale( float scaleBy );
		void normalize();
		void invert();
		void flip( float around );
		void rectify();
		// void smooth( int windowLength );
		
		static bool s_opt;
		
	private:
		float * mWaveform;
		int		mSize;
		float	mSizeForLookup;
		
	};
	
}

#endif // WAVETABLE_H