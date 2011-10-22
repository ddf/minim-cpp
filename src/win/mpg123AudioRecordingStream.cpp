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

#include "mpg123AudioRecordingStream.h"
#include "AudioSystem.h"

mpg123AudioRecordingStream::mpg123AudioRecordingStream( const char * filePath, const int bufferSize )
: m_filePath( filePath )
, m_bufferSize( bufferSize )
, m_sndFile(NULL)
, m_readBuffer(NULL)
, m_readBufferLength(0)
, m_bPlaying(false)
, m_millisLength(0)
, m_millisPosition(0)
{

}

mpg123AudioRecordingStream::~mpg123AudioRecordingStream()
{
	// make sure we are closed, this releases all resources
	close();
}

void mpg123AudioRecordingStream::mpg123Format::setFormat( mpg123_handle* fileInfo )
{
	int channels = 0, encoding = 0;
	long rate = 0;

	mpg123_getformat( fileInfo, &rate, &channels, &encoding );

	mChannels = channels;
	mFrameRate = (float)rate;
	mSampleRate = (float)rate;
	mFrameSize = mChannels*sizeof(float);
	mSampleSizeInBits = sizeof(float);
	mBigEndian = true;

	/* Ensure that this output format will not change */
	mpg123_format_none( fileInfo );
	mpg123_format( fileInfo, rate, channels, encoding);
}

void mpg123AudioRecordingStream::open()
{
	if ( !m_sndFile )
	{
		int err = MPG123_OK;
		
		m_sndFile = mpg123_new( NULL, &err );
		
		if ( m_sndFile )
		{
			// force floating point output
			mpg123_param( m_sndFile, MPG123_ADD_FLAGS, MPG123_FORCE_FLOAT, 0.0 );

			mpg123_open( m_sndFile, m_filePath.c_str() );

			m_audioFormat.setFormat( m_sndFile );

			// scan the file so we get accurate length info
			mpg123_scan( m_sndFile );

			// now we can make our read buffer
			m_readBufferLength = m_bufferSize * m_audioFormat.getChannels();
			m_readBuffer = new float[ m_readBufferLength ];
			memset( m_readBuffer, 0, m_readBufferLength*sizeof(float) );

			m_millisLength = (int)((float)getSampleFrameLength() / (float)m_audioFormat.getSampleRate() * 1000);
			m_millisPosition = 0;
		}
		else
		{
			Minim::error( mpg123_plain_strerror(err) );
		}
	}
}

void mpg123AudioRecordingStream::close()
{
	if ( m_sndFile )
	{
		mpg123_close( m_sndFile );
		mpg123_delete( m_sndFile );
		m_sndFile = NULL;

		delete [] m_readBuffer;
		m_readBuffer = NULL;
		m_readBufferLength = 0;
	}
}

long mpg123AudioRecordingStream::getSampleFrameLength() const
{
	if ( m_sndFile )
	{
		off_t len = mpg123_length( m_sndFile );
		if ( len != MPG123_ERR )
		{
			return len;
		}
	}

	return 0;
}

void mpg123AudioRecordingStream::play()
{
	m_bPlaying = true;
}

void mpg123AudioRecordingStream::pause()
{
	m_bPlaying = false;
}

void mpg123AudioRecordingStream::setMillisecondPosition( const unsigned int pos )
{
	if ( m_sndFile )
	{
		off_t framePosition = (off_t)((float)pos/1000 * m_audioFormat.getSampleRate());
		off_t frameLen = getSampleFrameLength();
		if ( framePosition > frameLen )
		{
			framePosition = frameLen;
		}
		framePosition = mpg123_seek( m_sndFile, framePosition, SEEK_SET );
		if ( framePosition != -1 )
		{
			m_millisPosition = (int)((float)framePosition / m_audioFormat.getSampleRate() * 1000);
		}
	}
}

void mpg123AudioRecordingStream::read( Minim::MultiChannelBuffer & buffer )
{
	if ( m_sndFile )
	{
		const unsigned int outBufferSize = buffer.getBufferSize();
		//assert(outBufferSize <= m_bufferSize && "You requested to read more sample frames than this recording stream can read at one time.");
			   
		int framesToRead     = outBufferSize;
		int totalFramesRead  = 0;
		
		const int numberChannels = m_audioFormat.getChannels();
		buffer.setChannelCount( numberChannels );
		// start with silence, we may not be playing, in which case we should return silence
		// or we might read fewer samples than the full buffer, in which case the rest of it
		// should be filled with silence.
		buffer.makeSilence();
		
		if ( m_bPlaying )
		{
			while( framesToRead > 0 )
			{
				size_t readSize = framesToRead < m_bufferSize ? framesToRead : m_bufferSize;
				// convert frames to bytes
				readSize *= numberChannels*sizeof(float);
				
				size_t samplesRead(0);
				int err = mpg123_read( m_sndFile, (unsigned char*)m_readBuffer, readSize, &samplesRead );
				
				// this comes back in bytes, so turn it into a useful number
				size_t framesRead = samplesRead / sizeof(float) / m_audioFormat.getChannels();
				
				if ( err == MPG123_DONE )
				{
					// might need to wrap back to beginning loop point if looping,
					// but for now we just stop playing.
					m_bPlaying = false;
					return;
				}
				else if ( err != MPG123_OK )
				{
					char msg[128];
					sprintf_s( msg, "Error reading from mp3 file: %s\n", mpg123_plain_strerror(err) );
					Minim::error( msg );
					return;
				}
				
				// and now we should be able to de-interleave our read buffer into buffer
				for(int c = 0; c < numberChannels; ++c)
				{
					float * channel = buffer.getChannel(c);
					for(size_t i = 0; i < framesRead; ++i)
					{
						const int offset = (i * numberChannels) + c; 
						const float sample = m_readBuffer[offset];
						channel[totalFramesRead + i ] = sample;
					}
				}
				
				framesToRead	 -= framesRead;
				totalFramesRead  += framesRead;
			}

			m_millisPosition += (int)((float)totalFramesRead / m_audioFormat.getSampleRate() * 1000);
		}
	}
	else 
	{
		Minim::error("Tried to read from an unopened stream!");
	}
}
