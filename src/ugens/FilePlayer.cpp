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
#include <stdio.h>

Minim::FilePlayer::FilePlayer( AudioRecordingStream * pReadStream )
: UGen()
, amplitude( *this, CONTROL, 1.0f )
, rate( *this, CONTROL, 1.0f )
, m_pStream( pReadStream )
, m_buffer( pReadStream->getFormat().getChannels(), pReadStream->bufferSize() )
, m_streamFrameLength( pReadStream->getSampleFrameLength() )
, m_outputPosition( 0 )
{
    m_pStream->open();
    // prep
    m_pStream->play();
    fillBuffer();
    m_pStream->pause();
}

Minim::FilePlayer::~FilePlayer()
{
    m_pStream->close();
    delete m_pStream;
}
	
void Minim::FilePlayer::uGenerate( float * channels, const int numberOfChannels )
{
    BMutexLock lock( m_mutex );
    
    if ( m_pStream->isPlaying() )
    {
        // we read through our local buffer, get another one
        if ( m_outputPosition >= m_pStream->bufferSize() )
        {
            fillBuffer();
            m_outputPosition -= m_pStream->bufferSize();
        }
        else if ( m_outputPosition < 0 )
        {
            int targetMillis = m_pStream->getMillisecondPosition() - framesToMillis(m_pStream->bufferSize()*2);
            if ( targetMillis < 0 )
            {
                targetMillis += m_pStream->getMillisecondLength();
            }
            m_pStream->setMillisecondPosition(targetMillis);
            fillBuffer();
            m_outputPosition += m_pStream->bufferSize();
        }
        
        switch ( numberOfChannels ) 
        {
            case 1:
            {
                channels[0] = m_buffer.getSample(0, m_outputPosition) * amplitude.getLastValue();
            }
				break;
                
            case 2:
            {
                channels[0] = m_buffer.getSample(0, m_outputPosition) * amplitude.getLastValue();
                if ( m_buffer.getChannelCount() == 2 )
                {
                    channels[1] = m_buffer.getSample(1, m_outputPosition) * amplitude.getLastValue();
                }
                else 
                {
                    channels[1] = channels[0];
                }
                
            }
				break;
                
            default:
                break;
        }
        
        m_outputPosition += rate.getLastValue();
    }
    else 
    {
        memset(channels, 0, sizeof(float)*numberOfChannels);
    }

}

void Minim::FilePlayer::fillBuffer()
{
    m_pStream->read( m_buffer );
}

void Minim::FilePlayer::copyBufferChannel( float * outSamples, const int channel )
{
    BMutexLock lock( m_mutex );
    
    memcpy(outSamples, m_buffer.getChannel(channel), sizeof(float)*m_buffer.getBufferSize());
}

unsigned int Minim::FilePlayer::getMillisecondPosition() const
{
    // the stream will always be ahead of where *we* are, since we read it a buffer at a time
    // but may be anywhere inside of that buffer when queried. because of this, we calculate 
    // the actual position based on where the streams says we are, less how many frame we 
    // have to go until we read from the stream again.
    const unsigned int streamPos = m_pStream->getMillisecondPosition();
    // how many milliseconds do we have left in our buffer?
    unsigned int millisLeft = framesToMillis(m_buffer.getBufferSize() - m_outputPosition);
    // we might have more time left in our buffer than the stream is reporting as its
    // position, which means that it looped and we haven't got past the loop point in the buffer yet.
    // so we need to make sure we don't generate a negative number
    if ( streamPos < millisLeft )
    {
        // so we disregard the number of milliseconds that correspond to the wrapped audio
        millisLeft -= streamPos;
        // and then calculate our offset from the end of the file (since we don't currently allow loop points)
        return m_pStream->getMillisecondLength() - millisLeft;
    }
    
    // subtract that from where the stream says it is.
    return streamPos - millisLeft;
}

void Minim::FilePlayer::setMillisecondPosition( const unsigned int pos )
{
    BMutexLock lock( m_mutex );
    
    // before we go moving the stream, see if this position is actually in our current buffer.
    const unsigned int streamPos = m_pStream->getMillisecondPosition();
    
    if ( pos == streamPos )
    {
        // just jump there, no further calc required.
        fillBuffer();
        m_outputPosition = 0;
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
    m_outputPosition = 0;
}