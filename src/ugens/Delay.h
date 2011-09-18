/*
 *  Delay.h
 *  MinimOSX
 *
 *  Created by Damien Di Fede on 9/17/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DELAYUGEN_H
#define DELAYUGEN_H

#include "UGen.h"

namespace Minim
{

	/**
	 * The Delay UGen is used to create delayed repetitions of the input audio.
	 * One can control the delay time and amplification of the repetition.
	 * One can also choose whether the repetition is fed back and/or the input is passed through.
	 * @author J Anderson Mills III
	 */
	class Delay : public UGen
	{
	public:
		
		/** 
		 * audio is the incoming audio
		 */
		UGenInput audio;
		
		/**
		 * delTime is the time for delay between repetitions.
		 */
		UGenInput delTime;
		
		/**
		 * delAmp is the strength of each repetition compared to the previous.
		 */
		UGenInput delAmp;
		
		/**
		 * Constructor for Delay.
		 * @param maxDelayTime
		 * 		is the maximum delay time for any one echo and the default echo time. 
		 * @param amplitudeFactor
		 *      is the amplification factor for feedback and should generally be from 0 to 1.
		 * @param feedBackOn
		 * 		is a boolean flag specifying if the repetition continue to feed back.
		 * @param passAudioOn
		 * 	 	is a boolean value specifying whether to pass the input audio to the output as well.
		 */
		explicit Delay( float maxDelayTime = 0.25f, float amplitudeFactor = 0.5f, bool feedBackOn = false, bool passAudioOn = true );
		virtual ~Delay();
		
	protected:
		
		/**
		 * When the sample rate is changed the buffer needs to be resized.
		 * Currently this causes the allocation of a completely new buffer, but 
		 * since a change in sampleRate will result in a change in the playback
		 * speed of the sound in the buffer, I'm okay with this.
		 */
		virtual void sampleRateChanged();
		
	private:
		
		// Recalculate the new bufferSize and make sure to clear out old data.
		void bufferSizeChanged();
		
		
	public:
		
		/**
		 * changes the time in between the echos to the value specified.
		 * @param delTime
		 * 		It can be up to the maxDelayTime specified.
		 * 		The lowest it can be is 1/sampleRate.	
		 */
		inline void setDelTime( float inDelayTime )
		{
			delayTime = inDelayTime;
			delTime.setLastValue( delayTime );
			bufferSizeChanged();
		}
		
		/**
		 * changes the feedback amplification of the echos.
		 * @param delayAmplitude
		 * 		This should normally be between 0 and 1 for decreasing feedback.
		 * 		Phase inverted feedback can be generated with negative numbers, but each echa will be the inverse
		 * 		of the one before it.
		 */
		inline void setDelAmp( float delayAmplitude )
		{
			amplitudeFactor = delayAmplitude;
			delAmp.setLastValue( delayAmplitude );
		}
		
	protected:
		
		virtual void uGenerate(float * channels, const int numChannels);
		
	private:
		
		// current delay time
		float delayTime;
		// maximum delay time
		float maxDelayTime;
		// current feedback factor
		float amplitudeFactor;
		// the delay buffer based on maximum delay time
		double * delayBuffer;
		// the bufferSize stored for convenience
		int bufferSize;
		// the index of the input and output of the buffer
		int iBufferIn, iBufferOut;
		// flag to include continual feedback.
		bool feedBackOn;
		// flag to pass the audio straight to the output.
		bool passAudioOn;
	};

}

#endif // DELAYUGEN_H