//
//  TouchServiceProvider.mm
//  MinimTouch
//
//  Created by Damien Di Fede on 3/6/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "TouchServiceProvider.h"
#include "TouchAudioOut.h"
#import  <AudioToolbox/AudioServices.h>
#include "Wavetable.h"

extern void displayErrorAndExit( NSString* message, OSStatus status );

TouchServiceProvider::TouchServiceProvider( float preferredBufferSize )
{
	OSStatus status;
	
	status = AudioSessionInitialize(NULL, NULL, NULL, NULL);
	
	if ( status )
	{
		displayErrorAndExit( @"Couldn't initialize an audio sesson.", status );
	}
	
	status = AudioSessionSetProperty( kAudioSessionProperty_PreferredHardwareIOBufferDuration, 
									 sizeof(preferredBufferSize), 
									 &preferredBufferSize
									 );
	
	if ( status )
	{
		displayErrorAndExit( @"Couldn't set the preferred hardware IO buffer duration.", status );
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
	
	if ( preferredBufferSize != checkBufferSize )
	{
		displayErrorAndExit( @"Preferred buffer size not equal to actual buffer size!", 0 );
	}
	
	// We really need to do the no-interp optimization on iPhone!
	Minim::Wavetable::s_opt = true;
}


Minim::AudioOut * TouchServiceProvider::getAudioOutput( const Minim::AudioFormat & outputFormat, int outputBufferSize )
{
	// TODO return an instance of a class that wraps the functionality required 
	//      to make audio output work. AudioOutput in mobilesynth is a good place to start	
	return new TouchAudioOut( outputFormat, outputBufferSize );
}
