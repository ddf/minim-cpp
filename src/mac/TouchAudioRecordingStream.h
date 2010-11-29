/*
 *  TouchAudioRecordingStream.h
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 11/25/10.
 *  Copyright 2010 Compartmental. All rights reserved.
 *
 */


#ifndef	TOUCHAUDIORECORDINGSTREAM_H
#define TOUCHAUDIORECORDINGSTREAM_H

#include "AudioRecordingStream.h"
#include <AudioToolbox/AudioToolbox.h>
#include "CAStreamBasicDescription.h"
#include "AudioFormat.h"
#include "TouchAudioFormat.h"
#include "AudioMetaData.h"

class TouchAudioRecordingStream : public Minim::AudioRecordingStream
{
public:
	
	// bufferSize should be the size of the buffer you plan to pass to read
	// so that this can allocate an appropiately sized byte buffer.
	TouchAudioRecordingStream( CFURLRef fileURL, int bufferSize );
	virtual ~TouchAudioRecordingStream();
	
	// AudioResource interface
	virtual void open();
	virtual void close();
	virtual const Minim::AudioFormat & getFormat() const { return m_fileFormat; }
	
	// AudioStream interface
	virtual void read( Minim::MultiChannelBuffer & buffer );
	
	// AudioRecordingStream interface
	virtual void play() { m_bIsPlaying = true; }
	virtual void pause() { m_bIsPlaying = false; }
	virtual bool isPlaying() const { return m_bIsPlaying; }
	
	virtual unsigned int bufferSize() const { return m_bufferSize; }
	
	virtual void loop(const unsigned int count) {}
	virtual void setLoopPoints(const unsigned int start, const unsigned int stop) {}
	virtual unsigned int getLoopCount() const { return 0; }
	
	virtual unsigned int getMillisecondPosition() const;
	virtual void setMillisecondPosition(const unsigned int pos);
	virtual unsigned int getMillisecondLength() const { return m_fileMillisLength; }
	
	virtual const Minim::AudioMetaData & getMetaData() const { return m_metaData; }
	
private:
	
	class TARSMetaData : public Minim::AudioMetaData
	{
	public:
		TARSMetaData() : Minim::AudioMetaData() {}
	};
	
	TARSMetaData m_metaData;
	
	// we use this to open the ExtAudioFile
	CFURLRef m_fileURL;
		
	// the ref to our audio file
	ExtAudioFileRef m_audioFileRef;
		
	// the format of our audio file
	TouchAudioFormat m_fileFormat;
		
	// the format we want the file read into
	CAStreamBasicDescription m_clientFormat;
		
	// are we "playing" or not
	bool m_bIsPlaying;
	
	// how many sample frames we expect to be asked to read at a time
	UInt32   m_bufferSize;
	// where we read the data to
	SInt16 * m_readBuffer;
	
	// how long is this file, in milliseconds
	UInt32   m_fileMillisLength;
};

#endif // TOUCHAUDIORECORDINGSTREAM_H
