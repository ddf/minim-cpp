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
#include <string.h> // for memset, memcpy
#include <cassert>

#ifndef NULL
#define NULL 0
#endif

namespace Minim
{
	
	// static int kUGensDefaultSize = 10;

Summer::Summer()
: UGen()
, m_accum( new float[1] ) // assume mono, same as UGen
, m_accumSize(1)
, m_inputs(NULL)
, m_inputsLength(16)
, volume( *this, CONTROL )
{
	m_inputs = new UGen*[m_inputsLength];
	memset( m_inputs, 0, sizeof(UGen*)*m_inputsLength );
	volume.setLastValue( 1 );
}
	
Summer::~Summer()
{
	if ( m_accum )
	{
		delete [] m_accum;
	}

	if ( m_inputs )
	{
		delete [] m_inputs;
	}
} 
	
///////////////////////////////////////////////////
void Summer::sampleRateChanged()
{
    BMutexLock lock( m_mutex );
    
	for( int i = 0; i < m_inputsLength; ++i )
	{
		if ( m_inputs[i] )
		{
			m_inputs[i]->setSampleRate( sampleRate() );
		}
	}
}
	
///////////////////////////////////////////////////
void Summer::channelCountChanged()
{
    BMutexLock lock( m_mutex );
    
	if ( m_accumSize < getAudioChannelCount() )
	{
		delete [] m_accum;
		m_accum = new float[getAudioChannelCount()];
		memset(m_accum, 0, sizeof(float)*getAudioChannelCount());
	}
	
	m_accumSize = getAudioChannelCount();
	
	for( int i = 0; i < m_inputsLength; ++i )
	{
		if ( m_inputs[i] )
		{
			m_inputs[i]->setAudioChannelCount( m_accumSize );
		}
	}
}
	
///////////////////////////////////////////////
void Summer::addInput( UGen * in )
{
    in->setSampleRate( sampleRate() );
    in->setAudioChannelCount( m_accumSize );
    
	// find a free slot
	for( int i = 0; i < m_inputsLength; ++i )
	{
		if ( m_inputs[i] == NULL )
		{
			m_inputs[i] = in;
			return;
		}
	}
    
    // lock if we have to mess with the list
    BMutexLock lock( m_mutex );

	// didn't find a slot, double the size of our list
	const int newLength = m_inputsLength*2;
	UGen** newList		= new UGen*[newLength];
	// initialize
	memset( newList, 0, sizeof(UGen*)*newLength );
	// copy old list to new list
	memcpy(newList, m_inputs, sizeof(UGen*)*m_inputsLength);
	// add new input to end of the list
	newList[m_inputsLength] = in;
	// delete old list
	delete [] m_inputs;
	// use new list
	m_inputsLength = newLength;
	m_inputs	   = newList;	
}

///////////////////////////////////////////////
void Summer::removeInput( UGen * in )
{    
    // simply nulls reference if pointers match
    // this is so that UGens can be removed from the list
    // while the Summer is ticking them. An example of 
    // this happening is ADSR unpatching itself.
    
	for( int i = 0; i < m_inputsLength; ++i )
	{
		if ( m_inputs[i] == in )
		{
			m_inputs[i] = NULL;
			return;
		}
	}
}

///////////////////////////////////////////////////
void Summer::uGenerate(float * channels, const int numChannels)
{	
    BMutexLock lock( m_mutex );

	// start out with silence
    UGen::fill( channels, 0, numChannels );

    const float v = volume.getLastValue();
	for( int i = 0; i < m_inputsLength; ++i )
	{
		if ( m_inputs[i] )
		{
            m_inputs[i]->tick( m_accum, numChannels );
            UGen::accum(channels, m_accum, numChannels, v);
		}
	}
}

} // namespace Minim