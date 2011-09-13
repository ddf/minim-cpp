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
	, m_outputPosition( 0 )
	{
		m_pStream->open();
		// prep
		m_pStream->play();
		fillBuffer();
		m_pStream->pause();
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
				fillBuffer();
			}
			
			for(int i = 0; i < numberOfChannels; ++i)
			{
				int chan = (i < m_buffer.getChannelCount() ? i : m_buffer.getChannelCount()-1);
				channels[i] = m_buffer.getChannel(chan)[m_outputPosition];
			}
			++m_outputPosition;
		}
		else 
		{
			memset(channels, 0, sizeof(float)*numberOfChannels);
		}

	}
	
	void FilePlayer::fillBuffer()
	{
		m_pStream->read( m_buffer );
		m_outputPosition = 0;
	}
	
	unsigned int FilePlayer::getMillisecondPosition() const
	{
		// the stream will always be ahead of where *we* are, since we read it a buffer at a time
		// but may be anywhere inside of that buffer when queried. because of this, we calculate 
		// the actual position based on where the streams says we are, less how many frame we 
		// have to go until we read from the stream again.
		const unsigned int streamPos = m_pStream->getMillisecondPosition();
		// how many milliseconds do we have left in our buffer?
		const unsigned int millisLeft = framesToMillis(m_buffer.getBufferSize() - m_outputPosition);
		// subtract that from where the stream says it is.
		return streamPos - millisLeft;
	}
	
	void FilePlayer::setMillisecondPosition( const unsigned int pos )
	{
		// before we go moving the stream, see if this position is actually in our current buffer.
		const unsigned int streamPos = m_pStream->getMillisecondPosition();
		
		if ( pos == streamPos )
		{
			// just jump there, no further calc required.
			fillBuffer();
			return;
		}
		
		// this is where are buffer starts relative to the beginning of the file
		const unsigned int bufferStartPos = streamPos - framesToMillis(m_buffer.getBufferSize());
		
		if ( pos == bufferStartPos )
		{
			// just move back our output position
			m_outputPosition = 0;
			return;
		}
		
		const unsigned int millisLeft = framesToMillis(m_buffer.getBufferSize() - m_outputPosition);
		// our current position is also relative to the beginning of the file
		const unsigned int ourPosition = streamPos - millisLeft;
		
		// its ahead of us but in our buffer
		if ( pos > ourPosition && pos < streamPos )
		{
			// covert back to sample frames
			const unsigned int sampleOffset = millisToFrames(pos - ourPosition);
			m_outputPosition += sampleOffset;
			return;
		}
		
		// its behind us but in our buffer
		if ( pos < ourPosition && pos > bufferStartPos )
		{
			const unsigned int sampleOffset = millisToFrames(ourPosition - pos);
			m_outputPosition -= sampleOffset;
			return;
		}
		
		// we can't seek locally, we need to ask the stream to do it.
		m_pStream->setMillisecondPosition(pos);
		// and update our state
		fillBuffer();
	}
}