/*
 *  Waveshaper.h
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

#ifndef WAVESHAPER_H
#define WAVESHAPER_H

#include "UGen.h"

namespace Minim 
{
	class Wavetable;
	
	class WaveShaper : public UGen
	{
	public:
		// will delete mapShape on deconstruction
		WaveShaper( const float outAmp, const float mapAmp, Wavetable * mapShape, const bool bWrapMap = false );
		virtual ~WaveShaper();
		
		/**
		 * The default input is "audio."
		 */
		UGenInput audio;
		
		/**
		 * The output amplitude
		 */
		UGenInput outAmplitude;
		
		/**
		 * The mapping amplitude of the input signal
		 */
		UGenInput mapAmplitude;
		
		Wavetable & getWavetable() { return *m_pMapShape; }
		
		float getMapLookup( const float in ) const;
		
	protected:
		
		// ugen implementation
		virtual void uGenerate( float * channels, const int numChannels );
		
	private:
		
		// the wavetable we use to shape our input
		Wavetable * m_pMapShape;
		// whether or not to wrap around or clamp at the edges of the map shape.
		bool		m_bWrapMap;
	};
}

#endif // WAVESHAPER_H

