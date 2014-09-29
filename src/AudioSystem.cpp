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
#include "ServiceProvider.h"
#include "AudioFormat.h"
#include "AudioOut.h"
#include "AudioRecordingStream.h"
#include "AudioOutput.h"
#include <stdio.h>

#ifdef WINDOWS
#include "DirectSoundServiceProvider.h"
#else
#include "TouchServiceProvider.h"
#endif

#ifndef NULL
#define NULL 0
#endif 

namespace Minim
{
	
static bool g_bDebugOn = false;
static LogFunc g_errorLog = 0;
static LogFunc g_debugLog = 0;

void setErrorLog( LogFunc func )
{
	g_errorLog = func;
}

void setDebugLog( LogFunc func )
{
	g_debugLog = func;
}
    
void error( const char * msg )
{
	if ( g_errorLog )
	{
		g_errorLog( msg );
	}
	else
	{
		printf("\nMinim Error: %s\n", msg);
	}
}
	
void debug( const char * msg )
{
    if ( g_bDebugOn )
    {
		if ( g_debugLog )
		{
			g_debugLog( msg );
		}
		else
		{
			printf("\nMinim Debug: %s\n", msg);
		}
    }
}
    
#if TARGET_OS_IPHONE
static void interruptionHandler( void* inUserData, UInt32 inState )
{
    AudioSystem* system = (AudioSystem*)inUserData;
    
    InterruptionState state = inState == kAudioSessionBeginInterruption ? InterruptionStateBegin : InterruptionStateEnd;
    system->handleInterruption( state );
}
#endif

AudioSystem::AudioSystem( const int outputBufferSize, AudioInterruptionListener interruptionListener, void* interruptUserData )
: mServiceProvider(0)
, mInterruptionListener(interruptionListener)
, mInterruptUserData(interruptUserData)
{
#ifdef WINDOWS
	mServiceProvider = new Minim::DirectSoundServiceProvider();
#elif TARGET_OS_IPHONE
	TouchServiceProvider::AudioSessionParameters sessionParams( (float)outputBufferSize / 44100.f );
    sessionParams.interruptListener = &interruptionHandler;
    sessionParams.interruptUserData = this;
	mServiceProvider = new TouchServiceProvider(sessionParams);
#else
	mServiceProvider = new TouchServiceProvider();
#endif
	mServiceProvider->start();
}

AudioSystem::~AudioSystem()
{
	mServiceProvider->stop();
	delete mServiceProvider;
}

void AudioSystem::debugOn() { g_bDebugOn = true; }
void AudioSystem::debugOff() { g_bDebugOn = false; }
    
////////////////////////////////////////////////////
void AudioSystem::handleInterruption(InterruptionState state)
{
    if ( mInterruptionListener )
    {
        mInterruptionListener( mInterruptUserData, state );
    }
}

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
float AudioSystem::loadFileIntoBuffer( const char * filename, MultiChannelBuffer & buffer )
{
	const int readBufferSize = 4096;
	float     sampleRate = 0;
	AudioRecordingStream * pStream = loadFileStream( filename, readBufferSize, false );
	if ( pStream )
	{
		pStream->open();
		pStream->play();
		sampleRate = pStream->getFormat().getSampleRate();
		const int channelCount = pStream->getFormat().getChannels();
		// for reading the file in, in chunks.
		MultiChannelBuffer readBuffer( channelCount, readBufferSize );
		// make sure the out buffer is the correct size and type.
		buffer.setChannelCount( channelCount );
		// how many samples to read total
		const long totalSampleCount = pStream->getSampleFrameLength();
		buffer.setBufferSize( (int)totalSampleCount );
		
		// now read in chunks.
		long totalSamplesRead = 0;
		while( totalSamplesRead < totalSampleCount )
		{
			// is the remainder smaller than our buffer?
			if ( totalSampleCount - totalSamplesRead < readBufferSize )
			{
				readBuffer.setBufferSize( (int)(totalSampleCount - totalSamplesRead) );
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
    else if ( filename && strlen(filename) > 0 )
    {
        printf("Unable to load an AudioRecordingStream for %s.\n", filename);
    }

	return sampleRate;
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