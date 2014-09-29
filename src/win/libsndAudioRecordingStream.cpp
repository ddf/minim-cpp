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

#include "libsndAudioRecordingStream.h"
#include "AudioSystem.h"
#include <sstream>

libsndAudioRecordingStream::libsndAudioRecordingStream( const char * filePath, const int bufferSize )
: m_metaData( this )
, m_filePath( filePath )
, m_bufferSize( bufferSize )
, m_sndFile(NULL)
, m_readBuffer(NULL)
, m_readBufferLength(0)
, m_bPlaying(false)
, m_millisLength(0)
, m_millisPosition(0)
{

}

libsndAudioRecordingStream::~libsndAudioRecordingStream()
{
	// make sure we are closed, this releases all resources
	close();
}

void libsndAudioRecordingStream::SFInfoFormat::setFromSFInfo( const SF_INFO & fileInfo )
{
	mChannels = fileInfo.channels;
	mFrameRate = (float)fileInfo.samplerate;
	mSampleRate = (float)fileInfo.samplerate;
	mFrameSize = mChannels*sizeof(float);
	mSampleSizeInBits = sizeof(float);
	mBigEndian = true;
}

void libsndAudioRecordingStream::open()
{
	if ( !m_sndFile )
	{
		std::stringstream msg;
		msg << "attempting to open " << m_filePath << " with libsndfile";
		Minim::debug( msg.str().c_str() );

		m_fileInfo.format = 0;
		m_sndFile = sf_open( m_filePath.c_str(), SFM_READ, &m_fileInfo );

		if ( m_sndFile )
		{
			SF_FORMAT_INFO finfo;
			finfo.format = m_fileInfo.format;
			finfo.name   = "";
			finfo.extension = "";

			int result = sf_command( NULL, SFC_GET_FORMAT_INFO, &finfo, sizeof(finfo) );

			if ( result )
			{
				std::stringstream error;
				error << "libsndfile opened " << m_filePath << " but file format appears invalid: " << finfo.format;
				Minim::error( error.str().c_str() );
			}

			msg.str("");
			msg << m_filePath << " opened with format=" << finfo.name
				<< " samplerate=" << m_fileInfo.samplerate
				<< " channels=" << m_fileInfo.channels
				<< " frames=" << m_fileInfo.frames;
			Minim::debug( msg.str().c_str() );

			m_audioFormat.setFromSFInfo( m_fileInfo );

			// now we can make our read buffer
			m_readBufferLength = m_bufferSize * m_fileInfo.channels;
			m_readBuffer = new float[ m_readBufferLength ];
			memset( m_readBuffer, 0, m_readBufferLength*sizeof(float) );

			m_millisLength = (int)((float)m_fileInfo.frames / (float)m_fileInfo.samplerate * 1000);
			m_millisPosition = 0;
		}
		else
		{
			std::stringstream error;
			error << "libsndfile failed to open " << m_filePath << ": " << sf_strerror(NULL);
			Minim::error( error.str().c_str() );
		}
	}
}

void libsndAudioRecordingStream::close()
{
	if ( m_sndFile )
	{
		std::stringstream msg;
		msg << "Closing libsndAudioRecordingStream for " << m_filePath;
		Minim::debug( msg.str().c_str() );

		sf_close( m_sndFile );
		m_sndFile = NULL;

		delete [] m_readBuffer;
		m_readBuffer = NULL;
		m_readBufferLength = 0;
	}
}

long libsndAudioRecordingStream::getSampleFrameLength() const
{
	if ( m_sndFile )
	{
		return (long)m_fileInfo.frames;
	}

	return 0;
}

void libsndAudioRecordingStream::play()
{
	m_bPlaying = true;
}

void libsndAudioRecordingStream::pause()
{
	m_bPlaying = false;
}

void libsndAudioRecordingStream::setMillisecondPosition( const unsigned int pos )
{
	if ( m_sndFile )
	{
		sf_count_t framePosition = (sf_count_t)((float)pos/1000 * 44100);
		if ( framePosition > m_fileInfo.frames )
		{
			framePosition = m_fileInfo.frames;
		}
		framePosition = sf_seek( m_sndFile, framePosition, SEEK_SET );
		if ( framePosition != -1 )
		{
			m_millisPosition = (int)((float)framePosition / m_fileInfo.samplerate * 1000);
		}
	}
}

unsigned long libsndAudioRecordingStream::getSampleFramePosition() const 
{
	if ( m_sndFile )
	{
		sf_count_t pos = sf_seek( m_sndFile, 0, SEEK_CUR );
		if ( pos == -1 )
		{
			return 0;
		}
		return (unsigned long)pos;
	}

	return 0;
}

void libsndAudioRecordingStream::read( Minim::MultiChannelBuffer & buffer )
{
	if ( m_sndFile )
	{
		const unsigned int outBufferSize = buffer.getBufferSize();
		//assert(outBufferSize <= m_bufferSize && "You requested to read more sample frames than this recording stream can read at one time.");
			   
		sf_count_t samplesToRead	= outBufferSize;
		sf_count_t totalSamplesRead	= 0;
		
		const int numberChannels = m_fileInfo.channels;
		buffer.setChannelCount( numberChannels );
		// start with silence, we may not be playing, in which case we should return silence
		// or we might read fewer samples than the full buffer, in which case the rest of it
		// should be filled with silence.
		buffer.makeSilence();
		
		if ( m_bPlaying )
		{
			while( samplesToRead > 0 )
			{
				sf_count_t readSize = samplesToRead < m_bufferSize ? samplesToRead : m_bufferSize;
				
				// heyo, we can just read floats and libsndfile converts for us!
				sf_count_t samplesRead = sf_readf_float( m_sndFile, m_readBuffer, readSize );
				
				// 0 read means EOF
				if ( samplesRead == 0 )
				{
					// might need to wrap back to beginning loop point if looping,
					// but for now we just stop playing.
					m_bPlaying = false;
					return;
				}
				
				// and now we should be able to de-interleave our read buffer into buffer
				for(int c = 0; c < numberChannels; ++c)
				{
					float * channel = buffer.getChannel(c);
					for(int i = 0; i < samplesRead; ++i)
					{
						const int offset = (i * numberChannels) + c; 
						const float sample = m_readBuffer[offset];
						channel[totalSamplesRead + i ] = sample;
					}
				}
				
				samplesToRead	 -= samplesRead;
				totalSamplesRead += samplesRead;
			}

			m_millisPosition += (int)((float)totalSamplesRead / m_fileInfo.samplerate * 1000);
		}
	}
	else 
	{
		Minim::error("Tried to read from an unopened stream!");
	}
}
