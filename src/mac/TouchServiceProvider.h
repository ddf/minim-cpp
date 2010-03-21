//
//  TouchServiceProvider.h
//  MinimTouch
//
//  Created by Damien Di Fede on 3/6/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#ifndef TOUCHSERVICEPROVIDER_H
#define TOUCHSERVICEPROVIDER_H

#include "ServiceProvider.h"

class TouchServiceProvider : public Minim::ServiceProvider
{

public:
	TouchServiceProvider( const float outputBufferDuration );
	virtual ~TouchServiceProvider() {}
	
	virtual void start() {}
	
	virtual void stop() {}
	
	virtual void debugOn() {}

	virtual void debugOff() {}

	virtual Minim::AudioRecordingStream * getAudioRecordingStream( const char * filename, int bufferSize, bool inMemory ) { return NULL; }
	
	virtual Minim::AudioStream * getAudioInput( const Minim::AudioFormat & inputFormat ) { return NULL; }
	
	virtual Minim::AudioOut * getAudioOutput( const Minim::AudioFormat & outputFormat, int outputBufferSize );
	
	virtual Minim::AudioSample * getAudioSample( const char * filename, int bufferSize ) { return NULL; }
	
	virtual Minim::AudioSample * getAudioSample( Minim::MultiChannelBuffer * samples, const Minim::AudioFormat & format, int bufferSize ) { return NULL; }
	
};

#endif // TOUCHSERVICEPROVIDER_H