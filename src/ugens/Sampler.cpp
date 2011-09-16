/*
 *  Sampler.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/18/11.
 *  Copyright 2011 Damien Di Fede. All rights reserved.
 *
 */

#include "Sampler.h"
#include <string>
#include <cassert>
#include <algorithm>

namespace Minim 
{
	
	Sampler::Sampler( const MultiChannelBuffer & sampleData, const int maxVoices )
	: UGen( 5 ) 
	, begin( *this, CONTROL )
	, end( *this, CONTROL )
	, attack( *this, CONTROL )
	, amplitude( *this, CONTROL )
	, rate( *this, CONTROL )
	, looping( false )
	, m_sampleData( sampleData )
	, m_maxTriggers( maxVoices )
	, m_nextTrigger( 0 )
	, m_triggers( NULL )
	{
		begin.setLastValue( 0.f );
		end.setLastValue( (float)sampleData.getBufferSize()-1 );
		attack.setLastValue( 0.f );
		amplitude.setLastValue( 1.f );
		rate.setLastValue( 1.f );
		
		m_triggers = new Trigger[ maxVoices ];
		for( int i = 0; i < maxVoices; ++i )
		{
			m_triggers[i].setSampler( this );
		}
	}
	
	Sampler::~Sampler()
	{
		delete[] m_triggers;
	}
	
	void Sampler::trigger()
	{
		m_triggers[ m_nextTrigger++ ].activate();
		
		// wrap
		if ( m_nextTrigger == m_maxTriggers )
		{
			m_nextTrigger = 0;
		}
	}
	
	void Sampler::uGenerate( float * sampleFrame, const int numChannels )
	{
		memset(sampleFrame, 0, sizeof(float)*numChannels);
		
		for( int i = 0; i < m_maxTriggers; ++i )
		{
			m_triggers[i].generate( sampleFrame, numChannels );
		}
	}
	
	void Sampler::Trigger::activate()
	{
		assert( m_sampler );
		
		m_beginSample    = m_sampler->begin.getLastValue();
		m_endSample      = m_sampler->end.getLastValue();
		m_rate		     = m_sampler->rate.getLastValue();
		m_attackLength	 = (int)std::max(m_sampler->sampleRate() * m_sampler->attack.getLastValue(), 1.f);
		m_attackAmp	     = 0.0f;
		m_attackAmpStep  = 1.f / (float)m_attackLength;
		m_release        = 0;
		m_sample	     = m_beginSample;
		m_outSampleCount = 0;
		m_done			 = false;
	}
	
	void Sampler::Trigger::generate( float * sampleFrame, const int numChannels )
	{
		assert( m_sampler );
		
		if ( m_done ) return;
		
		const float outAmp = m_sampler->amplitude.getLastValue() * m_attackAmp;
		
		for( int c = 0; c < numChannels; ++c )
		{
			int sourceChannel = c < m_sampler->m_sampleData.getChannelCount() ? c : m_sampler->m_sampleData.getChannelCount() - 1;
			sampleFrame[c] += outAmp * m_sampler->m_sampleData.getSample( sourceChannel, m_sample );
		}
		
		m_sample += m_rate;
		
		if ( m_sample > m_endSample )
		{
			if ( m_sampler->looping ) 
			{
				m_sample -= m_endSample - m_beginSample;
			}
			else 
			{
				m_done = true;
			}
		}
		
		++m_outSampleCount;
		if ( m_outSampleCount <= m_attackLength )
		{
			m_attackAmp += m_attackAmpStep;
		}
	}
};