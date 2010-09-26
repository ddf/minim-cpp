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
#import  <AudioToolbox/AudioServices.h>

class TouchServiceProvider : public Minim::ServiceProvider
{

public:
	
	struct AudioSessionParameters
	{
		AudioSessionParameters()
		: interruptRunLoop(NULL)
		, interruptRunLoopMode(NULL)
		, interruptListener(NULL)
		, interruptUserData(NULL)
		, outputBufferDuration(-1.f) // means we won't attempt to set the property
		, audioCategory(kAudioSessionCategory_MediaPlayback)
		, setActiveImmediately(true)
		{
		}
		
		
		// arguments for AudioSessionInitialize
		CFRunLoopRef                      interruptRunLoop;		
		CFStringRef                       interruptRunLoopMode; 
		AudioSessionInterruptionListener  interruptListener;    
		void                              *interruptUserData;
		
		// for setting the preferred IO buffer duration
		float							  outputBufferDuration; 
		
		// for setting the AudioCategory for this audio session
		UInt32							  audioCategory;		
		
		// to indicate whether or not the audio session should be set active immediately
		bool                              setActiveImmediately;  
		
	};
	
	// this will set the outputBufferDuration member of the provided parameters
	// to what the device actually chooses, which might be different than what you ask for.
	TouchServiceProvider( AudioSessionParameters & rParameters );
	virtual ~TouchServiceProvider() {}
	
	virtual void start() {}
	
	virtual void stop();
	
	virtual void debugOn() {}

	virtual void debugOff() {}

	virtual Minim::AudioRecordingStream * getAudioRecordingStream( const char * filename, int bufferSize, bool inMemory ) { return NULL; }
	
	virtual Minim::AudioStream * getAudioInput( const Minim::AudioFormat & inputFormat ) { return NULL; }
	
	virtual Minim::AudioOut * getAudioOutput( const Minim::AudioFormat & outputFormat, int outputBufferSize );
	
	virtual Minim::AudioSample * getAudioSample( const char * filename, int bufferSize ) { return NULL; }
	
	virtual Minim::AudioSample * getAudioSample( Minim::MultiChannelBuffer * samples, const Minim::AudioFormat & format, int bufferSize ) { return NULL; }
	
};

#endif // TOUCHSERVICEPROVIDER_H