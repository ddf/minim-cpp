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

#include "AudioOutput.h"
#include "AudioStream.h"
#include "AudioFormat.h"
#include "CodeTimer.h"

namespace Minim
{

	AudioOutput::AudioOutput(Minim::AudioOut *out)
	: AudioSource(out)
	, mSummer()
	, mNoteManager( *this )
	, mSummerStream(mSummer, mNoteManager, out->getFormat(), buffer().getBufferSize())
	{
		out->setAudioStream( &mSummerStream );
		out->open();
		
		mSummer.setSampleRate( getFormat().getSampleRate() );
		mSummer.setAudioChannelCount( getFormat().getChannels() );
	}
	
	void AudioOutput::pauseNotes()
	{
		mNoteManager.pause();
	}
	
	void AudioOutput::resumeNotes()
	{
		mNoteManager.resume();
	}
	
	void AudioOutput::playNote( float startTime, float duration, Instrument & instrument )
	{
		mNoteManager.addEvent(startTime, duration, instrument);
	}
	
	AudioOutput::SummerStream::SummerStream( Summer & summer, NoteManager & noteManager, const AudioFormat & format, const int bufferSize )
		: mSummer(summer)
		, mNoteManager(noteManager)
		, mFormat(format)
		, mVolume(1.f)
		, mTargetVolume(1.f)
	{
		mTickBuffer = new float[format.getChannels()];
        mBufferMicrosecondLength = (float)bufferSize / format.getSampleRate() * 1000000;
	}

	AudioOutput::SummerStream::~SummerStream()
	{
		delete [] mTickBuffer;
	}

	void AudioOutput::SummerStream::read( MultiChannelBuffer & buffer )
	{
		// CodeTimer timer("Buffer underrun in AudioOutput: ", mBufferMicrosecondLength );
		
		const int nChannels = buffer.getChannelCount();
		const int bsize = buffer.getBufferSize();
		for(int i = 0; i < bsize; ++i)
		{
            mNoteManager.tick();
    
            // don't need to memset our tick buffer
            // because the summer will assign for the first ugen it ticks
            // and then sum after that.
            mSummer.uGenerate( mTickBuffer, nChannels );
            for(int c = 0; c < nChannels; ++c)
            {
                float v = mVolume + (mTargetVolume - mVolume)*((float)i/bsize);
                buffer.getChannel(c)[i] = mTickBuffer[c] * v;
            }
		}
		mVolume = mTargetVolume;
	}


} // namespace Minim