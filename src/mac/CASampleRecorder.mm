/*
 *  CASampleRecorder.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/24/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CASampleRecorder.h"

CASampleRecorder::CASampleRecorder( NSString * pathToFile, const Minim::AudioFormat & sourceFormat, const int bufferSize )
: m_filePath( pathToFile )
, m_audioFile( NULL )
, m_bRecording( false )
, m_bufferSize( bufferSize )
, m_writeBuffer( NULL )
{
	[m_filePath retain];
	m_writeBuffer = new SInt16[ bufferSize * sourceFormat.getChannels() ];
	
	m_sourceFormat.SetCanonical( sourceFormat.getChannels(), true );
	m_sourceFormat.mSampleRate = sourceFormat.getSampleRate();
	
}

CASampleRecorder::~CASampleRecorder()
{
	[m_filePath release];
	ExtAudioFileDispose( m_audioFile );
	delete[] m_writeBuffer;
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
	ExtAudioFileDispose( m_audioFile );
	m_audioFile = NULL;
}

void CASampleRecorder::samples( const Minim::MultiChannelBuffer & buffer )
{
	if ( !m_bRecording || buffer.getBufferSize() != m_bufferSize )
	{
		return;
	}
	
	// interleave buffer into our write samples buffer
	const int numChan = m_sourceFormat.NumberChannels();
	for( int c = 0; c < numChan; ++c )
	{
		for( int s = 0; s < m_bufferSize; ++s )
		{
			m_writeBuffer[ s*numChan + c ] = buffer.getChannel(c)[s] * 32767.f;
		}
	}
	
	AudioBufferList fillBufList;
	fillBufList.mNumberBuffers = 1;
	fillBufList.mBuffers[0].mNumberChannels = numChan;
	fillBufList.mBuffers[0].mDataByteSize = m_sourceFormat.FramesToBytes( m_bufferSize );
	fillBufList.mBuffers[0].mData = m_writeBuffer;
	UInt32 frames = m_bufferSize;
	
	// printf("Writing %u frames of %d channel audio...\n", frames, numChan);
	OSStatus error = ExtAudioFileWriteAsync(m_audioFile, frames, &fillBufList);
	if ( error )
	{
		extern void displayErrorAndExit( NSString* message, OSStatus status );
		displayErrorAndExit( @"Error writing to the file asynchronously!", error );
	}
}
