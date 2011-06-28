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

#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include "AudioSource.h"
#include "Summer.h"
#include "AudioStream.h"
#include "NoteManager.h"


namespace Minim
{
	class UGen;
	
	class AudioOutput : public AudioSource
	{
	public:
		AudioOutput( AudioOut * out );
		
		void pauseNotes();
		void resumeNotes();
		
		void playNote(const float startTime, const float duration, Instrument & instrument);
		
		inline void setVolume( const float volume ) { mSummerStream.setVolume(volume); }
		inline float getVolume() const { return mSummerStream.getVolume(); }

	private:
		// UGen is our friend so that it can get to our summer
		friend class Minim::UGen;
		Summer mSummer;
		

		// an adapter class that will let us plug the Summer UGen into an AudioOut
		class SummerStream : public AudioStream
		{
		public:
			
			SummerStream( Summer & summer, NoteManager & noteManager, const AudioFormat & format );
			~SummerStream();

			// AudioResource impl
			virtual void open() {}
			virtual void close() {}
			virtual const AudioFormat & getFormat() const { return mFormat; }

			virtual void read( MultiChannelBuffer & buffer );
			
			void setVolume( const float volume ) { mTargetVolume = volume; }
			float getVolume() const { return mTargetVolume; }
			
		private:
			Summer & mSummer;
			const AudioFormat & mFormat;
			float * mTickBuffer;
			NoteManager & mNoteManager;
			// scalar applied to samples before being written into the output buffer
			float mVolume; // actual volume
			float mTargetVolume; // where we are headed
			
		} mSummerStream;
		
		NoteManager mNoteManager;

	};
};

#endif // AUDIOOUTPUT_H