/*
 *  AudioRecorder.h
 *  Minim
 *
 *  Created by Damien Di Fede on 6/24/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

namespace Minim 
{
	class AudioSource;
	class SampleRecorder;

	class AudioRecorder
	{
	public:
		
		AudioRecorder( AudioSource * recordSource, SampleRecorder * recorder );
		~AudioRecorder();
		
		
	};
}

#endif // AUDIORECORDER_H

