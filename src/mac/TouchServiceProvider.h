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

#if TARGET_OS_IPHONE
	struct AudioSessionParameters
	{
        // default argument means we won't try to set the output buffer duration when starting the audio session.
		AudioSessionParameters( float inBufferDuration = -1.0f )
		: interruptRunLoop(NULL)
		, interruptRunLoopMode(NULL)
		, interruptListener(NULL)
		, interruptUserData(NULL)
		, outputBufferDuration( inBufferDuration )
		, audioCategory(kAudioSessionCategory_MediaPlayback)
		, setActiveImmediately(true)
		{
		}
        
        AudioSessionParameters( float inBufferDuration, CFRunLoopRef inRunLoop, CFStringRef inRunLoopMode, AudioSessionInterruptionListener inListener, void* inUserData, UInt32 inAudioCategory, bool inActiveImmediately )
        : interruptRunLoop(inRunLoop )
        , interruptRunLoopMode( inRunLoopMode )
        , interruptListener( inListener )
        , interruptUserData( inUserData )
        , outputBufferDuration( inBufferDuration )
        , audioCategory( inAudioCategory )
        , setActiveImmediately( inActiveImmediately )
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
#endif	

	virtual ~TouchServiceProvider() {}
	
	virtual void start() {}
	
	virtual void stop();
	
	virtual void debugOn() {}

	virtual void debugOff() {}

	virtual Minim::AudioRecordingStream * getAudioRecordingStream( const char * filename, int bufferSize, bool inMemory );
	
	virtual Minim::AudioStream * getAudioInput( const Minim::AudioFormat & inputFormat ) { return NULL; }
	
	virtual Minim::AudioOut * getAudioOutput( const Minim::AudioFormat & outputFormat, int outputBufferSize );
	
	virtual Minim::AudioSample * getAudioSample( const char * filename, int bufferSize ) { return NULL; }
	
	virtual Minim::AudioSample * getAudioSample( Minim::MultiChannelBuffer * samples, const Minim::AudioFormat & format, int bufferSize ) { return NULL; }
	
	virtual Minim::SampleRecorder * getSampleRecorder( Minim::AudioSource * sourceToRecord, const char * fileName, const bool buffered );
	
};

#endif // TOUCHSERVICEPROVIDER_H