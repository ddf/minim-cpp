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

#include "Frequency.h"

#include <math.h>

#define HZA4       440.0f
#define MIDIA4     69.0f
#define MIDIOCTAVE 12.0f

namespace Minim
{
	
	Frequency::Frequency( float hz )
	: mFreq( hz )
	{
	}
	
	Frequency Frequency::ofHertz( float hz )
	{
		return Frequency(hz);
	}
	
    Frequency Frequency::ofMidiNote( float midiNote )
    {
        float hz = HZA4*powf( 2.0f, ( midiNote - MIDIA4 )/MIDIOCTAVE );
		return Frequency(hz);
    }

	float Frequency::asMidiNote() const
	{
		float midiNote = MIDIA4 + MIDIOCTAVE*(float)logf(mFreq / HZA4) / (float)logf(2.0);
		return midiNote;
	}
}