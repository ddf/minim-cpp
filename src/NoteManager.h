/*
 *  NoteManager.h
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/19/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef NOTEMANAGER_H
#define	NOTEMANAGER_H

#include "Instrument.h"

#include <map>
#include <vector>

namespace Minim 
{
	class AudioOutput;

	class NoteManager
	{
	public:

		NoteManager(AudioOutput & parent);
		~NoteManager();
		
		// events are always specified as happening some period of time from now.
		// but we store them as taking place at a specific time, rather than a relative time.
		void addEvent(const float startTime, const float duration, Instrument & instrument);

		
		inline void setTempo(const float tempo)
		{
			m_tempo = tempo;
		}
		
		inline void setNoteOffset(const float noteOffset)
		{
			m_noteOffset = noteOffset;
		}
		
		inline void setDurationFactor(const float durationFactor)
		{
			m_durationFactor = durationFactor;
		}
		
		inline void pause()
		{
			m_paused = true;
		}
		
		inline void resume()
		{
			m_paused = false;
		}
		
		void tick();
		
	private:
		
		class NoteEvent
		{
		public:
			enum Type
			{
				NOTE_ON,
				NOTE_OFF
			};
			
			NoteEvent( Instrument * instrument, Type type, float duration )
			: mInstrument(instrument)
			, mType(type)
			, mDuration(duration)
			{}
			
			void send();
			
		protected:
			Instrument * mInstrument;
			Type		 mType;
			
			// used for NOTE_ON
			float		 mDuration;
		};
		
		AudioOutput & m_out;
		
		float m_tempo;
		float m_noteOffset;
		float m_durationFactor;
		int   m_now;
		
		
		// our events are stored in a map.
		// the keys in this map are the "now" that the events should
		// occur at and the values are a list of events that occur
		// at that time.
		typedef std::vector< NoteEvent > TNoteEventList;
		typedef std::map< int, TNoteEventList > TNoteEventMap;
		TNoteEventMap m_events;
		
		// are we paused?
		// pausing is important because if we're going to queue up 
		// a large number of notes, we want to make sure their timestamps
		// are accurate. this won't be possible if the note manager
		// is sending events because of ticks from the audio output.
		bool m_paused;
		
		bool m_busy;
	};

}

#endif // NOTEMANAGER_H