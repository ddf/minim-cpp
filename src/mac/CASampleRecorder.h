/*
 *  CASampleRecorder.h
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/24/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef	CASAMPLERECORDER_H
#define CASAMPLERECORDER_H

#include "SampleRecorder.h"
#include "CAStreamBasicDescription.h"
#include <AudioToolbox/AudioToolbox.h>
#include "AudioFormat.h"

@class NSString;

class CASampleRecorder : public Minim::SampleRecorder
{
public:
	CASampleRecorder( NSString * pathToFile, const Minim::AudioFormat & sourceFormat, const int bufferSize );
	virtual ~CASampleRecorder();
	
	//--------------------------------------
	// SampleRecorder Interface
	//--------------------------------------
	virtual const char * filePath() const;
	
	virtual void beginRecord();
	virtual void endRecord();
	virtual bool isRecording() const;
	
	virtual void save();
	
	//---------------------------------------
	// AudioListener Interface
	//---------------------------------------
	virtual void samples( const class Minim::MultiChannelBuffer & buffer );
	
private:
	
	// name of the file.
	NSString *			m_filePath;
	
	CAStreamBasicDescription m_sourceFormat;
	
	// actual file we are writing to.
	ExtAudioFileRef		m_audioFile;
	
	// are we currently recording
	bool			    m_bRecording;
	
	// how many sample frames we expect to be asked to write at a time
	UInt32   m_bufferSize;
	// where we write the data to
	SInt16 * m_writeBuffer;
};

#endif // CASAMPLERECORDER_H