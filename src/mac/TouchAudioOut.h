//
//  TouchAudioOut.h
//  MinimTouch
//
//  Created by Damien Di Fede on 3/7/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#ifndef	TOUCHAUDIOOUT_H
#define	TOUCHAUDIOOUT_H

#include "AudioOut.h"
#include "MultiChannelBuffer.h"
#import	 <AudioUnit/AUComponent.h>

class TouchAudioOut : public Minim::AudioOut
{
public:
	TouchAudioOut( const Minim::AudioFormat & format, int bufferSize );
	virtual ~TouchAudioOut();
	
	// AudioResource implementation
	virtual void open();
	virtual void close();
	virtual const Minim::AudioFormat & getFormat() const { return mAudioFormat; }
	
	// AudioOut implementation
	virtual const int bufferSize() const { return mBuffer.getBufferSize(); }
	
	virtual void setAudioStream( Minim::AudioStream * stream );
	virtual void setAudioListener( Minim::AudioListener * listen );
	
	virtual const Minim::MultiChannelBuffer & getOutputBuffer() const { return mBuffer; }
	
	virtual void pauseProcessing();
	virtual void resumeProcessing();
	
private:
	
	static OSStatus renderCallback( void                        *inRefCon,
								    AudioUnitRenderActionFlags  *ioActionFlags,
								    const AudioTimeStamp        *inTimeStamp,
								    UInt32                      inBusNumber,
									UInt32                      inNumberFrames,
								    AudioBufferList             *ioData
								   );
	
	AudioComponentInstance		mAudioUnit;
	AudioStreamBasicDescription mStreamDesc;
	
	Minim::AudioFormat mAudioFormat;
	
	// we'll reuse this in the render callback to read from the stream
	Minim::MultiChannelBuffer mBuffer;
	Minim::AudioStream		*mStream;
	Minim::AudioListener	*mListener;
	
};

#endif // TOUCHAUDIOOUT_H