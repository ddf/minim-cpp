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
#include <cassert>

#ifndef NULL
#define NULL 0
#endif

struct Lock
{
    Lock( bool& _bLock )
    : bLock(_bLock)
    {
        bLock = true;
    }
    
    ~Lock()
    {
        bLock = false;
    }
    
    bool& bLock;  
};

#define LOCK while(m_bMutex){} Lock lock(m_bMutex);

namespace Minim
{
	
	// static int kUGensDefaultSize = 10;

Summer::Summer()
: UGen(1)
, m_accum( new float[1] ) // assume mono, same as UGen
, m_accumSize(1)
, head(NULL)
, volume( *this, CONTROL )
, m_bMutex(false)
{
	volume.setLastValue( 1 );
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
    LOCK;
    
	Node* n = head;
	while ( n )
	{
		if ( n->ugen ) n->ugen->setSampleRate(sampleRate());
		n = n->next;
	}
	
//	for (int i = 0; i < mUGensSize; i++)
//	{
//		UGen * u = mUGens[i];
//		if ( u )
//		{
//			u->setSampleRate(sampleRate());
//		}
//	}
}
	
///////////////////////////////////////////////////
void Summer::channelCountChanged()
{
    LOCK;
    
	if ( m_accumSize < getAudioChannelCount() )
	{
		delete [] m_accum;
		m_accum = new float[getAudioChannelCount()];
		memset(m_accum, 0, sizeof(float)*getAudioChannelCount());
	}
	
	m_accumSize = getAudioChannelCount();
	
	Node* n = head;
	while ( n )
	{
		n->ugen->setAudioChannelCount( getAudioChannelCount() );
		n = n->next;
	}
}
	
///////////////////////////////////////////////
void Summer::addInput( UGen * in )
{
    in->setAudioChannelCount(m_accumSize);
	in->setSampleRate( sampleRate() );
    
	Node * newNode = new Node(in);
    
    // insert at beginning of the list
    newNode->next = head;
    head = newNode;
}

///////////////////////////////////////////////
void Summer::removeInput( UGen * in )
{
    // simply nulls reference on link if pointers match
    // the actual removal of the link happens in uGenerate
    // this is so that UGens can be removed from the list
    // while the Summer is ticking them. An example of 
    // this happening is ADSR unpatching itself.
    
	if ( head == NULL )
	{
		return;
	}
	
	// special case first element
	if ( head->ugen == in )
	{
        head->ugen = NULL;
		return;
	}
	
	// find it in our list and null it.
	Node* n = head;
	
	while( n->next != NULL )
	{
		if ( n->next->ugen == in )
		{
            n->next->ugen = NULL;
			return;
		}
		n = n->next;
	}
}

///////////////////////////////////////////////////
void Summer::uGenerate(float * channels, int numChannels)
{	
    LOCK;
    
    // remove head if the ugen got unpatched
    while ( head && head->ugen == NULL )
    {
        Node* n = head;
        head = n->next;
        delete n;
    }
    
    // empty list, just fill with silence
    if ( head == NULL )
	{
        UGen::fill(channels, 0, numChannels);
		return;
	}
    
	// first one in the list, we can tick directly into channels
	head->ugen->tick( channels, numChannels );
	
	// now do the rest
	Node* n = head;
	while( n && n->next )
	{
        if ( n->next->ugen )
        {
            n->next->ugen->tick( m_accum, numChannels );
            for(int c = 0; c < numChannels; ++c)
            {
                channels[c] += m_accum[c];
            }
        }
        else // no ugen? remove the next link
        {
            Node* kill = n->next;
            n->next = n->next->next;
            delete kill;
        }
        
        n = n->next;
	}

	const float v = volume.getLastValue();
	for( int i = 0; i < numChannels; ++i )
	{
		channels[i] *= v;
	}

//	for(int i = 0; i < mUGensSize; ++i)
//	{
//		UGen * u = mUGens[i];
//		if ( u )
//		{
//			memset(m_accum, 0, sizeof(float) * numChannels);
//			u->tick( m_accum, numChannels );
//			for(int c = 0; c < numChannels; ++c)
//			{
//				channels[c] += m_accum[c];
//			}
//		}
//	}
}

} // namespace Minim