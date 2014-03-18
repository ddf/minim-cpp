/*
 *  CASampleRecorder.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/24/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CASampleRecorder.h"

CASampleRecorder::CASampleRecorder( NSString * pathToFile, const Minim::AudioFormat & sourceFormat )
: m_filePath( pathToFile )
, m_audioFile( NULL )
, m_sampleFramesRecorded(0)
, m_bRecording( false )
{
	[m_filePath retain];
	
	m_sourceFormat.SetCanonical( sourceFormat.getChannels(), true );
	m_sourceFormat.mSampleRate = sourceFormat.getSampleRate();
	
}

CASampleRecorder::~CASampleRecorder()
{
	[m_filePath release];
	ExtAudioFileDispose( m_audioFile );
}

const char * CASampleRecorder::filePath() const
{
	return [m_filePath UTF8String];
}

void CASampleRecorder::beginRecord()
{
	if ( m_bRecording )
	{
		return;
	}
	
	// not open yet? ok, do that.
	if ( m_audioFile == NULL )
	{
		CFURLRef sourceURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)m_filePath, kCFURLPOSIXPathStyle, false);
		m_sourceFormat.Print();
		OSStatus error = ExtAudioFileCreateWithURL(sourceURL, kAudioFileWAVEType, &m_sourceFormat, NULL, kAudioFileFlags_EraseFile, &m_audioFile);
		if ( error )
		{
			extern void displayErrorAndExit( NSString* message, OSStatus status );
			displayErrorAndExit( @"Couldn't create a file to record to!", error );
			return;
		}
		
		ExtAudioFileSetProperty(m_audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(m_sourceFormat), &m_sourceFormat);
		
        m_sampleFramesRecorded = 0;
		m_bRecording = true;
	}
}

void CASampleRecorder::endRecord()
{
	m_bRecording = false;
}

bool CASampleRecorder::isRecording() const
{
	return m_bRecording;
}

void CASampleRecorder::save()
{
    UInt32 propSize = sizeof(m_sampleFramesRecorded);
    printf( "Saving recording with %ld frames recorded.\n", m_sampleFramesRecorded );
    OSStatus result = ExtAudioFileSetProperty(m_audioFile, kExtAudioFileProperty_FileLengthFrames, propSize, &m_sampleFramesRecorded);
    if ( result )
    {
        extern void displayErrorAndExit( NSString* message, OSStatus status );
        displayErrorAndExit( @"Error setting the FileLengthFrames property!", result );
    }
    
	ExtAudioFileDispose( m_audioFile );
	m_audioFile = NULL;
}

void CASampleRecorder::samples( const Minim::MultiChannelBuffer & buffer )
{
	if ( !m_bRecording )
	{
		return;
	}

    const int numChan       = m_sourceFormat.NumberChannels();
    const int bufferSize    = buffer.getBufferSize();
    AudioSampleType writeSamples[ bufferSize * numChan ];
    const float sampleScale = (m_sourceFormat.mFormatFlags & kAudioFormatFlagIsSignedInteger) ? 32767.0f : 1.0f;
	
	// interleave buffer into our write samples buffer

	for( int c = 0; c < numChan; ++c )
	{
		for( int s = 0; s < bufferSize; ++s )
		{
			writeSamples[ s*numChan + c ] = buffer.getChannel(c)[s] * sampleScale;
		}
	}
	
	AudioBufferList fillBufList;
	fillBufList.mNumberBuffers              = 1;
	fillBufList.mBuffers[0].mNumberChannels = numChan;
	fillBufList.mBuffers[0].mDataByteSize   = m_sourceFormat.FramesToBytes( bufferSize );
	fillBufList.mBuffers[0].mData           = writeSamples;
	UInt32 frames                           = bufferSize;
	
	// printf("Writing %u frames of %d channel audio...\n", frames, numChan);
	OSStatus error = ExtAudioFileWriteAsync(m_audioFile, frames, &fillBufList);
	if ( error )
	{
		extern void displayErrorAndExit( NSString* message, OSStatus status );
		displayErrorAndExit( @"Error writing to the file asynchronously!", error );
	}
    
    m_sampleFramesRecorded += buffer.getBufferSize();
}
