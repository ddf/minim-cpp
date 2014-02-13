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

#ifndef LIBSNDSAMPLERECORDER_H
#define LIBSNDSAMPLERECORDER_H

#include "SampleRecorder.h"
#include "AudioSource.h"
#include "sndfile.h"
#include <string>

class libsndSampleRecorder : public Minim::SampleRecorder
{
public:
	libsndSampleRecorder( Minim::AudioSource* sourceToRecord, const char * recordingPath );
	virtual ~libsndSampleRecorder(void);

	// AudioListener implementation
	virtual void samples( const Minim::MultiChannelBuffer & buffer );

	// SampleRecoder implementation
	virtual const char * filePath() const { return m_filePath.c_str(); }
	virtual void beginRecord();
	virtual void endRecord();
	virtual bool isRecording() const { return m_bRecording; }
	virtual void save();

private:
	void openFile();
	void closeFile();
	
	// information about the file like format, channels, frames, etc.
	SF_INFO		m_fileInfo;
	// the file handle
	SNDFILE*	m_sndFile;

	std::string m_filePath;

	// the buffer we read into
	float  *		m_writeBuffer;
	unsigned int	m_writeBufferLength;
	bool			m_bRecording;
};

#endif // LIBSNDSAMPLERECORDER_H