/*
 *  NoteManager.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/19/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "NoteManager.h"
#include "AudioOutput.h"

namespace Minim 
{	
	NoteManager::NoteManager(AudioOutput & parent)
	: m_out(parent)
	, m_tempo(60.f)
	, m_noteOffset(0.f)
	, m_durationFactor(1.0f)
	, m_now(0)
	, m_paused(false)
	, m_busy(false)
	{
	}
	
	NoteManager::~NoteManager()
	{
	}
	
	void NoteManager::NoteEvent::send()
	{
		switch (mType) 
		{
		case NOTE_ON:
			{
				mInstrument->noteOn(mDuration);
			}
			break;
		case NOTE_OFF:
			{
				mInstrument->noteOff();
			}
			break;	
		default:
			break;
		}
	}
	
	// events are always specified as happening some period of time from now.
	// but we store them as taking place at a specific time, rather than a relative time.
	void NoteManager::addEvent(const float startTime, const float duration, Instrument & instrument)
	{
//		while ( m_busy );
//		
//		m_busy = true;
		
		int onAt = m_now + (int)(m_out.sampleRate() * ( startTime + m_noteOffset ) * 60.f / m_tempo);
		
		m_events[onAt].push_back( NoteEvent(&instrument, NoteEvent::NOTE_ON, duration) );
		
		int offAt = onAt + (int)(m_out.sampleRate() * duration * m_durationFactor * 60.f / m_tempo);
		
		m_events[offAt].push_back( NoteEvent(&instrument, NoteEvent::NOTE_OFF, 0.f) );
		
//		m_busy = false;
	}
	
	void NoteManager::tick()
	{
		if ( m_paused == false )
		{
//			while ( m_busy );
//			
//			m_busy = true;
			
			// find the events we should trigger now.
			TNoteEventMap::iterator eventsIter = m_events.find(m_now);
			if ( eventsIter != m_events.end() )
			{
				TNoteEventList & eventsToSend = eventsIter->second;
				for(int i = 0; i < eventsToSend.size(); ++i)
				{
					eventsToSend[i].send();
				}
				eventsToSend.clear();
				// remove this list because we've sent all the events
				m_events.erase( eventsIter );
			}
			
			// increment our now
			++m_now;
			
//			m_busy = false;
		}
	}
}
