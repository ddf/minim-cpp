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

#ifndef MULTIPLIER_H
#define MULTIPLIER_H

#include "UGen.h"

namespace Minim 
{
	
	class Multiplier : public UGen 
	{
	public:
		Multiplier( float mult = 0.f );
		virtual ~Multiplier();
		
		UGenInput * const audio;
		UGenInput * const amplitude;
		
		void setValue( float mult ) { m_mult = mult; }
		
	protected:
		
		// UGen impl
		void uGenerate( float * channels, int numChannels );
		
	private:
		float m_mult;
	};
	
}

#endif // MULTIPLIER_H