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

#include "AudioSource.h"
#include "AudioOut.h"
#include <string> // for memset

namespace Minim
{

	AudioSource::AudioSource( AudioOut * out )
	: mOutput(out)
	, mSampleBuffer( out->getOutputBuffer() )
	, mListener( *this )
	{
		out->setAudioListener( &mListener );
		
		memset( mListenerList, 0, sizeof(AudioListener*) * kMaxListeners );
	}

	AudioSource::~AudioSource()
	{
		delete mOutput;
	}

	float AudioSource::sampleRate() const
	{
		return mOutput->getFormat().getSampleRate();
	}

	void AudioSource::close()
	{
		mOutput->close();
	}
	
	void AudioSource::addListener( AudioListener * pListener )
	{
		for( int i = 0; i < kMaxListeners; ++i )
		{
			if ( mListenerList[i] == NULL )
			{
				mListenerList[i] = pListener;
				return;
			}
		}
	}
	
	void AudioSource::removeListener( AudioListener * pListener )
	{
		for( int i = 0; i < kMaxListeners; ++i )
		{
			if ( mListenerList[i] == pListener )
			{
				mListenerList[i] = NULL;
			}
		}
	}


	void AudioSource::OutputListener::samples( const MultiChannelBuffer & buffer )
	{
		for( int i = 0; i < kMaxListeners; ++i )
		{
			if ( mOwner.mListenerList[i] != NULL )
			{
				mOwner.mListenerList[i]->samples( buffer );
			}
		}
	}
} // namespace Minim