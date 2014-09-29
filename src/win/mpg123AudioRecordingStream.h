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

#ifndef MPG123AUDIORECORDINGSTREAM_H
#define MPG123AUDIORECORDINGSTREAM_H

#include "AudioRecordingStream.h"
#include "mpg123.h"

#include <string>

class mpg123AudioRecordingStream : public Minim::AudioRecordingStream
{
public:
	mpg123AudioRecordingStream( const char * filePath, const int bufferSize );
	virtual ~mpg123AudioRecordingStream();

	// AudioResource implementation
	virtual void open();
	virtual void close();
	virtual const Minim::AudioFormat & getFormat() const { return m_audioFormat; }

	// AudioStream implementation
	virtual void read( Minim::MultiChannelBuffer & buffer );

	// AudioRecordingStream implementation
	virtual void play();
	virtual void pause();
	virtual bool isPlaying() const { return m_bPlaying && m_sndFile; }
	virtual unsigned int bufferSize() const { return m_bufferSize; }
	virtual void loop(const int count) {}
	virtual void setLoopPoints(const unsigned int start, const unsigned int stop) {}
	virtual unsigned int getLoopCount() const { return 0; }
	virtual unsigned int getMillisecondPosition() const { return m_millisPosition; }
	virtual void setMillisecondPosition(const unsigned int pos);
	virtual int getMillisecondLength() const { return m_millisLength; }
	virtual long getSampleFrameLength() const;
	virtual unsigned long getSampleFramePosition() const;
	virtual const Minim::AudioMetaData & getMetaData() const { return m_metaData; }

private:
	class mpg123Format : public Minim::AudioFormat
	{
	public:
		mpg123Format() : Minim::AudioFormat()
		{
		}

		void setFormat( mpg123_handle* );
	};

	mpg123Format m_audioFormat;

	class SFMetaData : public Minim::AudioMetaData
	{
	public:
		SFMetaData( mpg123AudioRecordingStream* stream )
		: m_stream(stream)
		{
		}

		virtual int length() const { return m_stream->m_millisLength; }
		virtual const char * fileName() const { return m_stream->m_filePath.c_str(); }

		mpg123AudioRecordingStream* m_stream;
	};

	SFMetaData m_metaData;

	// the file handle
	mpg123_handle*	m_sndFile;

	std::string		m_filePath;
	int				m_bufferSize;

	// the buffer we read into
	float  *		m_readBuffer;
	unsigned int	m_readBufferLength;

	// are we playing
	bool			m_bPlaying;

	// properties
	int				m_millisLength;
	int				m_millisPosition;
};

#endif // MPG123AUDIORECORDINGSTREAM_H