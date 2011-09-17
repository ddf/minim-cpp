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

#ifndef LIBSNDAUDIORECORDINGSTREAM_H
#define LIBSNDAUDIORECORDINGSTREAM_H

#include "AudioRecordingStream.h"
#include "sndfile.h"
#include <string>

class libsndAudioRecordingStream : public Minim::AudioRecordingStream
{
public:
	libsndAudioRecordingStream( const char * filePath, const int bufferSize );
	virtual ~libsndAudioRecordingStream();

	// AudioResource implementation
	virtual void open();
	virtual void close();
	virtual const Minim::AudioFormat & getFormat() const { return m_audioFormat; }

	// AudioStream implementation
	virtual void read( Minim::MultiChannelBuffer & buffer );

	// AudioRecordingStream implementation
	virtual void play();
	virtual void pause();
	virtual bool isPlaying() const { return m_bPlaying; }
	virtual unsigned int bufferSize() const { return m_bufferSize; }
	virtual void loop(const unsigned int count) {}
	virtual void setLoopPoints(const unsigned int start, const unsigned int stop) {}
	virtual unsigned int getLoopCount() const { return 0; }
	virtual unsigned int getMillisecondPosition() const { return m_millisPosition; }
	virtual void setMillisecondPosition(const unsigned int pos);
	virtual int getMillisecondLength() const { return m_millisLength; }
	virtual long getSampleFrameLength() const;
	virtual const Minim::AudioMetaData & getMetaData() const { return m_metaData; }

private:
	class SFInfoFormat : public Minim::AudioFormat
	{
	public:
		SFInfoFormat() : Minim::AudioFormat()
		{
		}

		void setFromSFInfo( const SF_INFO & fileInfo );
	};

	SFInfoFormat m_audioFormat;

	class SFMetaData : public Minim::AudioMetaData
	{
	};

	SFMetaData m_metaData;

	// information about the file like format, channels, frames, etc.
	SF_INFO		m_fileInfo;
	// the file handle
	SNDFILE*	m_sndFile;

	std::string m_filePath;
	int			m_bufferSize;

	// the buffer we read into
	float  *		m_readBuffer;
	unsigned int	m_readBufferLength;

	// are we playing
	bool			m_bPlaying;

	// properties
	int				m_millisLength;
	int				m_millisPosition;
};

#endif // LIBSNDAUDIORECORDINGSTREAM_H