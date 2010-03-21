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
#include <algorithm>

namespace Minim
{

Summer::Summer()
: mOutput(NULL)
, m_accum(NULL)
, m_accumSize(0)
{
}

Summer::Summer(AudioOutput * out)
: mOutput(out)
{
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
	std::vector<UGen*>::iterator itr = mUGens.begin();
	for ( ; itr != mUGens.end(); ++itr) 
	{
		UGen * u = *itr;
		u->setSampleRate(sampleRate());
	}
}

///////////////////////////////////////////////////
void Summer::uGenerate(float * channels, int numChannels)
{	
	// if we were constructed with an output
	// we need to tick that output's noteManager!
	if ( mOutput != NULL )
	{
		// TODO
		// out->noteManager.tick();
	}
	
	// resize our accumulation buffer if it's not there or is too small
	if ( m_accum == NULL || m_accumSize < numChannels )
	{
		if ( m_accum == NULL )
		{
			delete [] m_accum;
		}
		
		m_accum = new float[ numChannels ];
		m_accumSize = numChannels;
	}

	int size = mUGens.size();
	for(int i = 0; i < size; ++i)
	{
		memset(m_accum, 0, sizeof(float) * numChannels);
		UGen * u = mUGens[i];
		u->tick( m_accum, numChannels );
		for(int c = 0; c < numChannels; c++)
		{
			channels[c] += m_accum[c];
		}
	}

	
	// now remove anything that's marked itself for removal
	size = mToRemove.size(); 
	for(int i = 0; i < size; ++i)
	{
		std::vector<UGen*>::iterator toRemove = std::find(mUGens.begin(), mUGens.end(), mToRemove[i]);
		if ( toRemove != mUGens.end() )
		{
			mUGens.erase( toRemove );
		}
	}
	
	if ( size > 0 )
	{
		mToRemove.clear();
	}
}

} // namespace Minim