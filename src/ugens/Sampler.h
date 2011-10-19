/*
 *  Sampler.h
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/18/11.
 *  Copyright 2011 Damien Di Fede. All rights reserved.
 *
 */

#ifndef SAMPLERUGEN_H
#define SAMPLERUGEN_H

#include "UGen.h"
#include "MultiChannelBuffer.h"

namespace Minim 
{
	class Sampler : public UGen
	{
	public:
		/** Construct a Sampler by copying the provided sample data into 
		 *  the internal buffer and allowing for a particular number of voices.
		 */
        Sampler( const int maxVoices );
		Sampler( const MultiChannelBuffer & sampleData, const int maxVoices );
		virtual ~Sampler();
		
		// the starting sample in the sample data we should use when triggering
		UGenInput begin;
		
		// the ending sample in the sample data we should use when trigger
		UGenInput end;
		
		// the attack time, in seconds, for triggers
		UGenInput attack;
		
		// overall volume control
		UGenInput amplitude;
		
		// playback rate used when triggering
		UGenInput rate;
		
		// whether this is a looping sampler or not
		// changing this affects all playing triggers
		bool looping;
		
		/** Triggers the Sampler using the current values of the inputs.
		 */
		void trigger();
        
        /** stop all active triggers */
        void stop();
        
        void setSample( const MultiChannelBuffer & sampleData );
		
	protected:
		
		void uGenerate( float * sampleFrame, const int numChannels );
		
	private:
		
		/** Represents a voice of the Sampler. In our list of Triggers,
		 *  some will be playing, others will be available.
		 */
		class Trigger
		{
		public:
			Trigger()
			: m_sampler( 0 )
			, m_done( true )
			{
			}
			
			void setSampler( Sampler * pOwner ) { m_sampler = pOwner; }
			
			// start this Trigger playing with the current settings of the Sampler
			void activate();
            
            // stop this trigger
            void stop();
			
			// generate one sample frame of data
			void generate( float * sampleFrame, const int numChannels );
			
		private:
			// the sampler that owns this trigger
			Sampler * m_sampler;
			// begin and end sample numbers
			float m_beginSample;
			float m_endSample;
			// playback rate
			float m_rate;
			// what sample we are at in our trigger. expressed as a float to handle variable rate.
			float m_sample;
			// how many output samples we have generated, tracked for attack/release
			float m_outSampleCount;
			// attack time, in samples
			int   m_attackLength;
			// current amplitude mod for attack
			float m_attackAmp;
			// how much to increase the attack amp each sample frame
			float m_attackAmpStep;
			// release time, in samples
			int   m_release;
			// whether we are done playing our bit of the sample or not
			bool  m_done;
		};
		
		// our voices;
		Trigger * m_triggers;
		// how many there are
		int	      m_maxTriggers;
		// what's the next one we should use
		int       m_nextTrigger;
		
		// the sample data we are playing.
		MultiChannelBuffer m_sampleData;
		
		
	};
}


#endif // SAMPLERUGEN_H