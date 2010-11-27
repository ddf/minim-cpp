/*
 *  FilePlayer.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 11/25/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "FilePlayer.h"
#include <string.h> // for memset

namespace Minim
{
	
	FilePlayer::FilePlayer( AudioRecordingStream * pReadStream )
	: UGen()
	, m_pStream( pReadStream )
	, m_buffer( pReadStream->getFormat().getChannels(), pReadStream->bufferSize() )
	, m_outputPosition( pReadStream->bufferSize() )
	{
		m_pStream->open();
	}
	
	FilePlayer::~FilePlayer()
	{
		m_pStream->close();
		delete m_pStream;
	}
	
	void FilePlayer::uGenerate( float * channels, const int numberOfChannels )
	{
		if ( m_pStream->isPlaying() )
		{
			// we read through our local buffer, get another one
			if ( m_outputPosition == m_pStream->bufferSize() )
			{
				m_pStream->read( m_buffer );
				m_outputPosition = 0;
			}
			
			for(int i = 0; i < numberOfChannels; ++i)
			{
				channels[i] = m_buffer.getChannel(i)[m_outputPosition];
			}
			++m_outputPosition;
		}
		else 
		{
			memset(channels, 0, sizeof(float)*numberOfChannels);
		}

	}
	
}