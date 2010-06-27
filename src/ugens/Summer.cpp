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
	
	// static int kUGensDefaultSize = 10;

Summer::Summer()
: mOutput(NULL)
, m_accum(NULL)
, m_accumSize(0)
, head(NULL)
{
}

Summer::Summer(AudioOutput * out)
: mOutput(out)
, head(NULL)
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
	Node* n = head;
	while ( n )
	{
		n->ugen->setSampleRate(sampleRate());
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
	
	///////////////////////////////////////////////
	void Summer::addInput( UGen * in )
	{
		Node * newNode = new Node(in);
		
		if ( head == NULL )
		{
			head = newNode;
		}
		else 
		{
			// insert at end of list
			Node * n = head;
			
			while ( n->next ) 
			{
				n = n->next;
			}
			
			n->next = newNode;
		}
	}
	
	///////////////////////////////////////////////
	void Summer::removeInput( UGen * in )
	{
		if ( head == NULL )
		{
			return;
		}
		
		// special case first element
		if ( head->ugen == in )
		{
			Node* del = head;
			head = head->next;
			delete del;
			return;
		}
		
		// find it in our list and remove it!
		Node* n = head;
		
		while( n->next != NULL )
		{
			if ( n->next->ugen == in )
			{
				Node* del = n->next;
				n->next = n->next->next;
				delete del;
				return;
			}
			n = n->next;
		}
	}

///////////////////////////////////////////////////
void Summer::uGenerate(float * channels, int numChannels)
{		
	if ( head == NULL )
	{
		memset(channels, 0, sizeof(float) * numChannels);
		return;
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
	
	// first one in the list, just do an = instead of +=
	// so that people that call this method don't have to
	// worry about what's in channels when we get it
	// memset(m_accum, 0, sizeof(float) * numChannels);
	head->ugen->tick( m_accum, numChannels );
	for(int c = 0; c < numChannels; ++c)
	{
		// the ugen may have been ticked already
		// in which case tick will not put anything in m_accum
		// so we always need to get the data from last values
		channels[c] = head->ugen->getLastValues()[c];
	}
	
	// now do the rest
	
	Node* n = head->next;
	
	while( n )
	{
		// memset(m_accum, 0, sizeof(float) * numChannels);
		n->ugen->tick( m_accum, numChannels );
		for(int c = 0; c < numChannels; ++c)
		{
			channels[c] += n->ugen->getLastValues()[c];
		}
		n = n->next;
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