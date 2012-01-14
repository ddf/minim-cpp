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

#ifndef SUMMER_H
#define SUMMER_H

#include "UGen.h"
#include "BMutex.hpp"

namespace Minim
{
	class Summer : public UGen
	{

	public:
		Summer();
		virtual ~Summer();
	
		UGenInput volume;

	protected:
		
		// ddf: override because everything that patches to us
		//      goes into our list. then when we generate a sample
		//      we'll sum the audio generated by all of the ugens patched to us.
		// Override
		virtual void addInput(UGen * input);
		
		// Override
		virtual void removeInput(UGen * input);

		// override
		virtual void sampleRateChanged();
		
		// override
		virtual void channelCountChanged();

	public:
		// UGen impl
		virtual void uGenerate(float * channels, const int numChannels);

	private:
	
		UGen** m_inputs;
		int	   m_inputsLength;	
		
		// array we accumulate samples into when we do our summing
		float * m_accum;
		int     m_accumSize;

        BMutex  m_mutex;
		
	};
};

#endif // SUMMER_H