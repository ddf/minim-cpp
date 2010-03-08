//
//  TouchServiceProvider.mm
//  MinimTouch
//
//  Created by Damien Di Fede on 3/6/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "TouchServiceProvider.h"
#include "TouchAudioOut.h"


Minim::AudioOut * TouchServiceProvider::getAudioOutput( const Minim::AudioFormat & outputFormat, int outputBufferSize )
{
	// TODO return an instance of a class that wraps the functionality required 
	//      to make audio output work. AudioOutput in mobilesynth is a good place to start	
	return new TouchAudioOut( outputFormat, outputBufferSize );
}
