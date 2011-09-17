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

#include "DirectSoundServiceProvider.h"
#include "libsndAudioRecordingStream.h"

#ifndef NULL
#define NULL 0
#endif

namespace Minim
{

	DirectSoundServiceProvider::DirectSoundServiceProvider()
	{
	}

	DirectSoundServiceProvider::~DirectSoundServiceProvider()
	{
	}
	  
	void DirectSoundServiceProvider::start()
	{
	}

	void DirectSoundServiceProvider::stop()
	{
	}
	  
	void DirectSoundServiceProvider::debugOn()
	{
	}

	void DirectSoundServiceProvider::debugOff()
	{
	}
	  
	AudioRecordingStream * DirectSoundServiceProvider::getAudioRecordingStream( const char * filename, int bufferSize, bool inMemory )
	{
		return new libsndAudioRecordingStream( filename, bufferSize );
	}

	AudioStream * DirectSoundServiceProvider::getAudioInput( const AudioFormat & inputFormat )
	{
		return NULL;
	}

	AudioOut * DirectSoundServiceProvider::getAudioOutput( const AudioFormat & outputFormat, int outputBufferSize )
	{
		return NULL;
	}

	AudioSample * DirectSoundServiceProvider::getAudioSample( const char * filename, int bufferSize )
	{
		return NULL;
	}

	AudioSample * DirectSoundServiceProvider::getAudioSample( MultiChannelBuffer * samples, const AudioFormat & format, int bufferSize )
	{
		return NULL;
	}
}