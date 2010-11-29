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
	close();
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
		
		SInt64 fileFrameLength;
		UInt32 propSize = sizeof(fileFrameLength);
		ExtAudioFileGetProperty(m_audioFileRef, kExtAudioFileProperty_FileLengthFrames, &propSize, &fileFrameLength);
		m_fileMillisLength = (UInt32)( (Float64)fileFrameLength / m_fileFormat.getFrameRate() * 1000.f );
		
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
	
	m_fileMillisLength = 0;
}

////////////////////////////////////////////
void TouchAudioRecordingStream::read( Minim::MultiChannelBuffer & buffer )
{
	if ( m_audioFileRef )
	{
		const UInt32 outBufferSize = buffer.getBufferSize();
		assert(outBufferSize <= m_bufferSize && "You requested to read more sample frames than this recording stream can read at one time.");
			   
		UInt32 samplesToRead = outBufferSize;
		const UInt32 numberChannels = m_clientFormat.NumberChannels();
		buffer.setChannelCount( numberChannels );
		// start with silence, we may not be playing, in which case we should return silence
		// or we might read fewer samples than the full buffer, in which case the rest of it
		// should be filled with silence.
		buffer.makeSilence();
		
		if ( m_bIsPlaying )
		{
			AudioBufferList fillBufList;
			fillBufList.mNumberBuffers = 1;
			fillBufList.mBuffers[0].mNumberChannels = numberChannels;
			fillBufList.mBuffers[0].mDataByteSize = m_clientFormat.FramesToBytes( outBufferSize );
			fillBufList.mBuffers[0].mData = m_readBuffer;
			
			// client format is always linear PCM - so here we determine how many frames of lpcm
			// we can read/write given our buffer size
			ExtAudioFileRead( m_audioFileRef, &samplesToRead, &fillBufList );
			
			// 0 read means EOF
			if ( samplesToRead == 0 )
			{
				// might need to wrap back to beginning loop point if looping,
				// but for now we just stop playing.
				m_bIsPlaying = false;
				return;
			}
			
			// and now we should be able to de-interleave our read buffer into buffer
			for(UInt32 c = 0; c < numberChannels; ++c)
			{
				float * channel = buffer.getChannel(c);
				for(UInt32 i = 0; i < samplesToRead; ++i)
				{
					const UInt32 offset = (i * numberChannels) + c; 
					const SInt16 sample = m_readBuffer[offset];
					channel[i] = (float)sample / 32767.f;
				}
			}
		}
	}
	else 
	{
		Minim::error("Tried to read from an unopened stream!");
	}

}

//////////////////////////////////////////////////////
unsigned int TouchAudioRecordingStream::getMillisecondPosition() const
{
	unsigned int position(0);
	if ( m_audioFileRef )
	{
		SInt64 currentFramePosition;
		ExtAudioFileTell(m_audioFileRef, &currentFramePosition);
		position = (unsigned int)( (Float64)currentFramePosition / m_fileFormat.getFrameRate() * 1000.f );
	}
		
	return position;
}

///////////////////////////////////////////////////////
void TouchAudioRecordingStream::setMillisecondPosition( const unsigned int pos )
{
	if ( m_audioFileRef )
	{
		// convert this millisecond position to a frame position
		const float seconds = (float)pos / 1000.f;
		const SInt64 closestFrame = (SInt64)roundf( seconds * m_fileFormat.getFrameRate() );
		ExtAudioFileSeek(m_audioFileRef, closestFrame);
	}
}

