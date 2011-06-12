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

#include "AudioSystem.h"
#include "AudioFormat.h"
#include "AudioOut.h"
#include "AudioRecordingStream.h"
#include "AudioOutput.h"
#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif 

namespace Minim
{
	
void error( const char * msg )
{
	printf("\nMinim Error: %s\n", msg);
}
	
void debug( const char * msg )
{
	printf("\nMinim Debug: %s\n", msg);
}

AudioSystem::AudioSystem(Minim::ServiceProvider *msp)
: mServiceProvider(msp)
{
	mServiceProvider->start();
}

AudioSystem::~AudioSystem()
{
	mServiceProvider->stop();
	delete mServiceProvider;
}

// TODO
void AudioSystem::debugOn() {}
void AudioSystem::debugOff() {}

////////////////////////////////////////////////////
AudioPlayer * AudioSystem::loadFile( const char * filename, const int bufferSize )
{
	AudioRecordingStream * rec = loadFileStream(filename, bufferSize, false);
	if ( rec != NULL )
	{
		const AudioFormat & format = rec->getFormat();
		AudioOut * out = mServiceProvider->getAudioOutput( format, bufferSize );
		if ( out != NULL )
		{
			// TODO
			// return new AudioPlayer(rec, out);
		}
		else
		{
			delete rec;
			delete out;
		}
	}

	// TODO
	//error("Couldn't load the file " + filename);
	return NULL;
}

//////////////////////////////////////////////////////
void AudioSystem::loadFileIntoBuffer( const char * filename, MultiChannelBuffer & buffer )
{
	const int readBufferSize = 4096;
	AudioRecordingStream * pStream = loadFileStream( filename, readBufferSize, false );
	if ( pStream )
	{
		pStream->open();
		pStream->play();
		const int channelCount = pStream->getFormat().getChannels();
		// for reading the file in, in chunks.
		MultiChannelBuffer readBuffer( channelCount, readBufferSize );
		// make sure the out buffer is the correct size and type.
		buffer.setChannelCount( channelCount );
		// how many samples to read total
		const long totalSampleCount = pStream->getSampleFrameLength();
		buffer.setBufferSize( totalSampleCount );
		
		// now read in chunks.
		long totalSamplesRead = 0;
		while( totalSamplesRead < totalSampleCount )
		{
			// is the remainder smaller than our buffer?
			if ( totalSampleCount - totalSamplesRead < readBufferSize )
			{
				readBuffer.setBufferSize( totalSampleCount - totalSamplesRead );
			}
			
			pStream->read( readBuffer );
			
			// copy data from one buffer to the other.
			for(int i = 0; i < channelCount; ++i)
			{
				memcpy(buffer.getChannel(i) + totalSamplesRead, readBuffer.getChannel(i), sizeof(float)*readBuffer.getBufferSize());
			}
			
			totalSamplesRead += readBuffer.getBufferSize();
		}
		
		pStream->close();
		delete pStream;
	}
}
	
//////////////////////////////////////////////////////
AudioInput * AudioSystem::getAudioInput( const AudioFormat & inputFormat, int outputBufferSize )
{
	AudioStream * stream = mServiceProvider->getAudioInput( inputFormat );
	if ( stream != NULL )
	{
		AudioOut * out = mServiceProvider->getAudioOutput( inputFormat, outputBufferSize );
		if ( out != NULL )
		{
			// TODO
			// return new AudioInput(stream, out);
		}
		else
		{
			delete stream;
			delete out;
		}
	}

	error("AudioSystem::getAudioInput: attempt failed, could not secure an AudioInput.");
	return NULL;
}

AudioOutput * AudioSystem::getAudioOutput( const AudioFormat & outputFormat, int outputBufferSize )
{
	AudioOut * out = mServiceProvider->getAudioOutput( outputFormat, outputBufferSize );
	if ( out != NULL )
	{
	  return new AudioOutput(out);
	}

	error("AudioSystem::getAudioOutput: attempt failed, could not secure an AudioOutput.");
	return NULL;
}

} // namespace Minim