/*
 *  TouchAudioRecordingStream.mm
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 11/25/10.
 *  Copyright 2010 Compartmental. All rights reserved.
 *
 */

#include "TouchAudioRecordingStream.h"
#include "AudioSystem.h"

////////////////////////////////////////////
TouchAudioRecordingStream::TouchAudioRecordingStream( CFURLRef fileURL, const int bufferSize )
: m_fileURL(fileURL)
, m_audioFileRef(NULL)
, m_readBuffer(NULL)
, m_bufferSize(bufferSize)
, m_bIsPlaying(false)
, m_fileFormat(0) // don't know how many channels yet
{
}

////////////////////////////////////////////
TouchAudioRecordingStream::~TouchAudioRecordingStream()
{
	CFRelease( m_fileURL );
	
	// just in case they forgot to close this
	if ( m_audioFileRef )
	{
		ExtAudioFileDispose( m_audioFileRef );
		m_audioFileRef = NULL;
	}
	
	if ( m_readBuffer )
	{
		delete [] m_readBuffer;
	}
}

////////////////////////////////////////////
void TouchAudioRecordingStream::open()
{
	if ( !m_audioFileRef )
	{
		ExtAudioFileOpenURL(m_fileURL, &m_audioFileRef);
		
		// get the source data format, so we can read at the proper sample rate
		CAStreamBasicDescription fileFormat;
		UInt32 size = sizeof(fileFormat);
		ExtAudioFileGetProperty( m_audioFileRef, kExtAudioFileProperty_FileDataFormat, &size, &fileFormat );
		
		m_fileFormat.SetWithDesc( fileFormat );
		
		// tell the file the format we expect from reads
		m_clientFormat.mSampleRate = fileFormat.mSampleRate;
		m_clientFormat.SetCanonical( fileFormat.NumberChannels(), true );
		ExtAudioFileSetProperty( m_audioFileRef, kExtAudioFileProperty_ClientDataFormat, size, &m_clientFormat );
		
		// allocate the buffer we'll use for reading
		m_readBuffer = new SInt16[ m_bufferSize * fileFormat.NumberChannels() ];
	}
}

////////////////////////////////////////////
void TouchAudioRecordingStream::close()
{
	if ( m_audioFileRef )
	{
		ExtAudioFileDispose( m_audioFileRef );
		m_audioFileRef = NULL;
	}
	
	if ( m_readBuffer )
	{
		delete [] m_readBuffer;
		m_readBuffer = NULL;
	}
}

////////////////////////////////////////////
void TouchAudioRecordingStream::read( Minim::MultiChannelBuffer & buffer )
{
	if ( m_audioFileRef )
	{
		UInt32 outBufferSize = buffer.getBufferSize();
		const UInt32 numberChannels = m_clientFormat.NumberChannels();
		buffer.setChannelCount( numberChannels );
		
		if ( m_bIsPlaying )
		{
			AudioBufferList fillBufList;
			fillBufList.mNumberBuffers = 1;
			fillBufList.mBuffers[0].mNumberChannels = numberChannels;
			fillBufList.mBuffers[0].mDataByteSize = m_clientFormat.FramesToBytes( outBufferSize );
			fillBufList.mBuffers[0].mData = m_readBuffer;
			
			// client format is always linear PCM - so here we determine how many frames of lpcm
			// we can read/write given our buffer size
			ExtAudioFileRead( m_audioFileRef, &outBufferSize, &fillBufList );
			
			
			// and now we should be able to de-interleave our read buffer into buffer
			for(UInt32 i = 0; i < outBufferSize; ++i)
			{
				for(UInt32 c = 0; c < numberChannels; ++c)
				{
					const UInt32 offset = (i * numberChannels) + c; 
					const SInt16 sample = m_readBuffer[offset];
					buffer.getChannel(c)[i] = (float)sample / 32767.f;
				}
			}
		}
		else 
		{
			buffer.makeSilence();
		}
	}
	else 
	{
		Minim::error("Tried to read from an unopened stream!");
	}

}





