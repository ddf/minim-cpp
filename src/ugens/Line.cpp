/*
 *  Line.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/23/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Line.h"

namespace Minim
{
	Line::Line( const float transitionTime, const float beginAmplitude, const float endAmplitude )
	: UGen()
	, m_lineTime( transitionTime )
	, m_begAmp( beginAmplitude )
	, m_endAmp( endAmplitude )
	, m_amp( beginAmplitude )
	, m_lineNow( 0.f )
	, m_bIsActivated( false )
	{
	}
    
    Line::~Line()
    {
        
    }
	
	void Line::activate()
	{
		m_lineNow = 0.f;
		m_amp = m_begAmp;
		m_bIsActivated = true;
	}
	
	void Line::activate( const float transitionTime, const float beginAmplitude, const float endAmplitude )
	{
		m_begAmp = beginAmplitude;
		m_endAmp = endAmplitude;
		m_lineTime = transitionTime;
		activate();
	}
	
	void Line::sampleRateChanged()
	{
		m_timeStepSize = 1.f / sampleRate();
	}
	
	void Line::uGenerate( float * sampleFrame, const int numChannels )
	{
		if ( !m_bIsActivated )
		{
			fill(sampleFrame, m_begAmp, numChannels);
		}
		else if ( m_lineNow >= m_lineTime )
		{
			fill(sampleFrame, m_endAmp, numChannels);
		}
		else 
		{
			m_amp += ( m_endAmp - m_amp ) * m_timeStepSize / ( m_lineTime - m_lineNow );
			fill(sampleFrame, m_amp, numChannels);
			m_lineNow += m_timeStepSize;
		}

	}
};