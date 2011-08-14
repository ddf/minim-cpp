/*
 *  Line.h
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/23/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "UGen.h"

namespace Minim
{
	class Line : public UGen 
	{
	public:
		
		explicit Line( const float transitionTime = 1.0f, const float beginAmplitude = 1.0f, const float endAmplitude = 0.f );
		
		/**
		 * Start the Line's transition.
		 */
		void activate();
		
		/**
		 * Set the Line's parameters and start the transition.
		 */
		void activate( const float transitionTime, const float beginAmplitude, const float endAmplitude );
		
		/**
		 * Has the Line completed its transition.
		 */
		inline bool isAtEnd() const { return (m_lineNow >= m_lineTime); }
		
		/**
		 * Set the value for the ending amplitude.
		 */
		inline void setEndAmp( const float newEndAmplitude ) { m_endAmp = newEndAmplitude; }
		
		/**
		 * Set the transition time, in seconds.
		 */
		inline void setLineTime( const float newLineTime ) { m_lineTime = newLineTime; }
		
		/**
		 * Get the current value of this Line.
		 */
		inline float getAmp() const { return m_amp; }
		
	protected:
		
		virtual void sampleRateChanged();
		
		virtual void uGenerate( float * sampleFrame, const int numChannels );
		
	private:
		
		float m_amp;
		float m_begAmp;
		float m_endAmp;
		bool  m_bIsActivated;
		float m_lineNow;
		float m_lineTime;
		float m_timeStepSize;
	};
};