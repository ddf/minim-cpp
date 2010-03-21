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

#include "Summer.h"
#include "AudioOutput.h"
#include <string.h> // for memset

#ifndef NULL
#define NULL 0
#endif	

namespace Minim
{
	
	static int kUGensDefaultSize = 10;

Summer::Summer()
: mOutput(NULL)
, m_accum(NULL)
, m_accumSize(0)
{
	initUGenList();
}

Summer::Summer(AudioOutput * out)
: mOutput(out)
, mUGensSize(kUGensDefaultSize)
{
	initUGenList();

	m_accumSize = out->getFormat().getChannels();
	m_accum = new float[ m_accumSize ];
}
	
Summer::~Summer()
{
	if ( m_accum )
	{
		delete [] m_accum;
	}
} 
	
///////////////////////////////////////////////////
void Summer::sampleRateChanged()
{
	for (int i = 0; i < mUGensSize; i++)
	{
		UGen * u = mUGens[i];
		if ( u )
		{
			u->setSampleRate(sampleRate());
		}
	}
}
	
	///////////////////////////////////////////////
	void Summer::addInput( UGen * in )
	{
		// find a slot and stick it there!
		for(int i = 0; i < mUGensSize; ++i)
		{
			if ( mUGens[i] == NULL )
			{
				mUGens[i] = in;
				return;
			}
		}
		
		// TODO: grow the list if we run out of slots.
	}
	
	///////////////////////////////////////////////
	void Summer::removeInput( UGen * in )
	{
		// find it in our list and remove it!
		for(int i = 0; i < mUGensSize; ++i)
		{
			if ( mUGens[i] == in )
			{
				mUGens[i] = NULL;
				return;
			}
		}
	}

///////////////////////////////////////////////////
void Summer::uGenerate(float * channels, int numChannels)
{	
	// if we were constructed with an output
	// we need to tick that output's noteManager!
	if ( mOutput )
	{
		// TODO
		// out->noteManager.tick();
	}
	
	// resize our accumulation buffer if it's not there or is too small
	if ( m_accum == NULL || m_accumSize < numChannels )
	{
		if ( m_accum )
		{
			delete [] m_accum;
		}
		
		m_accum = new float[ numChannels ];
		m_accumSize = numChannels;
	}

	for(int i = 0; i < mUGensSize; ++i)
	{
		UGen * u = mUGens[i];
		if ( u )
		{
			memset(m_accum, 0, sizeof(float) * numChannels);
			u->tick( m_accum, numChannels );
			for(int c = 0; c < numChannels; ++c)
			{
				channels[c] += m_accum[c];
			}
		}
	}
}
	
	void Summer::initUGenList()
	{
		mUGensSize = kUGensDefaultSize;
		mUGens = new UGen*[mUGensSize];
		memset(mUGens, 0, sizeof(UGen*)*kUGensDefaultSize);
	}

} // namespace Minim