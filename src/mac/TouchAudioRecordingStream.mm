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
, m_bIsLooping(false)
, m_loopCount(0)
, m_loopStart(0)
, m_loopStop(0)
, m_fileFormat(0) // don't know how many channels yet
, m_fileMillisLength(0)
, m_fileFrameLength(0)
, m_metaData(this)
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
		
		UInt32 propSize = sizeof(m_fileFrameLength);
		ExtAudioFileGetProperty(m_audioFileRef, kExtAudioFileProperty_FileLengthFrames, &propSize, &m_fileFrameLength);
		m_fileMillisLength = (UInt32)( (Float64)m_fileFrameLength / m_fileFormat.getFrameRate() * 1000.f );
		
		// allocate the buffer we'll use for reading
		if ( m_clientFormat.SampleWordSize() == sizeof(SInt16) )
		{
			m_readBuffer = new SInt16[ m_bufferSize * fileFormat.NumberChannels() ];
		}
		else if ( m_clientFormat.SampleWordSize() == sizeof(Float32) )
		{
			m_readBuffer = new Float32[ m_bufferSize * fileFormat.NumberChannels() ];
		}
        
        m_loopStop = m_fileFrameLength;
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
		if ( m_clientFormat.SampleWordSize() == sizeof(SInt16) )
		{
			delete [] (SInt16*)m_readBuffer;
		}
		else if ( m_clientFormat.SampleWordSize() == sizeof(Float32) )
		{
			delete [] (Float32*)m_readBuffer;
		}
		
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
		//assert(outBufferSize <= m_bufferSize && "You requested to read more sample frames than this recording stream can read at one time.");
			   
		UInt32 samplesToRead	= outBufferSize;
		UInt32 totalSamplesRead = 0;
		
		const UInt32 numberChannels = m_clientFormat.NumberChannels();
		buffer.setChannelCount( numberChannels );
		// start with silence, we may not be playing, in which case we should return silence
		// or we might read fewer samples than the full buffer, in which case the rest of it
		// should be filled with silence.
		buffer.makeSilence();
		
		if ( m_bIsPlaying )
		{
			while( samplesToRead > 0 )
			{
                if ( m_bIsLooping )
                {
                    // make sure we aren't outside of our loop points
                    SInt64 currentFramePosition;
                    ExtAudioFileTell(m_audioFileRef, &currentFramePosition);
                    if ( currentFramePosition < m_loopStart || currentFramePosition >= m_loopStop )
                    {
                        ExtAudioFileSeek(m_audioFileRef, m_loopStart);
                        currentFramePosition = m_loopStart;
                    }
                    
                    // see if the number of frames left between where we are 
                    // and the end of our loop is less than the size of our buffer
                    if ( m_loopStop - currentFramePosition < samplesToRead )
                    {
                        // only read as many samples as we actually need
                        samplesToRead = m_loopStop - currentFramePosition;
                        
                        // no more looping, just stop
                        if ( m_loopCount == 0 )
                        {
                            m_bIsLooping = false;
                            m_bIsPlaying = false;
                        }
                        else
                        {
                            if ( m_loopCount > 0 )
                            {
                                --m_loopCount;
                            }
                        }
                    }
                }
                
				UInt32 readSize = samplesToRead < m_bufferSize ? samplesToRead : m_bufferSize;
				AudioBufferList fillBufList;
				fillBufList.mNumberBuffers = 1;
				fillBufList.mBuffers[0].mNumberChannels = numberChannels;
				fillBufList.mBuffers[0].mDataByteSize = m_clientFormat.FramesToBytes( readSize );
				fillBufList.mBuffers[0].mData = m_readBuffer;
				
                // on input, ExtAudioFileRead wants to know how many frames to read.
				UInt32 samplesRead = readSize;
				OSStatus result = ExtAudioFileRead( m_audioFileRef, &samplesRead, &fillBufList );
                if ( result != noErr )
                {
                    printf("Error reading from a file, aborting read. [%d]\n", (int)result);
                    return;
                }
				
                // on output, samplesRead contains how many frames were actually read.
				// 0 read means EOF, so we should stop playing if we aren't looping.
				if ( samplesRead == 0 && !m_bIsLooping )
				{
					m_bIsPlaying = false;
					return;
				}
				
				// and now we should be able to de-interleave our read buffer into our output buffer
				const bool bIntegral = m_clientFormat.SampleWordSize() == sizeof(SInt16);
				for(UInt32 c = 0; c < numberChannels; ++c)
				{
					float * channel = buffer.getChannel(c);
                    
                    if ( bIntegral )
                    {
                        for(UInt32 i = 0; i < samplesRead; ++i)
                        {
                            const UInt32 offset = (i * numberChannels) + c; 
                            const SInt16 sample = ((SInt16*)m_readBuffer)[offset];
                            channel[totalSamplesRead + i] = (float)sample / 32767.f;
                        }
                    }
                    else
                    {
                        for(UInt32 i = 0; i < samplesRead; ++i)
                        {
                            const UInt32 offset = (i * numberChannels) + c; 
                            const Float32 sample = ((Float32*)m_readBuffer)[offset];
                            channel[totalSamplesRead + i ] = sample;
                        }   
                    }
				}
				
				totalSamplesRead += samplesRead;
                samplesToRead     = outBufferSize - totalSamplesRead;
			}
		}
	}
	else 
	{
		Minim::error("Tried to read from an unopened stream!");
	}
}

//////////////////////////////////////////////////////
void TouchAudioRecordingStream::loop( const int count )
{
	m_loopCount = count > -1 ? count : -1;
	m_bIsPlaying = true;
	m_bIsLooping = true;
}

//////////////////////////////////////////////////////
void TouchAudioRecordingStream::setLoopPoints( const unsigned int start, const unsigned int stop )
{
	// convert millisecond positions to frame positions
	m_loopStart = (float)start / 1000.f * m_fileFormat.getSampleRate();
	m_loopStop  = (float)stop / 1000.f * m_fileFormat.getSampleRate();
	// make sure our loop points are at least one buffer apart
	if ( m_loopStop - m_loopStart < m_bufferSize )
	{
		m_loopStop = m_loopStart + m_bufferSize;
		// if that extends beyond the end of the file
		// back up the start point.
		if ( m_loopStop > m_fileFrameLength )
		{
			m_loopStop = m_fileFrameLength;
			m_loopStart = m_loopStop - m_bufferSize;
		}
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

