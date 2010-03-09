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
#include <vector>

namespace Minim  
{
	
	class Wavetable : public Waveform
	{
	public:
		Wavetable( int size );
		Wavetable( const std::vector<float> & wave );
		
		void setWaveform( const std::vector<float> & waveform ) { mWaveform = waveform; }
		
		float get( int index ) const;
		void  set( int index, float value );
		
		int size() const { return (int)mWaveform.size(); }
		
		// Waveform impl: at should be between 0 and 1.
		virtual float value( float at ) const;
		
		std::vector<float> & getWaveform() { return mWaveform; }
		const std::vector<float> & getWaveform() const { return mWaveform; }
		
		// tranforms
		void scale( float scaleBy );
		void normalize();
		void invert();
		void flip( float around );
		void rectify();
		// void smooth( int windowLength );
		
	private:
		std::vector<float> mWaveform;
		
	};
	
}

#endif // WAVETABLE_H