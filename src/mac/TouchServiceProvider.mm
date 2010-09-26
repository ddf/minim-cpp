//
//  TouchServiceProvider.mm
//  MinimTouch
//
//  Created by Damien Di Fede on 3/6/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "TouchServiceProvider.h"
#include "TouchAudioOut.h"
#include "Wavetable.h"

extern void displayErrorAndExit( NSString* message, OSStatus status );

TouchServiceProvider::TouchServiceProvider( AudioSessionParameters & rParameters )
{
	OSStatus status;
	
	status = AudioSessionInitialize( rParameters.interruptRunLoop, 
									 rParameters.interruptRunLoopMode, 
									 rParameters.interruptListener, 
									 rParameters.interruptUserData
									);
	
	if ( status )
	{
		displayErrorAndExit( @"Couldn't initialize an audio sesson.", status );
	}
	
	UInt32 category = rParameters.audioCategory;
	status = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
	
	if ( status )
	{
		displayErrorAndExit(@"Couldn't set the AudioCategory to MediaPlayback.", status);
	}
	
	if ( rParameters.outputBufferDuration > 0.f )
	{
		Float32 preferredBufferSize = rParameters.outputBufferDuration;
		status = AudioSessionSetProperty( kAudioSessionProperty_PreferredHardwareIOBufferDuration, 
										 sizeof(preferredBufferSize), 
										 &preferredBufferSize
										 );
	
		if ( status )
		{
			displayErrorAndExit( @"Couldn't set the preferred hardware IO buffer duration.", status );
		}
	}
	
	Float32 checkBufferSize(0.f);
	UInt32	dataSize = sizeof(checkBufferSize);
	status = AudioSessionGetProperty( kAudioSessionProperty_PreferredHardwareIOBufferDuration,
									 &dataSize,
									 &checkBufferSize
									 );
	
	if ( status )
	{
		displayErrorAndExit( @"Couldn't get the preferred hardware IO buffer duration.", status );
	}
	
	rParameters.outputBufferDuration = checkBufferSize;
	
	if ( rParameters.setActiveImmediately )
	{
		status = AudioSessionSetActive(true);
	
		if ( status )
		{
			displayErrorAndExit(@"Couldn't set the audio session active!", status);
		}
	}
	
	// We really need to do the no-interp optimization on iPhone!
	Minim::Wavetable::s_opt = true;
}

////////////////////////////////////////////////////////////////
void TouchServiceProvider::stop()
{
	AudioSessionSetActive(false);
}


Minim::AudioOut * TouchServiceProvider::getAudioOutput( const Minim::AudioFormat & outputFormat, int outputBufferSize )
{
	return new TouchAudioOut( outputFormat, outputBufferSize );
}
