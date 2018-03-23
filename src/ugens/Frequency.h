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

#ifndef FREQUENCY_H
#define FREQUENCY_H

namespace Minim 
{
	
	class Frequency 
	{
	public:
		
		static Frequency ofHertz( float hz );
        static Frequency ofMidiNote( float midiNote );
		
		float asMidiNote() const;
		inline float asHz() const { return mFreq; }
		
		inline void setAsHz( const float hz ) { mFreq = hz; }
		
	private:
		Frequency( float hz );
		
		float mFreq;
		
	};
	
};

#endif // FREQUENCY_H

