/*
 *  TickRate.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 1/23/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "TickRate.h"
#include <string.h> // for memset, memcpy

namespace Minim
{
	TickRate::TickRate( const float tickRate )
	: UGen()
	, m_pAudio(0)
	, value( *this, CONTROL )
	, m_currentSampleFrame( new float[1] ) // start mono, same as ugen
	, m_nextSampleFrame( new float[1] )
	, m_sampleFrameSize(1)
	, m_sampleCount( 0.f )
	, m_bInterpolate( false )
	{
		value.setLastValue(tickRate);
	}
	
	TickRate::~TickRate()
	{
		delete [] m_currentSampleFrame;
		delete [] m_nextSampleFrame;
	}
	
	// override
	void TickRate::addInput( UGen * in )
	{
		m_pAudio = in;
		m_pAudio->setAudioChannelCount( m_sampleFrameSize );		
	}
	
	// override
	void TickRate::removeInput( UGen * in )
	{
		if ( m_pAudio == in )
		{
			m_pAudio = NULL;
		}
	}
	
	// override
	void TickRate::sampleRateChanged()
	{
		if ( m_pAudio )
		{
			m_pAudio->setSampleRate( sampleRate() );
		}
	}
	
	// override
	void TickRate::channelCountChanged()
	{
		if ( m_sampleFrameSize < getAudioChannelCount() )
		{
			delete [] m_currentSampleFrame;
			delete [] m_nextSampleFrame;
			
			m_currentSampleFrame = new float[ getAudioChannelCount() ];
			m_nextSampleFrame = new float[ getAudioChannelCount() ];
			
			if ( m_pAudio )
			{
				m_pAudio->setAudioChannelCount( getAudioChannelCount() );
				m_pAudio->tick( m_currentSampleFrame, getAudioChannelCount() );
				m_pAudio->tick( m_nextSampleFrame, getAudioChannelCount() );
				m_sampleCount = 0.f;
			}
		}
		
		m_sampleFrameSize = getAudioChannelCount();
	}
	
	// override
	void TickRate::uGenerate(float * channels, const int numChannels ) 
	{
		float sampleStep = value.getLastValue();
		
		// for 0 or negative rate values, we just stop generating audio
		// effectively pausing generation of the patched ugen.
		if ( sampleStep <= 0.f )
		{
			memset(channels, 0, sizeof(float)*numChannels);
			
			return;
		}
		
		if ( m_bInterpolate )
		{
			for(int i = 0; i < numChannels; ++i)
			{
				float sampleDiff = m_nextSampleFrame[i] - m_currentSampleFrame[i];
				channels[i] = m_currentSampleFrame[i] + sampleDiff * m_sampleCount;
			}
		}
		else
		{
			memcpy(channels, m_currentSampleFrame, sizeof(float)*numChannels);
		}
		
		if ( m_pAudio )
		{
			m_sampleCount += sampleStep;
			
			while( m_sampleCount >= 1.f )
			{
				memcpy(m_currentSampleFrame, m_nextSampleFrame, sizeof(float)*m_sampleFrameSize);
				m_pAudio->tick(m_nextSampleFrame, m_sampleFrameSize);
				m_sampleCount -= 1.f;
			}
		}
	}
	
}